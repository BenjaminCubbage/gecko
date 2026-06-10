export function createFocusableChildrenTreeWalker(root) {
    return document.createTreeWalker(
        root,
        NodeFilter.SHOW_ELEMENT,
        el => {
            return el.tabIndex >= 0 && (el.checkVisibility?.() ?? true)
                ? NodeFilter.FILTER_ACCEPT
                : NodeFilter.FILTER_SKIP;
        });
}

export function getNextFocusableElement(el) {
    const tw = createFocusableChildrenTreeWalker(document);
    tw.currentNode = el;
    return tw.nextNode();
}

export function getPrevFocusableElement(el) {
    const tw = createFocusableChildrenTreeWalker(document);
    tw.currentNode = el;
    return tw.previousNode();
}