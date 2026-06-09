function onFocus({ currentTarget }) {
    currentTarget
        .setSelectionRange(0, currentTarget.value.length);
}

/*
    Automatically highlight the text of an <input> or <textarea>
    when focused.
*/
export const autoHighlight = {
    mounted(el, { value = true }) {
        if (import.meta.env.DEV &&
            !(el instanceof HTMLInputElement) &&
            !(el instanceof HTMLTextAreaElement)) {
            console.error('[autoHighlight]: Element must be an <input> or <textarea>');
            return;
        }

        if (value)
            el.addEventListener('focus', onFocus);
    },

    updated(el, { value = true, oldValue = true }) {
        if (value !== oldValue)
            value
                ? el.addEventListener   ('focus', onFocus)
                : el.removeEventListener('focus', onFocus);
    },

    beforeUnmount(el) {
        el.removeEventListener('focus', onFocus);
    }
};