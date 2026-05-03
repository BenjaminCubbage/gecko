<template>
    <ToolBarChip
        :pen-size="penSize"
        color="yellow"
        :aria-label="`${penSize} brush`"
        @click="cycle">
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

const penSize = defineModel({
    type:     String,
    required: true,
    validator(value) {
        return [
            'small',
            'medium',
            'large'
        ].includes(value);
    }
});

function cycle() {
    penSize.value =
        penSize.value === 'small'
            ? 'medium'
            : penSize.value === 'medium'
                ? 'large'
                : 'small';
}
</script>