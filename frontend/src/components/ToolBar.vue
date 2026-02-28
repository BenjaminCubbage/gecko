<template>
    <menu
        class="tool-bar"
        aria-label="Toolbar">
        <li>
            <ToolBarChipPenSize v-model="penSize" />
        </li>
        <li>
            <ToolBarChipEraser v-model="isErasing" />
        </li>
        <li>
            <ToolBarChipClear @click="emit('clear')" />
        </li>
        <li>
            <ToolBarChipSend :disabled="sendDisabled" @click="emit('send')" />
        </li>
    </menu>
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
    isolation:       isolate;
    justify-content: center;
    padding:         4px 0;

    background:
        linear-gradient(
            var(--col-gray-2) 50%,
            var(--col-gray-3) 50%);

    box-shadow:
             var(--shadow-inst-dist)            var(--shadow-inst-dist)       var(--col-green-6),
        calc(var(--shadow-inst-dist) * -1) calc(var(--shadow-inst-dist) * -1) var(--col-green-0),
        var(--shadow-inst-gray);

    border-radius: var(--radius-s);
    border:        var(--border-s);

    corner-shape: notch;
}
</style>