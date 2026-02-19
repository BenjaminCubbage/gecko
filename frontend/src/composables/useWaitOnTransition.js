import {
    onUnmounted,
    readonly,
    ref,
    watch
} from 'vue';

/*
    Returns a ref which will be set to true once an element's
    transition is completed.

    You can optionally choose to only listen for the first
    event ("once"). In this case, you should never end up
    calling resetIsTransitionCompleted.
*/
export function useWaitOnTransition(elRef, { propertyName = null, once = false } = {}) {
    const transitionCompleted = ref(false);

    watch(elRef, (newValue, oldValue) => {
        if (once && transitionCompleted.value)
            return;

        if (oldValue != null) unregister(oldValue);
        if (newValue != null) register(newValue);
    });

    function handleTransitionEnd(e) {
        if (e.target !== elRef.value || propertyName !== e.propertyName)
            return;

        transitionCompleted.value = true;

        if (once)
            unregister(e.target);
    }

    function register(el) {
        el.addEventListener('transitionend', handleTransitionEnd);
    }

    function unregister(el) {
        el.removeEventListener('transitionend', handleTransitionEnd);
    }

    function resetIsTransitionCompleted() {
        transitionCompleted.value = false;
    }

    onUnmounted(() => {
        unregister(elRef.value);
    });

    return {
        isTransitionCompleted: readonly(transitionCompleted),
        resetIsTransitionCompleted
    };
}