<template>
    <ToolBarChip
        ref="toolBarChipEl"
        class="tool-bar-chip-clear"
        color="red"
        @click="click">
        <template #icon>
            <IconTrash class="icon" />
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
import IconTrash   from './IconTrash.vue';

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
.icon {
    height: 47px;
}

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

    & > .st0 {
        fill: var(--col-red-4);
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