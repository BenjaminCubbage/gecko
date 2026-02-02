<template>
    <ToolbarChipLayout
        class="toolbar-chip-send"
        :class="{
            'toolbar-chip-send--pressed':  isPressed,
            'toolbar-chip-send--disabled': disabled
        }">
        <template #pad>
            <div class="pad txtr-diag txtr-diag--dk-green"></div>
        </template>

        <template #icons>
            <button class="icon" @click="click" :disabled="disabled">
                <div class="icon-text">
                    <span class="letter letter--1">S</span>
                    <span class="letter letter--2">E</span>
                    <span class="letter letter--3">N</span>
                    <span class="letter letter--4">D</span>
                </div>

                <div
                    class="rocket-svgs"
                    @animationcancel="animationCancelled">
                    <svg
                        class="rocket-svg-rocket"
                        version="1.1"
                        viewBox="-20 -25 498 505">
                        <g class="stroke">
                            <polygon class="st0" points="351.7,318.9 351.7,302.5 351.7,286.1 335.3,286.1 335.3,269.7 318.9,269.7 318.9,286.1 302.5,286.1
                                302.5,302.5 286.1,302.5 269.7,302.5 253.4,302.5 237,302.5 220.6,302.5 204.2,302.5 187.8,302.5 171.4,302.5 155.1,302.5
                                155.1,286.1 138.7,286.1 138.7,269.7 122.3,269.7 122.3,286.1 105.9,286.1 105.9,302.5 105.9,318.9 89.5,318.9 89.5,335.3
                                89.5,351.7 89.5,368 89.5,384.4 105.9,384.4 105.9,400.8 122.3,400.8 122.3,384.4 138.7,384.4 138.7,368 138.7,351.7 155.1,351.7
                                155.1,335.3 171.4,335.3 187.8,335.3 187.8,351.7 204.2,351.7 220.6,351.7 237,351.7 253.4,351.7 269.7,351.7 269.7,335.3
                                286.1,335.3 302.5,335.3 302.5,351.7 318.9,351.7 318.9,368 318.9,384.4 335.3,384.4 335.3,400.8 351.7,400.8 351.7,384.4
                                368,384.4 368,368 368,351.7 368,335.3 368,318.9 	"/>
                            <polygon class="st1" points="155.1,269.7 171.4,269.7 171.4,286.1 187.8,286.1 204.2,286.1 220.6,286.1 237,286.1 253.4,286.1
                                269.7,286.1 286.1,286.1 286.1,269.7 302.5,269.7 302.5,253.4 318.9,253.4 318.9,237 318.9,220.6 318.9,204.2 318.9,187.8
                                318.9,187.8 318.9,171.4 318.9,155.1 302.5,155.1 302.5,138.7 302.5,122.3 286.1,122.3 286.1,105.9 286.1,89.5 269.7,89.5
                                269.7,73.1 253.4,73.1 253.4,56.8 237,56.8 220.6,56.8 204.2,56.8 204.2,73.1 187.8,73.1 187.8,89.5 171.4,89.5 171.4,105.9
                                171.4,122.3 155.1,122.3 155.1,138.7 155.1,155.1 138.7,155.1 138.7,171.4 138.7,187.8 138.7,187.8 138.7,204.2 138.7,220.6
                                138.7,237 138.7,253.4 155.1,253.4 	"/>
                        </g>

                        <polygon class="st0" points="269.7,89.5 269.7,73.1 253.4,73.1 253.4,56.8 204.2,56.8 204.2,73.1 187.8,73.1 187.8,89.5
                            171.4,89.5 171.4,122.3 286.1,122.3 286.1,89.5 	"/>
                    </svg>

                    <svg class="rocket-svg-flame" version="1.1" x="0px" y="0px" viewBox="0 0 457.6 457.6">
                        <polygon class="stroke st3" points="278.5,99.5 278.5,79.6 258.6,79.6 258.6,59.7 238.7,59.7 238.7,39.8 218.8,39.8 218.8,59.7
                            198.9,59.7 198.9,79.6 179.1,79.6 179.1,99.5 159.2,99.5 159.2,119.4 159.2,139.3 159.2,159.2 179.1,159.2 179.1,179 198.9,179
                            198.9,198.9 198.9,218.8 218.8,218.8 218.8,238.7 218.8,258.6 218.8,278.5 238.7,278.5 238.7,258.6 238.7,238.7 238.7,218.8
                            258.6,218.8 258.6,198.9 258.6,179 278.5,179 278.5,159.2 298.4,159.2 298.4,139.3 298.4,119.4 298.4,99.5 	"/>
                    </svg>
                </div>
            </button>
        </template>
    </ToolbarChipLayout>
</template>

<script setup>
import { ref } from 'vue';
import ToolbarChipLayout from './ToolbarChipLayout.vue';

defineProps({
    disabled: { type: Boolean, default: false }
});

const emit = defineEmits(['click']);

const isPressed   = ref(false);
let activeTimeout = null;

function click() {
    if (!isPressed.value) {
        isPressed.value = true;
        activeTimeout = setTimeout(() => {
            isPressed.value = false;
            activeTimeout   = null;
        }, 5700);

        emit('click');
    }
}

function animationCancelled() {
    isPressed.value = false;

    if (activeTimeout != null)
        clearTimeout(activeTimeout);
}
</script>

<style scoped>
.toolbar-chip-send {
    --chip-aura:   0 0 0 black;
    --chip-offset: 0px;
}

.pad {
    padding: 0 10px;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    box-shadow:
        var(--chip-aura),
        calc(var(--shadow-dist-s) - var(--chip-offset))
        calc(var(--shadow-dist-s) - var(--chip-offset)) 0 black;

    transform: translate(
        var(--chip-offset),
        var(--chip-offset));

    transition:
        box-shadow 50ms linear,
        transform  50ms linear;
}

.icon {
    align-items: center;
    display:     flex;
    gap:         5px;
    padding:     0 5px 0 10px;
    line-height: 0;

    transform:
        translateY(-4px)
        scale(1, 1.1)
        translate(
            var(--chip-offset),
            var(--chip-offset));

    transition: transform 50ms linear;
}

.icon-text {
    -webkit-text-stroke: 4px white;
    color:               var(--col-green-5);
    font-family:         var(--font-heading);
    font-size:           3rem;
    letter-spacing:      0.02em;

    paint-order: stroke;
}

/*
    Rocket SVGs
*/

.rocket-svgs {
    transform:
        translateY(var(--rocket-offset-y))
        rotate(var(--rocket-rotation));

    display:     grid;
    z-index:     0;
    opacity:     var(--rocket-opacity);
    will-change: transform;
}

.rocket-svg-rocket {
    transform:
        scale(1.15);

    grid-area: 1 / 1;
    height:    36px;
    z-index:   1;
}

.rocket-svg-flame {
    transform:
        translateY(24px)
        rotate(var(--flame-rotation));

    grid-area:   1 / 1;
    opacity:     var(--flame-opacity);
    will-change: transform;
}

.st0{ fill:#FF0600; }
.st1{ fill:white;   }
.st2{ fill:white;   }
.st3{ fill:#ff7b00; }

.stroke {
    paint-order:  stroke fill;
    stroke-width: 64px;
    stroke:       black;
}

.toolbar-chip-send--disabled {
    pointer-events: none;
    filter:         grayscale();
    opacity:        0.5;
}

.toolbar-chip-send--pressed :where(.rocket-svg-flame) {
    animation: flame-bob 200ms steps(5) 24;
}

.toolbar-chip-send--pressed :where(.rocket-svgs) {
    animation:
        rocket-bob    200ms  steps(2)                    infinite 0ms,
        fly-away      5000ms cubic-bezier(.68,.01,.62,1) 1        500ms,
        rocket-return 200ms  ease                        1        5500ms;

    pointer-events: none;
}

/*
    Letters
*/

.letter {
    color: var(--col-green-6);
}

.toolbar-chip-send:hover {
    --chip-offset: calc(var(--shadow-dist-s) / 2);
}

.toolbar-chip-send:active,
.toolbar-chip-send--pressed,
.toolbar-chip-send--pressed:hover {
    --chip-aura:   var(--shadow-aura);
    --chip-offset: var(--shadow-dist-s);
}

.toolbar-chip-send--pressed :where(.letter) {
    animation: letter-jump 400ms ease;
}

.letter {
    display:   inline-block;
    transform: translateY(calc(var(--letter-jump-arc) * -1));
}

@supports (-moz-appearance: none) {
    .stroke {
        stroke-linecap:  round;
        stroke-linejoin: round;
    }
}

@supports (order: sibling-index()) {
    .letter {
        animation-delay: calc(sibling-index() * 25ms);
    }
}

@supports not (order: sibling-index()) {
    .letter--1 { animation-delay: calc(1 * 25ms); }
    .letter--2 { animation-delay: calc(2 * 25ms); }
    .letter--3 { animation-delay: calc(3 * 25ms); }
    .letter--4 { animation-delay: calc(4 * 25ms); }
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

@property --flame-scale {
    inherits:      false;
    initial-value: 100%;
    syntax:        "<percentage>";
}

@property --flame-rotation {
    inherits:      true;
    initial-value: 0deg;
    syntax:        "<angle>";
}

@property --flame-opacity {
    inherits: true;
    initial-value: 0%;
    syntax: "<percentage>";
}

@keyframes flame-bob {
    0% {
        --flame-opacity:  100%;
        --flame-rotation: 5deg;
    }

    50% {
        --flame-rotation: -5deg;
    }

    100% {
        --flame-opacity:  100%;
        --flame-rotation: 5deg;
    }
}

@property --rocket-rotation {
    inherits:      false;
    initial-value: 0deg;
    syntax:        "<angle>";
}

@property --rocket-offset-y {
    inherits: false;
    initial-value: 0px;
    syntax: "<length>";
}

@property --rocket-opacity {
    inherits: false;
    initial-value: 100%;
    syntax: "<percentage>";
}

@keyframes rocket-bob {
    0% {
        --rocket-rotation: 1.5deg;
    }

    50% {
        --rocket-rotation: -1.5deg;
    }

    100% {
        --rocket-rotation: 1.5deg;
    }
}

@keyframes fly-away {
    0% {
        --rocket-offset-y: 0px;
    }

    94.9999% {
        --rocket-offset-y: -800px;
        --rocket-opacity: 100%;
    }

    95% {
        --rocket-opacity: 0%;
    }

    100% {
        --rocket-opacity: 0%;
    }
}

@keyframes rocket-return {
    0% {
        --rocket-offset-y: 2px;
        --rocket-opacity:  0%;
    }

    100% {
        --rocket-offset-y: 0px;
        --rocket-opacity:  100%;
    }
}
</style>