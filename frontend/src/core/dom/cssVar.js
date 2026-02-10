export function getComputedCSSVar(name, target = null) {
    return window
        .getComputedStyle(target ?? document.documentElement)
        .getPropertyValue(name);
}