export function useScrollbarWidthProperty() {
    function updateScrollbarWidthProperty() {
        const el = document.createElement('div');
        el.style.cssText = `
            position:   absolute;
            overflow:   scroll;
            width:      20px;
            height:     1px;
        `;

        document.body.append(el);
        const scrollbarWidth = el.offsetWidth - el.clientWidth;
        el.remove();
        
        document.documentElement.style.setProperty(
            '--scrollbar-width', `${scrollbarWidth}px`);
    }

    return {
        updateScrollbarWidthProperty
    };
}