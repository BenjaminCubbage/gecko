<template>
    <div class="recipient-select-callout">
        <slot></slot>
    </div>
</template>

<style scoped>
.recipient-select-callout {
    padding:     0 36px;
    user-select: none;

    -webkit-text-stroke: 4px white;
    color:               black;
    font-family:         var(--font-heading);
    font-size:           2.4rem;
    letter-spacing:      0.02em;

    background:
        linear-gradient(
            var(--col-gray-2) 50%,
            var(--col-gray-3) 50%);

    box-shadow:
        var(--shadow-s),
        inset 0 3px 0 var(--col-gray-1);

    border-radius: var(--radius-s);
    border:        var(--border-s);
    corner-shape:  notch;

    paint-order: stroke;
}
</style>