<template>
    <ToolBarChip
        ref="toolBarChipEl"
        class="tool-bar-chip-pen-size"
        color="orange"
        :aria-label="`${penSize} brush`"
        @click="cycle"
        v-bind="attrs">
        <template #icon>
            <svg class="brushes-svg" viewBox="0 0 40 40">
                <polygon class="st1 stroke" points="34.5,4.2 32.9,4.2 31.4,4.2 31.4,5.8 29.8,5.8 29.8,7.3 28.3,7.3 28.3,8.8 26.8,8.8 26.8,10.4
                    25.2,10.4 25.2,11.9 23.7,11.9 23.7,13.4 22.1,13.4 22.1,14.9 20.6,14.9 20.6,16.5 19.1,16.5 19.1,18 17.5,18 17.5,19.5 16,19.5
                    16,21.1 16,22.6 17.5,22.6 17.5,24.1 19.1,24.1 20.6,24.1 20.6,22.6 22.1,22.6 22.1,21.1 23.7,21.1 23.7,19.5 25.2,19.5 25.2,18
                    26.8,18 26.8,16.5 28.3,16.5 28.3,14.9 29.8,14.9 29.8,13.4 31.4,13.4 31.4,11.9 32.9,11.9 32.9,10.4 34.5,10.4 34.5,8.8 36,8.8
                    36,7.3 36,5.8 36,4.2 "/>
                <polygon class="st2" points="20.6,21.1 20.6,19.5 19.1,19.5 19.1,18 17.5,18 17.5,19.5 16,19.5 16,21.1 16,22.6 17.5,22.6
                    17.5,24.1 19.1,24.1 20.6,24.1 20.6,22.6 22.1,22.6 22.1,21.1 "/>
                <path v-show="penSize === 'small'" style="translate: 2px 0" class="st0 stroke" d="M14.6,34.5V33H13c0-0.9,0-2.3,0-3.1h1.5c0-1.4,0-3.2,0-4.6H13c0-0.4,0-1.2,0-1.6h-1.5c0,0.4,0,1.2,0,1.6
                    c-0.4,0-1.2,0-1.7,0v1.5H8.3v1.5H6.7c0,2,0,4.2,0,6.2h1.5V36c2.6,0,5.4,0,8,0v-1.5H14.6z"/>
                <path v-show="penSize === 'medium'" class="st0 stroke" d="M19.2,34.5V33c-1.4,0-3.2,0-4.6,0c0-0.9,0-2.2,0-3.1H16c0-1.3,0-3.2,0-4.5h-1.5v-1.6h-3.1v1.6
                    c-0.8,0-2.3,0-3,0v1.5H6.7v1.5H5.2c0,2,0,4.2,0,6.2h1.5V36c4.6,0,9.4,0,14,0v-1.5H19.2z"/>
                <polygon v-show="penSize === 'large'" class="st0 stroke" points="20.6,34.5 20.6,33 19.1,33 19.1,31.4 17.6,31.4 17.6,29.9 16,29.9 16,28.4 17.6,28.4 17.6,25.3
                    16,25.3 16,23.8 16,23.7 11.4,23.7 11.4,23.8 11.4,25.3 8.3,25.3 8.3,26.8 6.8,26.8 6.8,28.4 5.2,28.4 5.2,34.5 6.8,34.5 6.8,36
                    23.7,36 23.7,34.5 "/>
            </svg>
        </template>

        <template #label>
            SIZE:

            <transition name="size-number" mode="out-in">
                <span class="size-number" :key="penSize">
                    {{ sizeNumber }}
                </span>
            </transition>
        </template>
    </ToolBarChip>
</template>

<script setup>
import {
    computed,
    useAttrs,
    useTemplateRef
} from 'vue';

import ToolBarChip from './ToolBarChip.vue';

const attrs = useAttrs();

const penSize = defineModel({
    type: String,
    required: true,
    validator(value) {
        return [
            'small',
            'medium',
            'large'
        ].includes(value);
    }
});

const toolBarChipEl = useTemplateRef('toolBarChipEl');

const sizeNumber = computed(() => {
    return penSize.value === 'small'
        ? '1'
        : penSize.value === 'medium'
            ? '2'
            : '3';
});

function cycle() {
    penSize.value =
        penSize.value === 'small'
            ? 'medium'
            : penSize.value === 'medium'
                ? 'large'
                : 'small';
}

defineExpose({
    innerElement: computed(() => toolBarChipEl.value?.innerElement)
});
</script>

<style scoped>
.tool-bar-chip-pen-size {
    gap: 7px;
}

.icon {
    align-items: center;
    display:     flex;
    transition:  translate 50ms ease;
}

.brushes-svg {
    width:       30px;
    scale:       1.22;
    paint-order: stroke;

    & > .stroke {
        stroke:       black;
        stroke-width: 5.8px;

        @supports (-moz-appearance: none) {
            stroke-linecap:  round;
            stroke-linejoin: round;
        }
    }

    & > .st0{ fill: #ff9e01; }
    & > .st1{ fill: #eb8f1e; }
    & > .st2{ fill: #BDC6C9; }
}

.size-number {
    display:     inline-block;
    margin-left: 8px;

    &.size-number-enter-active,
    &.size-number-leave-active {
        transition: translate 100ms ease, filter 100ms;
    }

    &.size-number-enter-from,
    &.size-number-leave-to {
        translate: 0 -2px;
        filter: brightness(1.1);
    }
}
</style>