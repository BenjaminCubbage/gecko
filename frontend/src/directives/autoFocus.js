export const autoFocus = {
    mounted(el, { value = true }) {
        if (value)
            el.focus();
    }
};