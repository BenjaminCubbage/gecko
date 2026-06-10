/*
    Roving tab index implementation. Usage:

    <ul v-roving-container v-roving-home="0">
        <li><button v-roving-item></button></li>
        <li><button v-roving-item></button></li>
    </li>

    Once the container loses focus, only the item at the home
    index is visible in tab order, defaulting to the first item.

    Due to the way this is coded, all focusable elements within
    a given container should be specified as roving items. But
    that's a plus for accessibility anyways!

    Subcontainers are also supported, up to one subcontainer
    per item.

    In this example, a user browse the top-level container as
    normal, and then enter and exit the subcontainer at any
    time by pressing Space/Enter or Esc/Tab respectively:

    <ul v-roving-container>
        <li v-roving-item>
            <div v-roving-container>
                <button v-roving-item>1</button>
                <button v-roving-item>2</button>
                <button v-roving-item>3</button>
            </div>
        </li>

        <li v-roving-item>
            <div v-roving-container>
                <button v-roving-item>4</button>
                <button v-roving-item>5</button>
                <button v-roving-item>6</button>
            </div>
        </li>

        <li v-roving-item>
            <div v-roving-container>
                <button v-roving-item>7</button>
                <button v-roving-item>8</button>
                <button v-roving-item>9</button>
            </div>
        </li>
    </ul>
*/

/*
    Utilities
*/

function scrollElementIntoView(el) {
    el.scrollIntoView({
        block:  'nearest',
        inline: 'nearest'
    });
}

function findAncestorElement(el, predicate) {
    while (el = el.parentElement)
        if (predicate(el))
            return el;
    return null;
}

function isItem(el) {
    return el?.dataset?.rovingTabIndexItem ?? false;
}

function isContainer(el) {
    return el?.dataset?.rovingTabIndexContainer ?? false;
}

function getItemContainer(el) {
    return findAncestorElement(el, isContainer);
}

function getSubcontainerItem(el) {
    if (isItem(el))
        return el;

    return findAncestorElement(el, isItem);
}

function isContainerSubcontainer(container) {
    return findAncestorElement(container, isContainer) != null;
}

function getContainerItemsWalker(container) {
    return document.createTreeWalker(
        container,
        NodeFilter.SHOW_ELEMENT,
        el => {
            /*
                Don't traverse subcontainers.
            */
            if (isContainer(el.parentNode) && el.parentNode !== container)
                return NodeFilter.FILTER_REJECT;

            return isItem(el)
                ? NodeFilter.FILTER_ACCEPT
                : NodeFilter.FILTER_SKIP;
        });
}

function getContainerItems(container) {
    const walker = getContainerItemsWalker(container);
    const items  = [];

    for (let node = walker.nextNode(); node; node = walker.nextNode())
        items.push(node);

    let homeIndex = 0|container?.dataset?.rovingTabIndexHomeIndex;;
    homeIndex = Math.max(homeIndex, 0);
    homeIndex = Math.min(homeIndex, items.length - 1);

    return {
        items,
        homeIndex
    };
}

function getFirstContainerItem(container) {
    return getContainerItemsWalker(container).nextNode();
}

function getLastContainerItem(container) {
    const walker = getContainerItemsWalker(container);

    let last = null;
    while (walker.nextNode())
        last = walker.currentNode;

    return last;
}

function getNextItem(item, forward = true) {
    const container = getItemContainer(item);
    const walker    = getContainerItemsWalker(container);

    walker.currentNode = item;

    const result = forward
        ? walker.nextNode()
        : walker.previousNode();

    return result !== container
        ? result
        : item;
}

function getItemSubcontainer(item) {
    if (isContainer(item))
        return item;

    const walker = document.createTreeWalker(
        item,
        NodeFilter.SHOW_ELEMENT,
        el => {
            if (isContainer(el))
                return getSubcontainerItem(el) === item
                    ? NodeFilter.FILTER_ACCEPT
                    : NodeFilter.FILTER_REJECT;

            return NodeFilter.FILTER_SKIP;
        }
    );

    return walker.nextNode();
}

function resetContainerIndices(container, checkFocus = true) {
    const {
        items,
        homeIndex
    } = getContainerItems(container);

    if (!checkFocus || !container.dataset.rovingTabIndexContainerHasFocus) {
        items.forEach(s => s.tabIndex = -1);

        if (homeIndex !== -1 && !isContainerSubcontainer(container))
            items[homeIndex].tabIndex = 0;
    }
}

/*
    Event handlers
*/

function containerFocusIn({ currentTarget }) {
    currentTarget.dataset.rovingTabIndexContainerHasFocus = true;
}

function containerFocusOut({ currentTarget, relatedTarget }) {
    const {
        items,
    } = getContainerItems(currentTarget);

    if (!items.includes(relatedTarget)) {
        delete currentTarget.dataset.rovingTabIndexContainerHasFocus;
        resetContainerIndices(currentTarget, false);
    }
}

function itemFocusIn({ currentTarget }) {
    currentTarget.tabIndex = 0;
}

function itemFocusOut({ currentTarget, relatedTarget }) {
    if (!currentTarget.contains(relatedTarget))
        currentTarget.tabIndex = -1;
}

function itemKeyDown(e) {
    if (e.currentTarget !== e.target)
        return;

    switch (e.key) {
    case 'ArrowUp':
    case 'ArrowLeft':
    case 'ArrowDown':
    case 'ArrowRight':
    {
        const item =
            getNextItem(e.currentTarget,
                e.key === 'ArrowDown' ||
                e.key === 'ArrowRight');

        if (item) {
            item.focus();
            scrollElementIntoView(item);
            e.preventDefault();
        }
    }
    break;

    case 'Enter':
    case 'Space':
    {
        const subcontainer = getItemSubcontainer(e.currentTarget);

        if (subcontainer) {
            const {
                items,
                homeIndex
            } = getContainerItems(subcontainer);

            if (homeIndex !== -1) {
                items[homeIndex].focus();
                scrollElementIntoView(items[homeIndex]);
            }

            e.preventDefault();
        }
    }
    break;

    case 'Tab':
    case 'Escape':
    {
        const container  = getItemContainer(e.currentTarget);
        const parentItem = getSubcontainerItem(container);

        if (parentItem) {
            parentItem.focus();
            scrollElementIntoView(parentItem);
            e.preventDefault();
        }
    }
    break;

    case 'Home':
    case 'End':
    {
        const container = getItemContainer(e.currentTarget);

        const newItem = e.key === 'End'
            ? getLastContainerItem(container)
            : getFirstContainerItem(container);

        newItem.focus();
        scrollElementIntoView(newItem);
    }
    break;
    }
}

/*
    Element binding / registration
*/

function bindContainer(container) {
    container.addEventListener('focusin',  containerFocusIn);
    container.addEventListener('focusout', containerFocusOut);
    container.dataset.rovingTabIndexContainer = true;
}

function unbindContainer(container) {
    container.removeEventListener('focusin',  containerFocusIn);
    container.removeEventListener('focusout', containerFocusOut);
    delete container.dataset.rovingTabIndexContainer;
}

function bindItem(el) {
    el.tabIndex = -1;
    el.addEventListener('keydown',  itemKeyDown);
    el.addEventListener('focusin',  itemFocusIn);
    el.addEventListener('focusout', itemFocusOut);
    el.dataset.rovingTabIndexItem = true;
    resetContainerIndices(getItemContainer(el));
}

function unbindItem(el) {
    const wasFocused = document.activeElement === el;
    const newFocusEl = wasFocused
        ? getNextItem(el, true) ?? getNextItem(el, false)
        : null;

    el.removeEventListener('keydown',  itemKeyDown);
    el.removeEventListener('focusin',  itemFocusIn);
    el.removeEventListener('focusout', itemFocusOut);
    delete el.dataset.rovingTabIndexItem;

    resetContainerIndices(getItemContainer(el), wasFocused);
    newFocusEl?.focus();
}

/*
    Exports
*/

export const rovingTabIndexContainer = {
    beforeMount(el, { value = true }) {
        if (value)
            bindContainer(el);
    },

    updated(el, { value = true, oldValue = true }) {
        if (value !== oldValue)
            (value ? bindContainer : unbindContainer)(el);
    },

    unmounted(el) {
        unbindContainer(el);
    }
};

export const rovingTabIndexItem = {
    mounted(el, { value = true }) {
        if (value)
            bindItem(el);
    },

    updated(el, { value = true, oldValue = true }) {
        if (value !== oldValue)
            (value ? bindItem : unbindItem)(el);
    },

    beforeUnmount(el) {
        unbindItem(el);
    }
};

export const rovingTabIndexHomeIndex = (el, { value = null }) => {
    if (value != null)
        el.dataset.rovingTabIndexHomeIndex = value;
    else
        delete el.dataset.rovingTabIndexHomeIndex;

    resetContainerIndices(el);
};