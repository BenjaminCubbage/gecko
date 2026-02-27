<template>
    <button
        class="logout-button"
        @click="logOut">
        LOG OUT
    </button>
</template>

<script setup>
import { inject } from 'vue';
import { Keys }   from '@/core/di/keys.js';

const session = inject(Keys.SessionStore);

function logOut() {
    session.requestLogOut();
}
</script>

<style>
.logout-button {
    height:          35px;
    justify-content: center;
    line-height:     1;
    padding:         0 18px;

    -webkit-text-stroke: var(--text-stroke-s);
    color:               black;
    font-size:           2.2rem;
    letter-spacing:      0.04em;

    --logout-offset: 0px;

    background:
        linear-gradient(
            var(--col-red-2) 50%,
            var(--col-red-3) 50%);

    box-shadow:
        calc(var(--shadow-dist-s) - var(--logout-offset))
        calc(var(--shadow-dist-s) - var(--logout-offset))
        0 black,
        var(--shadow-inst-red);

    border: var(--border-s);
    border-radius: var(--radius-s);

    transform:
        translate(
            var(--logout-offset),
            var(--logout-offset));

    transition:
        box-shadow 50ms ease,
        transform  50ms ease;

    corner-shape: notch;
    paint-order:  stroke;
}

.logout-button:hover {
    --logout-offset: calc(var(--shadow-dist-s) / 2);
}

.logout-button:active {
    --logout-offset: var(--shadow-dist-s);
}
</style>