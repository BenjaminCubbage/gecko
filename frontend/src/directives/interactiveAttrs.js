/*
    interactiveAttrs: Attach data-hovered and data-pressed
    attributes to elements when they are hovered and pressed.

    This prevents the required >100ms delay for :active and :hover
    selectors on mobile.

    data-hovered will be set when the element is hovered or 
    pressed by a mouse pointer.

    data-pressed will be set when the element is pressed by the
    main button (such as the left mouse button).

    We use setPointerCapture to make pressing "sticky", so a mouse
    can leave the element and reenter it while keeping the button
    pressed.
*/

function onPointerEnter({ currentTarget, pointerType}) {
    if (pointerType === 'mouse')
        currentTarget.dataset.hovered = true;
}

function onPointerLeave({ currentTarget }) {
    delete currentTarget.dataset.pressed;
    delete currentTarget.dataset.hovered;
}

function onPointerDown({ currentTarget, pointerId, pointerType, button }) {
    if (button != 0)
        return;

    currentTarget.setPointerCapture(pointerId);
    currentTarget.dataset.pressed = true;
}

function onPointerUp(e) {
    /* Pointer capture is released automatically. */
    delete e.currentTarget.dataset.pressed;
}

function onClick(e) {
    if (e.pointerId !== -1) {
        /* Ignore click event if capturing pointer click outside bounds. */
        const hitTest = e.currentTarget.contains(
            document.elementFromPoint(e.clientX, e.clientY));

        if (!hitTest)
            e.stopPropagation();
    }
}

export const interactiveAttrs = {
    mounted(el, { value = true }) {
        if (value) {
            el.addEventListener('pointerenter', onPointerEnter);
            el.addEventListener('pointerleave', onPointerLeave);
            el.addEventListener('pointerdown',  onPointerDown);
            el.addEventListener('pointerup',    onPointerUp);

            el.addEventListener('click', onClick, { capture: true });
        }
    },

    updated(el, { value = true, oldValue = true }) {
        if (value == oldValue)
            return;

        if (value) {
            el.addEventListener('pointerenter', onPointerEnter);
            el.addEventListener('pointerleave', onPointerLeave);
            el.addEventListener('pointerdown',  onPointerDown);
            el.addEventListener('pointerup',    onPointerUp);

            el.addEventListener('click', onClick, { capture: true });
        } else {
            el.removeEventListener('pointerenter', onPointerEnter);
            el.removeEventListener('pointerleave', onPointerLeave);
            el.removeEventListener('pointerdown',  onPointerDown);
            el.removeEventListener('pointerup',    onPointerUp);

            el.removeEventListener('click', onClick, { capture: true });
        }
    },

    beforeUnmount(el) {
        /* Not going to release pointer capture here. */

        el.removeEventListener('pointerenter', onPointerEnter);
        el.removeEventListener('pointerleave', onPointerLeave);
        el.removeEventListener('pointerdown',  onPointerDown);
        el.removeEventListener('pointerup',    onPointerUp);

        el.removeEventListener('click', onClick, { capture: true });

        delete el.dataset.pressed;
        delete el.dataset.hovered;
    }
};