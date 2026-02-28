import {
    onUnmounted,
    ref,
    watch
} from 'vue';

export function useIsFocusWithin(elRefs) {
    const registeredEls = new Set();
    const isFocusWithin = ref(false);

    watch(elRefs, (newValue, oldValue) => {
        for (const el of oldValue ?? []) {
            if (el) {
                unregister(el);
                registeredEls.delete(el);
            }
        }

        for (const el of newValue ?? []) {
            if (el) {
                register(el);
                registeredEls.add(el);
            }
        }
    });

    function handleFocusIn() {
        isFocusWithin.value = true;
    }

    function handleFocusOut(e) {
        const newFocusedEl = e.relatedTarget;

        for (const el of registeredEls)
            if (el.contains(newFocusedEl))
                return;

        isFocusWithin.value = false;
    }

    function register(el) {
        el.addEventListener('focusin',  handleFocusIn);
        el.addEventListener('focusout', handleFocusOut);
    }

    function unregister(el) {
        el.removeEventListener('focusin',  handleFocusIn);
        el.removeEventListener('focusout', handleFocusOut);
    }

    onUnmounted(() => {
        for (const el of registeredEls.values())
            unregister(el);

        registeredEls.clear();
    });

    return {
        isFocusWithin
    };
}