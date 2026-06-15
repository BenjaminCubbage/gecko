<template>
    <menu class="
        tool-bar
        shdw shdw--elevated-m
        shdw-after  shdw-after--inst-gray  shdw-after--elevated-m
        shdw-before shdw-before--inst-gray shdw-before--elevated-m"
        aria-label="Toolbar"
        v-roving-container>
        <!-- <div inert class="knobs knobs--left  shdw-after shdw-after--inst-lt-gray shdw-after--elevated-s shdw-before shdw-before--inst-lt-gray"></div>
        <div inert class="knobs knobs--right shdw-after shdw-after--inst-lt-gray shdw-after--elevated-s shdw-before shdw-before--inst-lt-gray"></div> -->

        <li><ToolbarChipBrush  v-model:is-selected="isPainting" v-model:brush-size="brushSize"   v-roving-item /></li>
        <li><ToolBarChipEraser v-model:is-selected="isErasing"  v-model:eraser-size="eraserSize" v-roving-item /></li>
        <li><ToolBarChipClear  @click="emit('clear')"                                            v-roving-item /></li>

        <li v-if="!isSendDisabled">
            <ToolBarChipSend v-roving-item @click="emit('send')" />
        </li>
    </menu>
</template>

<script setup>
import { 
    ref,
    watch
} from 'vue';

import ToolBarChipClear  from './ToolBarChipClear.vue';
import ToolBarChipEraser from './ToolBarChipEraser.vue';
import ToolbarChipBrush  from './ToolbarChipBrush.vue';
import ToolBarChipSend   from './ToolBarChipSend.vue';

defineProps({
    isSendDisabled: {
        type:    Boolean,
        default: false
    }
});

const emit = defineEmits([
    'clear',
    'send'
]);

const brushSize  = defineModel('brushSize',  { type: String,  required: true });
const eraserSize = defineModel('eraserSize', { type: String,  required: true })
const isErasing  = defineModel('isErasing',  { type: Boolean, required: true });
const isPainting = ref(true);

watch(isPainting, newValue => isErasing.value  = !newValue, { immediate: true });
watch(isErasing,  newValue => isPainting.value = !newValue, { immediate: true });
</script>

<style scoped>
.tool-bar {
    display: flex;
    gap:     18px;
    padding: 0 24px;

    background:    var(--col-lt-gray-4);
    border:        var(--border-s);
    border-radius: var(--radius-s);

    --shdw-etc:
        inset  3px  3px var(--col-lt-gray-0),
        inset -3px -3px var(--col-gray-4);
}

/* .knobs {
    position:       absolute;
    inset:          0;
    pointer-events: none;

    &::before,
    &::after {
        content:  '';
        display:  block;
        position: absolute;

        width:  21px;
        height: 18px;
        border:        var(--border-s);
        border-radius: var(--radius-s);

        background: var(--col-lt-gray-3);
    }

    &.knobs--left {
        &::before { inset: -6px auto auto -6px; border-radius: var(--radius-s) 0 var(--radius-s) 0; }
        &::after  { inset: auto auto -6px -6px; border-radius: 0 var(--radius-s) 0 var(--radius-s); }
    }
    &.knobs--right {
        &::before { inset: -6px -6px auto auto; border-radius: 0 var(--radius-s) 0 var(--radius-s); }
        &::after  { inset: auto -6px -6px auto; border-radius: var(--radius-s) 0 var(--radius-s) 0; }
    }
} */
</style>