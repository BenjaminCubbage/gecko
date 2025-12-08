<template>
    <div class="canvas-editor">
        <DrawableCanvas ref="drawableCanvas" />
        
        <div class="buttons">
            <EditorButton @click="save">SAVE</EditorButton>
            <EditorButton @click="load">LOAD</EditorButton>
            <EditorButton @click="clear">CLEAR</EditorButton>
        </div>
    </div>
</template>

<script setup>
import { useTemplateRef } from 'vue';
import EditorButton from './EditorButton.vue';
import DrawableCanvas from './DrawableCanvas.vue';
import { CanvasSave } from '@/core/canvas/CanvasSave.js';
import { CanvasLoad } from '@/core/canvas/CanvasLoad.js';

const drawableCanvas = useTemplateRef('drawableCanvas');

function save() {
    CanvasSave.promptSaveCompressed(drawableCanvas.value?.getCanvasElement()).catch(()=>{});
}

function load() {
    CanvasLoad.promptLoadCompressed(drawableCanvas.value?.getCanvasElement()).catch(()=>{});
}

function clear() {
    drawableCanvas.value?.clear();
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