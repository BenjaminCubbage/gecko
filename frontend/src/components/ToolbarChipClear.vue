<template>
    <ToolbarChipLayout
        class="toolbar-chip-clear"
        :class="{ 'toolbar-chip-clear--pressed': isPressed }">
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
    </ToolbarChipLayout>
</template>

<script setup>
import { ref } from 'vue';
import ToolbarChipLayout from './ToolbarChipLayout.vue';

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
.toolbar-chip-clear {
    --chip-aura:   0 0;
    --chip-offset: 0px;
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
        box-shadow 50ms linear,
        translate  50ms linear;

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

    paint-order: stroke;

    transition: transform 50ms linear;

    transform:
        translateY(-4px)
        scale(1, 1.1)
        translateY(var(--chip-offset));
}

.toolbar-chip-clear:hover {
    --chip-offset: calc(var(--shadow-dist-s) / 2);
}

.toolbar-chip-clear:active,
.toolbar-chip-clear--pressed,
.toolbar-chip-clear--pressed:hover {
    --chip-aura:   var(--shadow-aura);
    --chip-offset: var(--shadow-dist-s);
}

/*
    letters
*/

.letter {
    display:   inline-block;
    transform: translateY(calc(var(--letter-jump-arc) * -1));
}

.toolbar-chip-clear--pressed :where(.letter) {
    animation:
        calc(var(--animation-duration) - 100ms)
        ease
        letter-jump;
}

@supports (order: sibling-index()) {
    .letter {
        animation-delay: calc(sibling-index() * 20ms);
    }
}

@supports not (order: sibling-index()) {
    .letter--1 { animation-delay: calc(1 * 20ms); }
    .letter--2 { animation-delay: calc(2 * 20ms); }
    .letter--3 { animation-delay: calc(3 * 20ms); }
    .letter--4 { animation-delay: calc(4 * 20ms); }
    .letter--5 { animation-delay: calc(5 * 20ms); }
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