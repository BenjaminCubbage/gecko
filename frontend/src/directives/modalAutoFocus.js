import { nextTick } from 'vue';
import { delay }    from '@/core/async/delay.js';

const Attributes = {
    ModalAutoFocusTarget: 'data-modal-auto-focus-target'
};

function isAutoFocusTarget(el) {
    return el.hasAttribute(Attributes.ModalAutoFocusTarget);
}

async function dialogToggled({ currentTarget }) {
    if (!currentTarget.open)
        return;

    let fallbackTarget = null;
    let focusTarget = document.createTreeWalker(
        currentTarget,
        NodeFilter.SHOW_ELEMENT,
        el => {
            if (!fallbackTarget && el.tabIndex != -1)
                fallbackTarget = el;

            return isAutoFocusTarget(el)
                ? NodeFilter.FILTER_ACCEPT
                : NodeFilter.FILTER_SKIP;
        }
    ).nextNode() ?? fallbackTarget;

    focusTarget?.focus({
        focusVisible: false
    });
}

export const modalAutoFocus = {
    mounted(el, { value = true }) {
        el.addEventListener('toggle', dialogToggled);
    },

    updated(el, { value = true, oldValue }) {
        if (value === oldValue)
            return;

        (value ? el.addEventListener : el.removeEventListener)('toggle', dialogToggled);
    },

    beforeUnmount(el) {
        el.removeEventListener('toggle', dialogToggled);
    }
}

export function modalAutoFocusTarget(el, { value = true }) {
    value
        ? el.setAttribute(Attributes.ModalAutoFocusTarget, true)
        : el.removeAttribute(Attributes.ModalAutoFocusTarget);
}