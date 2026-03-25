/*
    Set the textContent of an element after a 200ms delay.

    The primary purpose of this directive is to allow for a
    just-mounted <output> or role="status" element to be
    announced by a screenreader.
*/

const timeoutSymbol = Symbol();

export const deferredContent = {
    mounted(el, { value }) {
        clearTimeout(el[timeoutSymbol]);
        if (value != null) {
            el[timeoutSymbol] = setTimeout(() => {
                el.textContent = value;
            }, 200);
        } else
            delete el[timeoutSymbol];
    },

    beforeUnmount(el) {
        clearTimeout(el[timeoutSymbol]);
    }
};