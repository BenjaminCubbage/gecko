const clickListener   = Symbol();
const clickExceptions = Symbol();

function createClickListener(el, handler) {
    return function(e) {
        if (el.contains(e.target))
            return;

        for (let exception of el[clickExceptions] ?? [])
            if (exception.contains(e.target))
                return;

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
};

/*
    Dont fire if _these_ elements were clicked. Can be a single
    node or an array of nodes.
*/
export const clickedOutsideExcept = {
    mounted(el, { value }) {
        el[clickExceptions]
            = value instanceof Array ? value : [value];
    },

    updated(el, { value, oldValue }) {
        if (value === oldValue)
            return;

        el[clickExceptions]
            = value instanceof Array ? value : [value];
    },

    unmounted(el) {
        delete el[clickExceptions];
    }
};