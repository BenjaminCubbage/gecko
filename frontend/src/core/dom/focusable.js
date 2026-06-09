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

export function getNextFocusableElement(el) {
    let passedEl = false;

    const tw = document.createTreeWalker(
        document.documentElement,
        NodeFilter.SHOW_ELEMENT,
        candidateEl => {
            if (candidateEl === el) {
                passedEl = true;
                return NodeFilter.FILTER_SKIP;
            }
            
            if (passedEl && 
                candidateEl.tabIndex >= 0 && 
                candidateEl.checkVisibility({ visibilityProperty: true }))
                return NodeFilter.FILTER_ACCEPT;

            return NodeFilter.FILTER_SKIP;
        });

    return tw.nextNode();
}

export function getPrevFocusableElement(el) {
    let firstFocusable = null;
    let passedEl       = false;

    const tw = document.createTreeWalker(
        document.documentElement,
        NodeFilter.SHOW_ELEMENT,
        candidateEl => {
            if (candidateEl === el || passedEl) {
                passedEl = true;
                return NodeFilter.FILTER_REJECT;
            }

            if (candidateEl.tabIndex >= 0 && 
                candidateEl.checkVisibility({ visibilityProperty: true })) {
                return NodeFilter.FILTER_ACCEPT;
            }

            return NodeFilter.FILTER_SKIP;
        });

    const result = null;
    while (tw.nextNode())
        result = currentNode;

    return result;
}