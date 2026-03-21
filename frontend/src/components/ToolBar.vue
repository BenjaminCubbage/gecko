<template>
    <menu
        ref="toolBarEl"
        class="
            tool-bar
            shdw shdw--inst-gray shdw--otst-green"
        aria-label="Tool Bar"
        v-roving-container>
        <li><ToolBarChipPenSize v-roving-item ref="chipPenSizeEl" :size="chipPenSizeSize" v-model="penSize" /></li>
        <li><ToolBarChipEraser  v-roving-item ref="chipEraserEl"                          v-model="isErasing" /></li>
        <li><ToolBarChipClear   v-roving-item ref="chipClearEl"   :size="chipClearSize"   @click="emit('clear')" /></li>
        <li><ToolBarChipSend    v-roving-item ref="chipSendEl"    :size="chipSendSize"    :disabled="sendDisabled" @click="emit('send')" /></li>
    </menu>
</template>

<script setup>
import {
    computed,
    useTemplateRef
} from 'vue';

import ToolBarChipClear   from './ToolBarChipClear.vue';
import ToolBarChipEraser  from './ToolBarChipEraser.vue';
import ToolBarChipPenSize from './ToolBarChipPenSize.vue';
import ToolBarChipSend    from './ToolBarChipSend.vue';

import { useElementDimensions } from '@/composables/useElementDimensions.js';

const props = defineProps({
    sendDisabled: {
        type:     Boolean,
        required: true
    }
});

const emit = defineEmits([
    'clear',
    'send'
]);

const penSize   = defineModel('penSize',   { type: String,  required: true });
const isErasing = defineModel('isErasing', { type: Boolean, required: true });

const toolBarEl = useTemplateRef('toolBarEl');

const { inline: toolbarWidth } = useElementDimensions(toolBarEl);

const chipPenSizeSize = computed(() => toolbarWidth.value > 400 ? 'normal' : 'small');
const chipClearSize   = computed(() => toolbarWidth.value > 450 ? 'normal' : 'small');
const chipSendSize    = computed(() => toolbarWidth.value > 350 ? 'normal' : 'small');
</script>

<style scoped>
.tool-bar {
    contain: layout inline-size;

    display:         flex;
    isolation:       isolate;
    justify-content: center;
    padding:         4px 0;

    background:
        linear-gradient(
            var(--col-gray-2) 50%,
            var(--col-gray-3) 50%);

    border-radius: var(--radius-s);
    border:        var(--border-s);
}
</style>