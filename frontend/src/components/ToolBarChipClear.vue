<template>
    <ToolBarChip
        ref="toolBarChipEl"
        class="tool-bar-chip-clear"
        color="dk-red"
        aria-label="Clear"
        :disable-style="isPressed ? 'none' : 'grayed'"
        :sr-status="srStatus"
        :temporarily-disabled="isPressed"
        :temporarily-pressed="isPressed"
        :highlight-style="size === 'normal' ? 'shadow' : 'filter'"
        @click="click">
        <template v-if="size === 'normal'" #label>
            <div class="icon-text">
                <span class="letter letter--1">C</span>
                <span class="letter letter--2">L</span>
                <span class="letter letter--3">E</span>
                <span class="letter letter--4">A</span>
                <span class="letter letter--5">R</span>
            </div>
        </template>

        <template v-else #icon>
            <svg class="icon-svg" viewBox="0 0 40 40">
                <path class="st0" d="M34.2,3.9c-0.9,0-2.8,0-3.8,0v1.9h-1.9v1.9h-1.9v1.9h-1.9v1.9h-1.9v1.9H21v1.9c0,0-1.9,0-1.9,0v-1.9h-1.9v-1.9h-1.9V9.6
                    h-1.9V7.7h-1.9V5.8H9.6V3.9c-1.7,0-4,0-5.7,0c0,1.7,0,4,0,5.7h1.9v1.9h1.9v1.9h1.9v1.9h1.9v1.9h1.9V19h1.9v1.9h-1.9v1.9h-1.9v1.9
                    H9.6v1.9H7.7v1.9H5.8v1.9H3.9c0,1.7,0,4,0,5.7c1.7,0,4,0,5.7,0v-1.9h1.9v-1.9h1.9v-1.9h1.9v-1.9h1.9v-1.9h1.9v-1.9h1.9v1.9h1.9v1.9
                    h1.9v1.9h1.9v1.9h1.9v1.9h1.9v1.9c1.7,0,4,0,5.7,0c0-1.7,0-4,0-5.7h-1.9v-1.9h-1.9v-1.9h-1.9v-1.9h-1.9v-1.9h-1.9v-1.9h-1.9V19h1.9
                    v-1.9h1.9v-1.9h1.9v-1.9h1.9v-1.9h1.9V9.6h1.9c0-1.7,0-4,0-5.7H34.2z"/>
            </svg>
        </template>
    </ToolBarChip>
</template>

<script setup>
import {
    computed,
    onUnmounted,
    ref,
    useTemplateRef
} from 'vue';

import ToolBarChip from './ToolBarChip.vue';

defineProps({
    size: {
        type:    String,
        required: true,
        validator(value) {
            return [
                'normal',
                'small'
            ].includes(value);
        }
    }
});

const emit = defineEmits([
    'click'
]);

const isPressed    = ref(false);
let unpressTimeout = null;

const toolBarChipEl = useTemplateRef('toolBarChipEl');

const srStatus = computed(() => {
    return isPressed.value
        ? 'Clearing Canvas'
        : null;
});

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

defineExpose({
    innerElement: computed(() => toolBarChipEl.value?.innerElement)
});
</script>

<style scoped>
.tool-bar-chip-clear {
    &.tool-bar-chip-clear[data-pressed=true] {
        pointer-events: none;
        cursor:         default;

        .letter {
            animation-name:     letter-jump;
            animation-duration: 400ms;
        }

        .icon-svg {
            animation-name:     spin;
            animation-duration: 1000ms;
        }
    }

    &:hover,
    &.tool-bar-chip-clear[data-pressed=true] {
        z-index: 10;

        .letter,
        .icon-text {
            will-change: transform;
        }
    }
}

.icon-text {
    display: contents;
}

.icon-svg {
    width: 30px;
    
    stroke-width: 6.9px;
    stroke:       black;
    
    scale:      0.88;
    transform:  scale(1.2);
    translate:  0.2px 1.4px;
    transition: rotate 800ms ease;

    paint-order: stroke;


    & > .st0 { 
        fill: var(--col-red-4); 
    }

    @supports (-moz-appearance: none) {
        stroke-linecap:  round;
        stroke-linejoin: round;
    }
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

@keyframes spin {
    from { rotate: 0deg; }
    to   { rotate: 360deg; }
}
</style>