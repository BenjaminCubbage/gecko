import {
    watch,
    onUnmounted
} from 'vue';

export function useRovingFocus(containerElRef, elsRef, prioritySelectionIndexRef = null) {
    let registeredContainer   = null;
    let registeredEls         = [];
    let currentFocusableIndex = 0;

    watch(containerElRef, newValue => {
        unregisterContainer(registerContainer);
        registeredContainer = newValue;
        registerContainer(registeredContainer);
    });

    watch(elsRef, newValue => {
        /*
            This is inefficient, maybe make this faster later?
        */
        registeredEls.forEach(unregisterEl);
        registeredEls = newValue;
        registeredEls.forEach(registerEl);
        checkTabIndices();
    });

    /*
        Make sure exactly one of the registered elements is
        focusable, then update currentFocusableIndex.

        Call this when the list of registered elements changes.
    */
    function checkTabIndices() {
        if (!registeredEls.length)
            return;

        if (registeredEls.length === 1) {
            registeredEls[0].tabIndex = 0;
            return;
        }

        let foundFocusable = false;

        for (const [el, index] of registeredEls.map((v, i) => [v, i])) {
            if (el.tabIndex === 0) {
                if (foundFocusable) {
                    el.tabIndex = -1;
                } else {
                    foundFocusable        = true;
                    currentFocusableIndex = index;
                }
            }
        }

        if (!foundFocusable) {
            if (prioritySelectionIndexRef?.value != null) {
                if (prioritySelectionIndexRef.value < 0 ||
                    prioritySelectionIndexRef.value >= registeredEls.length) {
                    throw new RangeError(
                        '[useRovingFocus]: prioritySelectionIndexRef was out of ' +
                        'range.');
                }

                currentFocusableIndex = prioritySelectionIndexRef.value;
                registeredEls[currentFocusableIndex].tabIndex = 0;
            } else {
                /*
                    Nearest neighbor of last focused element
                */
                currentFocusableIndex = Math.min(currentFocusableIndex, registeredEls.length);
                registeredEls[currentFocusableIndex].tabIndex = 0;
            }
        }
    }

    function containerFocusInCallback({ target }) {
        const index = registeredEls.indexOf(target);

        if (index !== -1 && index !== currentFocusableIndex) {
            /*
                Focus was set programmatically.
            */
            if (currentFocusableIndex >= registeredEls.length)
                throw new RangeError(
                    '[useRovingFocus]: currentFocusableIndex was greater than ' +
                    'number of registered elements.');

            if (registeredEls[currentFocusableIndex].tabIndex != 0)
                throw new Error(
                    '[useRovingFocus]: currentFocusableIndex was not actually ' +
                    'focusable.');

            registeredEls[currentFocusableIndex].tabIndex = -1;
            currentFocusableIndex = index;
            registeredEls[currentFocusableIndex].tabIndex = 0;
        }
    }

    function containerFocusOutCallback({ relatedTarget }) {
        if (!registeredEls.includes(relatedTarget) &&
            prioritySelectionIndexRef?.value != null) {
            registeredEls[currentFocusableIndex].tabIndex = -1;
            currentFocusableIndex = prioritySelectionIndexRef?.value ?? 0;
            registeredEls[currentFocusableIndex].tabIndex = 0;
        }
    }

    function elKeyDownCallback({ target, key }) {
        const index = registeredEls.indexOf(target);

        if (index === -1)
            throw new Error(
                '[useRovingFocus]: Callback triggered on unregistered ' +
                'element.');

        if (!containerElRef.value)
            throw new Error(
                '[useRovingFocus]: Nominal descendant triggered callback ' +
                'but container was falsy.')

        switch (key) {
        case 'ArrowUp':
        case 'ArrowLeft':
            if (index === 0)
                break;
            registeredEls[index    ].tabIndex = -1;
            registeredEls[index - 1].tabIndex = 0;
            currentFocusableIndex = index - 1;

            registeredEls[index - 1].focus();
            break;

        case 'ArrowDown':
        case 'ArrowRight':
            if (index === registeredEls.length - 1)
                break;
            registeredEls[index    ].tabIndex = -1;
            registeredEls[index + 1].tabIndex = 0;
            currentFocusableIndex = index + 1;
            
            registeredEls[index + 1].focus();
            break;
        }
    }

    function registerContainer(containerEl) {
        containerEl.addEventListener('focusin',  containerFocusInCallback);
        containerEl.addEventListener('focusout', containerFocusOutCallback);
    }

    function unregisterContainer(containerEl) {
        containerEl?.removeEventListener?.('focusin',  containerFocusInCallback);
        containerEl?.removeEventListener?.('focusout', containerFocusOutCallback);
    }

    function registerEl(el) {
        el.addEventListener('keydown', elKeyDownCallback);
    }

    function unregisterEl(el) {
        el?.removeEventListener('keydown', elKeyDownCallback);
    }

    onUnmounted(() => {
        registeredEls.forEach(unregisterEl);
        unregisterContainer(registeredContainer);
    });
}