export function createFocusableChildrenTreeWalker(root) {
    return document.createTreeWalker(
        root,
        NodeFilter.SHOW_ELEMENT,
        el => {
            return el.tabIndex >= 0
                ? NodeFilter.FILTER_ACCEPT
                : NodeFilter.FILTER_SKIP;
        });
}