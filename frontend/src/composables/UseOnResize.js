export function useOnResize() {
    class Entry {
        constructor({ el, observer, handlers }) {
            this.el    = el;
            this.observer = observer;
            this.handlers = handlers;
        }
    }

    const entries = [];

    function getEntryByElement(el) { 
        return entries.find(o => o.el == el); 
    }

    function handleObserverEvent(el) {
        getEntryByElement(el)?.handlers?.forEach(h => h());
    }

    function addResizeHandler(el, cb) {
        let entry;

        if ((entry = getEntryByElement(el)) == null) {
            entries.push(entry = new Entry({
                el: el, 
                observer: new ResizeObserver(() => handleObserverEvent(el)),
                handlers: [cb]
            }));

            entry.observer.observe(el);
        } else {
            // note(ben): Is it correct to prevent duplicates?
            // We treat the cb as if it were an index on removal,
            // so it should probably be unique.
            if (!entries.handlers.includes(cb))
                entry.handlers.push(cb);
        }
    }

    function removeResizeHandler(el, cb) {
        let entry = getEntryByElement(el);

        if (!entry)
            return;

        if (entry.handlers?.length)
            handlers.splice(entry.handlers.indexOf(cb), 1);

        if (!entry.handlers?.length) {
            entry.observer.unobserve(el);
            entries.splice(entries.indexOf(entry), 1);
        }
    }

    return {
        addResizeHandler,
        removeResizeHandler
    };
}