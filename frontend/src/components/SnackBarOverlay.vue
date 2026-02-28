<template>
    <section
        class="snack-bar-overlay"
        aria-label="Status message">
        <transition name="expand" mode="out-in">
            <div
                v-if="isVisible"
                class="snack-bar-content txtr-diag txtr-diag--green"
                :data-dismissed-manually="dismissedManually"
                :key="latestMessage.id">
                <div
                    class="snack-bar-message"
                    aria-hidden="true">
                    {{ latestMessage.text }}
                </div>

                <button
                    ref="closeButtonEl"
                    class="close-button txtr-diag txtr-diag--dk-red"
                    aria-label="Hide status"
                    @click="handleClick"
                    @focus="handleFocus">
                    x
                </button>
            </div>
        </transition>

        <!-- SR -->
        <div
            role="status"
            :aria-live="isVisible && latestMessage ? 'polite' : 'off'"
            class="util-sr-only">
            {{ srStatus }}
        </div>
    </section>
</template>

<script setup>
import {
    computed,
    inject,
    nextTick,
    onUnmounted,
    ref,
    watch
} from 'vue';

import { Keys } from '@/core/di/keys.js';

const { latestMessage } = inject(Keys.SnackBarStore);

const isVisible      = ref(false);
const dismissedManually = ref(false);

let autoDismissTimeout = null;
let lastFocusedElement = null;

const srStatus = computed(() => {
    return (
        !latestMessage.value
            ? 'Status messages will appear here'
            : isVisible.value
                ? latestMessage.value.text
                : `Last status was: ${latestMessage.value.text}`);
});

watch(latestMessage, async newValue => {
    if (!newValue)
        return;

    clearTimeout(autoDismissTimeout);

    dismissedManually.value = false;
    await nextTick();
    isVisible.value = true;

    autoDismissTimeout = setTimeout(() => {
        isVisible.value = false;
    }, 7000);
}, {
    immediate: true
});

async function handleClick() {
    dismissedManually.value = true;
    await nextTick();
    isVisible.value = false;

    lastFocusedElement?.focus();
}

function handleFocus(e) {
    if (e.relatedTarget)
        lastFocusedElement = e.relatedTarget;
}

onUnmounted(() => {
    clearTimeout(autoDismissTimeout);
});
</script>

<style scoped>
.snack-bar-overlay {
    contain:        layout;
    container-name: snack-bar;
    container-type: inline-size;

    --container-height: 180px;

    position: fixed;
    inset: auto 0 0 0;

    height:         var(--container-height);
    padding-bottom: calc(var(--vp-margin) + var(--shadow-dist-l));

    display:       grid;
    place-content: end center;

    will-change: transform, opacity;

    pointer-events: none;

    & > .snack-bar-content {
        pointer-events: auto;
    }
}

.snack-bar-content {
    position: relative;
    padding:  6px;

    border:        var(--border-s);
    border-radius: var(--radius-s);
    corner-shape:  notch;

    box-shadow:
        var(--shadow-l),
        var(--shadow-inst-green);

    max-width: min(600px, calc(100cqw - (2 * var(--vp-margin))));

    --transition-dur: 600ms;
    --fade-delay:     100ms;
    --easing-pop:     cubic-bezier(.58, -0.22, .69, 1.22);

    &[data-dismissed-manually=true] {
        --transition-dur: 200ms;
    }

    &.expand-enter-active,
    &.expand-leave-active {
        transition:
            translate var(--transition-dur) var(--easing-pop),
            opacity   var(--transition-dur) var(--fade-delay) ease;
    }

    &.expand-enter-from,
    &.expand-leave-to {
        translate: 0 var(--container-height);
        opacity:   0;
    }
}

.snack-bar-message {
    cursor: default;

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

.close-button {
    --transition-dur: 50ms;
    --press-depth:    0px;

    position: absolute;
    right:    -9px;
    top:      -9px;

    width:  1.4em;
    height: 1.4em;

    display:       grid;
    place-content: center;

    padding-bottom: 3px;

    font-size: 2.2rem;
    color:     black;

    border:        var(--border-s);
    border-radius: var(--radius-s);
    corner-shape:  notch;

    box-shadow:
        var(--shadow-inst-red),
        0 calc(var(--shadow-dist-xs) - var(--press-depth)) 0 black;

    text-shadow:
        -1.5px -1.5px var(--col-red-1),
         1.5px  1.5px var(--col-red-5);

    transition:
        box-shadow var(--transition-dur),
        translate  var(--transition-dur),
        filter     var(--transition-dur);

    &:hover {
        filter: var(--filter-hl-1);
    }

    &:active {
        --press-depth: var(--shadow-dist-xs);
        translate: 0 var(--press-depth);
    }

    &:focus-visible {
        filter: var(--filter-hl-1);
    }
}
</style>