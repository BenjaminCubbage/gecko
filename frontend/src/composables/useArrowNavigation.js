import { 
    onUnmounted,
    watch
} from 'vue';

/*
    This could probably be more generalized eventually.

    Listen to arrow key events. Currently there is no
    way to unregister a handler.
*/
export function useArrowNavigation(elRefs) {
    const registeredEls = [];
    const handlers      = new Map();

    watch(elRefs, newValue => {
        if (!newValue)
            return;

        const newEls = newValue instanceof Array
            ? newValue
            : [newValue];

        unregister(registeredEls.filter(e => !newEls.includes(e)));
        registeredEls.length = 0;
        registeredEls.push(...newEls);
        register(newEls);
    }, {
        immediate: true
    });

    function handleKeyDown(e) {
        let anyHandlers = false;

        for (const handler of handlers.get(e.key) ?? []) {
            handler();
            anyHandlers = true;
        }

        if (anyHandlers)
            e.preventDefault();
    }

    function register(els) {
        for (const el of els)
            el.addEventListener('keydown', handleKeyDown);
    }

    function unregister(els) {
        for (const el of els)
            el.removeEventListener('keydown', handleKeyDown);
    }

    function registerHandler(key, handler) {
        const handlerList = handlers.get(key);

        if (handlerList)
            handlerList.push(handler);
        else
            handlers.set(key, [handler]);
    }

    onUnmounted(() => {
        unregister(registeredEls);
    });

    return {
        onArrowLeft(fn)  { registerHandler('ArrowLeft',  fn); },
        onArrowRight(fn) { registerHandler('ArrowRight', fn); },
        onArrowUp(fn)    { registerHandler('ArrowUp',    fn); },
        onArrowDown(fn)  { registerHandler('ArrowDown',  fn); }
    };
}