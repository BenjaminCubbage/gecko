const clickListener = Symbol();

function createClickListener(el, handler) {
    return function(e) {
        if (!el.contains(e.target))
            handler(e);
    }
}

export const clickedOutside = {
    mounted(el, { value }) {
        document.addEventListener(
            'click', 
            el[clickListener] = createClickListener(el, value),
            true);
    },

    updated(el, { value, oldValue }) {
        if (value === oldValue)
            return;

        document.removeEventListener(
            'click', 
            el[clickListener],
            true);
        document.addEventListener(
            'click', 
            el[clickListener] = createClickListener(el, value),
            true);
    },

    beforeUnmount(el) {
        document.removeEventListener(
            'click', 
            el[clickListener],
            true);
        delete el[clickListener];
    }
}