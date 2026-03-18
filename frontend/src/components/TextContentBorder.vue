<template>
    <div class="text-content-border txtr-diag txtr-diag--green">
        <div class="inner-content">
            <slot></slot>
        </div>
    </div>
</template>

<style scoped>
.text-content-border {
    contain: layout;

    padding: 6px;

    border:        var(--border-s);
    border-radius: var(--radius-s);
    corner-shape:  notch;

    box-shadow:
        var(--shadow-l),
        var(--shadow-inst-green);

    max-width: min(600px, calc(100cqw - (2 * var(--vp-margin))));
}

.inner-content {
    -webkit-text-stroke: var(--text-stroke-s);
    paint-order:         stroke;

    font-size:   2.2rem;
    line-height: 1;
    color:       black;

    border:        var(--border-s);
    border-radius: var(--radius-s);
    corner-shape:  notch;

    padding: 6px 22px;

    box-shadow:
        var(--shadow-inst-lt-gray),
         3px  3px var(--col-green-6),
        -3px -3px var(--col-green-1);

    background:
        linear-gradient(
            var(--col-lt-gray-2) 50%,
            var(--col-lt-gray-4) 50%);
}
</style>