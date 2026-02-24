<template>
    <ToolBarChip
        class="tool-bar-chip-clear"
        :class="{ 'tool-bar-chip-clear--pressed': isPressed }">
        <template #pad>
            <div class="pad txtr-diag txtr-diag--dk-red"></div>
        </template>

        <template #icons>
            <button class="icon" @click="click">
                <div
                    class="icon-text"
                    :style="`--animation-duration: ${animationDuration}ms`">
                    <span class="letter letter--1">C</span>
                    <span class="letter letter--2">L</span>
                    <span class="letter letter--3">E</span>
                    <span class="letter letter--4">A</span>
                    <span class="letter letter--5">R</span>
                </div>
            </button>
        </template>
    </ToolBarChip>
</template>

<script setup>
import { ref }     from 'vue';
import ToolBarChip from './ToolBarChip.vue';

const props = defineProps({
    animationDuration: { type: Number, default: 500 }
});

const emit = defineEmits(['click']);

const isPressed    = ref(false);
const unpressDelay = 500;
let timeoutAcc     = 0;

function click() {
    if (!isPressed.value)
        emit('click');

    isPressed.value = true;

    ++timeoutAcc;
    setTimeout(() => {
        if (!--timeoutAcc)
            isPressed.value = false;
    }, props.animationDuration + unpressDelay);
}
</script>

<style scoped>
.tool-bar-chip-clear {
    --chip-aura:   0 0;
    --chip-offset: 0px;

    transition: z-index 0ms 1000ms allow-discrete;

    &:hover:not(.tool-bar-chip-clear--pressed),
    &:active {
        filter: var(--filter-hl-2);
    }

    &:active,
    &.tool-bar-chip-clear--pressed,
    &.tool-bar-chip-clear--pressed:hover {
        --chip-aura:   var(--shadow-aura);
        --chip-offset: var(--shadow-dist-s);
    }

    &.tool-bar-chip-clear--pressed {
        pointer-events: none;
        cursor:         default;

        .letter {
            animation-name:     letter-jump;
            animation-duration: calc(var(--animation-duration) - 100ms);
        }
    }

    &:hover,
    &.tool-bar-chip-clear--pressed {
        z-index: 10;

        .pad,
        .letter,
        .icon-text {
            will-change: transform;
            transform:   translateZ(0);
        }
    }
}

.pad {
    border:        var(--border-s);
    border-radius: var(--radius-s);

    box-shadow:
        var(--chip-aura),
        0 calc(var(--shadow-dist-s) - var(--chip-offset))
        0 black,
        inset  3px  3px var(--col-red-0),
        inset -3px -3px var(--col-red-6);

    transition:
        box-shadow  50ms linear,
        translate   50ms linear,
        transform   0ms  allow-discrete 1000ms;

    transform: none;
    translate: 0 var(--chip-offset);

    corner-shape: notch;
}

.icon {
    padding:     0 10px;
    line-height: 0;
}

.icon-text {
    -webkit-text-stroke: var(--text-stroke-l);
    color:               var(--col-red-6);
    font-family:         var(--font-heading);
    font-size:           3rem;
    letter-spacing:      0.02em;

    transition:
        translate 50ms ease,
        transform 0ms  allow-discrete 1000ms;

    scale:     1 1.1;
    translate: 0 calc(var(--chip-offset) - 4px);

    paint-order: stroke;
}

/*
    letters
*/

.letter {
    display:   inline-block;

    transition: transform 0ms allow-discrete 1000ms;
    transform:  none;
    translate:  0 calc(var(--letter-jump-arc) * -1);

    @supports (order: sibling-index()) {
        animation-delay: calc(sibling-index() * 20ms);
    }

    @supports not (order: sibling-index()) {
        &.letter--1 { animation-delay: calc(1 * 20ms); }
        &.letter--2 { animation-delay: calc(2 * 20ms); }
        &.letter--3 { animation-delay: calc(3 * 20ms); }
        &.letter--4 { animation-delay: calc(4 * 20ms); }
        &.letter--5 { animation-delay: calc(5 * 20ms); }
    }
}

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
</style>