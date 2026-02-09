<template>
    <div class="recipient-select-callout">
        <slot></slot>
    </div>
</template>

<style scoped>
.recipient-select-callout {
    padding:     0 36px;
    user-select: none;

    -webkit-text-stroke: var(--text-stroke-s);
    color:               black;
    font-family:         var(--font-heading);
    font-size:           2.4rem;
    letter-spacing:      0.02em;

    background:
        linear-gradient(
            var(--col-green-2) 50%,
            var(--col-green-4) 50%);

    box-shadow:
        var(--shadow-s),
        inset 0 3px 0 var(--col-green-0);

    border-radius: var(--radius-s);
    border:        var(--border-s);
    corner-shape:  notch;

    paint-order: stroke;
}
</style>