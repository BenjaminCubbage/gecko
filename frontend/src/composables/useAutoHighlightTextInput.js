import { 
    onUnmounted, 
    ref,
    watch
} from 'vue';

/* 
    Select the entirety of an input's content when it receives
    focus.

    The input elRefs should be able to be watch()ed. It can be a
    function. The result should be an HTMLInputElement or array
    of them.

    Current modes are 'all' and 'end'.
*/
export function useAutoHighlightTextInput(elRefs, mode = 'end') {
    const enabled = ref(true);
    const registeredEls = [];

    watch(elRefs, newValue => {
        const newEls = newValue instanceof Array 
            ? newValue
            : [newValue];

        /*
            This is mainly designed for small collections for now, so
            .includes is OK.
        */

        unregister(registeredEls.filter(e => !newEls.includes(e)));
        registeredEls.length = 0;
        registeredEls.push(...newEls);
        register(newEls);
    }, {
        immediate: true
    });

    onUnmounted(() => {
        unregister(registeredEls);
        registeredEls.length = 0;
    });

    function handleGotFocus(e) {
        if (enabled.value) {
            switch (mode) {
            case 'end':
                e.target.selectionStart = e.target.value.length;
                e.target.selectionEnd   = e.target.selectionStart;
                break;

            case 'all':
                e.target.select();
                break;
            }
        }
    }

    function register(els) {
        if (import.meta.env.DEV) {
            if (els.some(e => e && !(e instanceof HTMLInputElement))) {
                console.warn(`[useAutoHighlightTextInput]: Not all elements were of type HTMLInputElement`);
            }
        }
        
        for (const el of els.filter(e => e instanceof HTMLInputElement))
            el.addEventListener('focus', handleGotFocus);
    }

    function unregister(els) {
        for (const el of els.filter(e => e instanceof HTMLInputElement))
            el.removeEventListener('focus', handleGotFocus);
    }

    return {
        enabled
    };
}