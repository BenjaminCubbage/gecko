<template>
    <ToolBarChip
        class="tool-bar-chip-clear"
        color="red"
        @click="onClick">
        <template #icon>
            <IconTrash ref="iconEl" height="47px" />
        </template>
    </ToolBarChip>
</template>

<script setup>
import {
    ref,
    useTemplateRef
} from 'vue';

import ToolBarChip from './ToolBarChip.vue';
import IconTrash   from './IconTrash.vue';

const emit = defineEmits([ 'click' ]);

const iconEl = useTemplateRef('iconEl');

function onClick() {
    iconEl.value?.animateShaking();
    emit('click');
}
</script>