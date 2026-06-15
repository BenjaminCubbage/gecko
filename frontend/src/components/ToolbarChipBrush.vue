<template>
    <ToolBarChip
        ref="chipEl"
        :pen-size="brushSize"
        :is-selected="isSelected"
        color="yellow"
        :aria-label="`${brushSize} brush`"
        @click="onClick">
        <template #icon>
            <IconBrush height="48px" />
        </template>
    </ToolBarChip>
</template>

<script setup>
import {
    computed,
    useTemplateRef
} from 'vue';

import ToolBarChip from './ToolBarChip.vue';
import IconBrush   from './IconBrush.vue';

const isSelected = defineModel('is-selected', {
    type:     Boolean,
    required: true
});

const brushSize = defineModel('brush-size', {
    type:     String,
    required: true
});

const chipEl = useTemplateRef('chipEl');

function onClick() {
    if (!isSelected.value)
        isSelected.value = true;
    else
        cycle();
}

function cycle() {
    brushSize.value =
        brushSize.value === 'small'
            ? 'medium'
            : brushSize.value === 'medium'
                ? 'large'
                : 'small';
}
</script>