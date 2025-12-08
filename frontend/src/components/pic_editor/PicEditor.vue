<template>
    <div class="canvas-editor">
        <PicEditorCanvas ref="picEditorCanvas" />
        
        <div class="buttons">
            <PicEditorButton @click="save">SAVE</PicEditorButton>
            <PicEditorButton @click="load">LOAD</PicEditorButton>
            <PicEditorButton @click="clear">CLEAR</PicEditorButton>
        </div>
    </div>
</template>

<script setup>
import { useTemplateRef } from 'vue';
import PicEditorButton from './PicEditorButton.vue';
import PicEditorCanvas from './PicEditorCanvas.vue';
import { CanvasSave } from '@/core/canvas/CanvasSave.js';
import { CanvasLoad } from '@/core/canvas/CanvasLoad.js';

const picEditorCanvas = useTemplateRef('picEditorCanvas');

function save() {
    CanvasSave.promptSaveCompressed(picEditorCanvas.value?.getCanvasElement()).catch(()=>{});
}

function load() {
    CanvasLoad.promptLoadCompressed(picEditorCanvas.value?.getCanvasElement()).catch(()=>{});
}

function clear() {
    picEditorCanvas.value?.clear();
}
</script>

<style>
    .canvas-editor {
        display: flex;
        flex-direction: column;
        align-items: center;
        gap: 12px;
    }

    .buttons {
        display: flex;
        gap: 16px;
    }
</style>