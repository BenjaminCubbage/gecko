<template>
    <ToolBarChip
        class="tool-bar-chip-clear"
        color="dk-red"
        :disable-style="isPressed ? 'none' : 'grayed'"
        :class="{ 'tool-bar-chip-clear--pressed': isPressed }"
        :disabled="isPressed"
        :aria-pressed="isPressed"
        @click="click">
        <template #label>
            <div
                class="icon-text"
                :style="`--animation-duration: ${animationDuration}ms`">
                <span class="letter letter--1">C</span>
                <span class="letter letter--2">L</span>
                <span class="letter letter--3">E</span>
                <span class="letter letter--4">A</span>
                <span class="letter letter--5">R</span>
            </div>
        </template>
    </ToolBarChip>
</template>

<script setup>
import { 
    onUnmounted, 
    ref 
} from 'vue';
import ToolBarChip from './ToolBarChip.vue';

const emit = defineEmits([
    'click'
]);

const isPressed    = ref(false);
let unpressTimeout = null;

function click() {
    emit('click');
    isPressed.value = true;

    unpressTimeout = setTimeout(() => {
        isPressed.value = false;
    }, 1000);
}

onUnmounted(() => {
    clearTimeout(unpressTimeout);
});
</script>

<style scoped>
.tool-bar-chip-clear {
    transition: z-index 0ms 1000ms allow-discrete;

    &.tool-bar-chip-clear--pressed {
        pointer-events: none;
        cursor:         default;

        .letter {
            animation-name:     letter-jump;
            animation-duration: 400ms;
        }
    }

    &:hover,
    &.tool-bar-chip-clear--pressed {
        z-index: 10;

        .letter,
        .icon-text {
            will-change: transform;
            transform:   translateZ(0);
        }
    }
}

.icon-text {
    display: contents;

    transition: transform 0ms allow-discrete 1000ms;
}

/*
    letters
*/

.letter {
    display:    inline-block;
    transition: transform 0ms allow-discrete 1000ms;

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

@keyframes letter-jump {
    0%   { translate: 0  0px; }
    50%  { translate: 0 -3px; }
    85%  { translate: 0  1px; }
    100% { translate: 0  0px; }
}
</style>