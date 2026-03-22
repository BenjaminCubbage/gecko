export function focusFirstFocusableChild(root) {
    const child = document.createTreeWalker(
        root,
        NodeFilter.SHOW_ELEMENT,
        el => {
            return el.tabIndex >= 0
                ? NodeFilter.FILTER_ACCEPT
                : NodeFilter.FILTER_SKIP;
        }).nextNode();

    child?.focus();
    return child;
}