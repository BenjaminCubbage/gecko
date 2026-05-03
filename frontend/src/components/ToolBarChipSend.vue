<template>
    <ToolBarChip
        color="green"
        class="tool-bar-chip-send"
        :is-busy="isLaunching"
        @click="click">
        <template #icon>
            <IconRocket ref="iconEl" height="46.5px" />
        </template>
    </ToolBarChip>
</template>

<script setup>
import {
    ref,
    useTemplateRef
} from 'vue';

import ToolBarChip from './ToolBarChip.vue';
import IconRocket  from './IconRocket.vue';

defineProps({
    disabled: {
        type:    Boolean,
        default: false
    }
});

const emit = defineEmits([
    'click'
]);

const iconEl      = useTemplateRef('iconEl');
const isLaunching = ref(false);

async function click() {
    emit('click');
    isLaunching.value = true;
    iconEl.value?.animateLaunch().then(() =>
        isLaunching.value = false);
}
</script>