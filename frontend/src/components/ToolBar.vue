<template>
    <div class="tool-bar">
        <ToolBarChipPenSize v-model="penSize" />
        <ToolBarChipEraser v-model="isErasing" />
        <ToolBarChipClear @click="emit('clear')" />
        <ToolBarChipSend :disabled="sendDisabled" @click="emit('send')" />
    </div>
</template>

<script setup>
import ToolBarChipClear   from './ToolBarChipClear.vue';
import ToolBarChipEraser  from './ToolBarChipEraser.vue';
import ToolBarChipPenSize from './ToolBarChipPenSize.vue';
import ToolBarChipSend    from './ToolBarChipSend.vue';

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

const penSize = defineModel('penSize', {
    type:     String,
    required: true
});

const isErasing = defineModel('isErasing', {
    type:     Boolean,
    required: true
});
</script>

<style scoped>
.tool-bar {
    display:         flex;
    gap:             12px;
    isolation:       isolate;
    justify-content: center;
    padding:         9px;

    background:
        linear-gradient(
            var(--col-gray-2) 50%,
            var(--col-gray-3) 50%);
            
    box-shadow:
         3px  3px var(--col-green-6),
        -3px -3px var(--col-green-0),
        inset  3px  3px 0 var(--col-gray-0),
        inset -3px -3px 0 var(--col-gray-4);

    border-radius: var(--radius-s);
    border:        var(--border-s);

    corner-shape: notch;
}
</style>