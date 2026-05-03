<template>
    <ToolBarChip
        ref="toolBarChipEl"
        class="tool-bar-chip-pen-size"
        :size="penSize"
        color="yellow"
        :aria-label="`${penSize} brush`"
        @click="cycle"
        v-bind="attrs">
        <template #icon>
            <IconBrush
                class="icon"
                :size="penSize" />
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
import IconBrush   from './IconBrush.vue';

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
.tool-bar-chip-pen-size--size-normal {
    gap: 7px;
}

.icon {
    height: 48px;
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