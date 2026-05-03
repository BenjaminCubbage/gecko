<template>
    <ToolBarChip
        ref="toolBarChipEl"
        :is-pressed="isToggled"
        color="lt-gray"
        @click="isToggled = !isToggled">
        <template #icon>
            <IconEraser class="icon" />
        </template>
    </ToolBarChip>
</template>

<script setup>
import {
    computed,
    useTemplateRef
} from 'vue';

import ToolBarChip from './ToolBarChip.vue';
import IconEraser  from './IconEraser.vue';

const isToggled = defineModel({ default: true });

const toolBarChipEl = useTemplateRef('toolBarChipEl');

defineExpose({
    innerElement: computed(() => toolBarChipEl.value?.innerElement)
});
</script>

<style scoped>
.icon {
    height: 47px;
}
</style>