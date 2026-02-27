<template>
    <ToolBarChip
        class="tool-bar-chip-send"
        color="dk-green"
        icon-placement="right"
        aria-label="Send"
        :disable-style="isLaunching ? 'none' : 'grayed'"
        :disabled="disabled"
        :sr-status="srStatus"
        :temporarily-disabled="isLaunching"
        :temporarily-pressed="isLaunching"
        @click="click">
        <template #label>
            <span class="letter letter--1">S</span>
            <span class="letter letter--2">E</span>
            <span class="letter letter--3">N</span>
            <span class="letter letter--4">D</span>
        </template>

        <template #icon>
            <div
                class="rocket-svgs"
                @animationcancel="animationCancelled">
                <svg
                    class="rocket-svg-rocket"
                    version="1.1"
                    viewBox="-20 -25 498 505">
                    <polygon class="st0 stroke" points="351.7,318.9 351.7,302.5 351.7,286.1 335.3,286.1 335.3,269.7 318.9,269.7 318.9,286.1 302.5,286.1
                        302.5,302.5 286.1,302.5 269.7,302.5 253.4,302.5 237,302.5 220.6,302.5 204.2,302.5 187.8,302.5 171.4,302.5 155.1,302.5
                        155.1,286.1 138.7,286.1 138.7,269.7 122.3,269.7 122.3,286.1 105.9,286.1 105.9,302.5 105.9,318.9 89.5,318.9 89.5,335.3
                        89.5,351.7 89.5,368 89.5,384.4 105.9,384.4 105.9,400.8 122.3,400.8 122.3,384.4 138.7,384.4 138.7,368 138.7,351.7 155.1,351.7
                        155.1,335.3 171.4,335.3 187.8,335.3 187.8,351.7 204.2,351.7 220.6,351.7 237,351.7 253.4,351.7 269.7,351.7 269.7,335.3
                        286.1,335.3 302.5,335.3 302.5,351.7 318.9,351.7 318.9,368 318.9,384.4 335.3,384.4 335.3,400.8 351.7,400.8 351.7,384.4
                        368,384.4 368,368 368,351.7 368,335.3 368,318.9 	"/>
                    <polygon class="st1 stroke" points="155.1,269.7 171.4,269.7 171.4,286.1 187.8,286.1 204.2,286.1 220.6,286.1 237,286.1 253.4,286.1
                        269.7,286.1 286.1,286.1 286.1,269.7 302.5,269.7 302.5,253.4 318.9,253.4 318.9,237 318.9,220.6 318.9,204.2 318.9,187.8
                        318.9,187.8 318.9,171.4 318.9,155.1 302.5,155.1 302.5,138.7 302.5,122.3 286.1,122.3 286.1,105.9 286.1,89.5 269.7,89.5
                        269.7,73.1 253.4,73.1 253.4,56.8 237,56.8 220.6,56.8 204.2,56.8 204.2,73.1 187.8,73.1 187.8,89.5 171.4,89.5 171.4,105.9
                        171.4,122.3 155.1,122.3 155.1,138.7 155.1,155.1 138.7,155.1 138.7,171.4 138.7,187.8 138.7,187.8 138.7,204.2 138.7,220.6
                        138.7,237 138.7,253.4 155.1,253.4 	"/>

                    <polygon class="st0" points="269.7,89.5 269.7,73.1 253.4,73.1 253.4,56.8 204.2,56.8 204.2,73.1 187.8,73.1 187.8,89.5
                        171.4,89.5 171.4,122.3 286.1,122.3 286.1,89.5 	"/>
                </svg>

                <svg class="rocket-svg-flame" version="1.1" x="0px" y="0px" viewBox="0 0 457.6 457.6">
                    <polygon class="stroke st2" points="278.5,99.5 278.5,79.6 258.6,79.6 258.6,59.7 238.7,59.7 238.7,39.8 218.8,39.8 218.8,59.7
                        198.9,59.7 198.9,79.6 179.1,79.6 179.1,99.5 159.2,99.5 159.2,119.4 159.2,139.3 159.2,159.2 179.1,159.2 179.1,179 198.9,179
                        198.9,198.9 198.9,218.8 218.8,218.8 218.8,238.7 218.8,258.6 218.8,278.5 238.7,278.5 238.7,258.6 238.7,238.7 238.7,218.8
                        258.6,218.8 258.6,198.9 258.6,179 278.5,179 278.5,159.2 298.4,159.2 298.4,139.3 298.4,119.4 298.4,99.5 	"/>
                </svg>
            </div>
        </template>
    </ToolBarChip>
</template>

<script setup>
import {
    computed,
    onUnmounted,
    ref
} from 'vue';

import ToolBarChip from './ToolBarChip.vue';

defineProps({
    disabled: { type: Boolean, default: false }
});

const emit = defineEmits([
    'click'
]);

const isLaunching = ref(false);
let launchTimeout = null;

const srStatus = computed(() => {
    return isLaunching.value
        ? 'Sending image'
        : null;
});

function click() {
    isLaunching.value = true;
    launchTimeout = setTimeout(() => {
        isLaunching.value = false;
    }, 5700);
    emit('click');
}

function animationCancelled() {
    isLaunching.value = false;
    clearTimeout(launchTimeout);
}

onUnmounted(() => {
    clearTimeout(launchTimeout);
})
</script>

<style scoped>
.tool-bar-chip-send {
    contain: layout;
    gap:     6px;

    /*
        Animations
    */

    &[data-pressed=true] .letter {
        animation-name:     letter-jump;
        animation-duration: 400ms;
    }

    &[data-pressed=true] {
        .rocket-svgs {
            animation:
                rocket-bob    200ms  steps(2)                    infinite 0ms,
                fly-away      5000ms cubic-bezier(.68,.01,.62,1) 1        500ms,
                rocket-return 200ms  ease                        1        5500ms;

            pointer-events: none;
        }

        .rocket-svg-flame {
            animation: flame-bob 200ms steps(5) 24;
        }
    }

    /*
        Texture promotion
    */

    &:hover,
    &[data-pressed=true] {
        .rocket-svgs,
        .rocket-svg-rocket,
        .rocket-svg-flame,
        .pad,
        .letter {
            will-change: transform;
        }

        .rocket-svg-flame {
            /* Disallow culling */
            opacity: 0.001;
        }
    }
}

.rocket-svgs {
    display:        grid;
    isolation:      isolate;
    opacity:        var(--rocket-opacity);
    pointer-events: none;

    width: 30px;
    translate: 0 0.2px;

    & > * {
        grid-area: 1 / 1;
    }

    & > .rocket-svg-rocket {
        z-index: 1;
        scale:   1.45;
    }

    & > .rocket-svg-flame {
        opacity:   0;
        z-index:   0;
        translate: 0 24px;
    }

    .stroke {
        stroke-width: 62px;
        stroke:       black;
        paint-order:  stroke fill;

        @supports (-moz-appearance: none) {
            stroke-linecap:  round;
            stroke-linejoin: round;
        }
    }

    & > svg > .st0{ fill:#FF0600; }
    & > svg > .st1{ fill:#FFFFFF; }
    & > svg > .st2{ fill:#ff7b00; }
}

.letter {
    display:   inline-block;
    transform: translateY(calc(var(--letter-jump-arc) * -1));
    color:     var(--col-green-6);

    @supports (order: sibling-index()) {
        animation-delay: calc(sibling-index() * 25ms);
    }

    @supports not (order: sibling-index()) {
        &.letter--1 { animation-delay: calc(1 * 25ms); }
        &.letter--2 { animation-delay: calc(2 * 25ms); }
        &.letter--3 { animation-delay: calc(3 * 25ms); }
        &.letter--4 { animation-delay: calc(4 * 25ms); }
    }
}
</style>

<style>
@property --letter-jump-arc {
    inherits:      false;
    initial-value: 0;
    syntax:        "<length>";
}

@keyframes letter-jump {
    0%   { --letter-jump-arc:  0px; }
    50%  { --letter-jump-arc:  3px; }
    85%  { --letter-jump-arc: -1px; }
    100% { --letter-jump-arc:  0px; }
}

@keyframes flame-bob {
    0%   { rotate:  5deg; opacity: 100%; }
    50%  { rotate: -5deg; }
    100% { rotate:  5deg; opacity: 100%; }
}

@keyframes rocket-bob {
    0%   { rotate:  1.5deg; }
    50%  { rotate: -1.5deg; }
    100% { rotate:  1.5deg; }
}

@keyframes fly-away {
    0% {
        opacity:   100%;
        translate: 0 0;
    }

    94% {
        opacity:   100%;
        translate: 0 -800px;
    }

    95%  { opacity: 0%; }
    100% { opacity: 0%; }
}

@keyframes rocket-return {
    0% {
        opacity: 0%;
        translate: 0 2px;
    }

    100% {
        opacity:   100%;
        translate: 0 0;
    }
}
</style>