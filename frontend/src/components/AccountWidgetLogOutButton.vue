<template>
    <button
        class="logout-button txtr-diag txtr-diag--red"
        @click="emit('click')">
        Log Out
    </button>
</template>

<script setup>
const emit = defineEmits(['click']);
</script>

<style>
.logout-button {
    align-items:     center;
    display:         flex;
    justify-content: center;
    line-height:     1em;
    padding:         4px 18px;
    position:        relative;

    -webkit-text-stroke: var(--text-stroke-s);
    color:               black;
    font-family:         var(--font-heading);
    font-size:           2.31rem;
    letter-spacing:      0.04em;

    --logout-offset: 0px;

    box-shadow:
        calc(var(--shadow-dist-s) - var(--logout-offset))
        calc(var(--shadow-dist-s) - var(--logout-offset))
        0 black,
        inset 0  3px 0 var(--col-red-1),
        inset 0 -3px 0 var(--col-red-4);

    border: var(--border-s);
    border-radius: var(--radius-s);

    paint-order: stroke;

    transform:
        translate(
            var(--logout-offset),
            var(--logout-offset));

    transition:
        box-shadow 50ms ease,
        transform  50ms ease;
}

.logout-button:hover {
    --logout-offset: calc(var(--shadow-dist-s) / 2);
}

.logout-button:active {
    --logout-offset: var(--shadow-dist-s);
}
</style>