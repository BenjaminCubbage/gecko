<template>
    <div class="canvas-editor">
        <PicEditorCanvas ref="picEditorCanvas" @canvasChanged="canvasChanged" />

        <div class="buttons">
            <PicEditorButton @click="send">SEND</PicEditorButton>
            <PicEditorButton @click="save">SAVE</PicEditorButton>
            <PicEditorButton @click="load">LOAD</PicEditorButton>
            <PicEditorButton @click="clear">CLEAR</PicEditorButton>
        </div>
    </div>
</template>

<script setup>
import { useTemplateRef, ref, inject } from 'vue';
import PicEditorButton from './PicEditorButton.vue';
import PicEditorCanvas from './PicEditorCanvas.vue';
import { CanvasSave } from '@/core/canvas/CanvasSave.js';
import { CanvasLoad } from '@/core/canvas/CanvasLoad.js';
import { Dispatch } from '@/core/dispatch/Dispatch.js';

const picEditorCanvas = useTemplateRef('picEditorCanvas');
const session = inject('session');
const idempotencyKey = ref(crypto.randomUUID());

function send() {
    if (!picEditorCanvas.value)
        console.error(`Could not resolve templated ref 'picEditorCanvas'`)
    else
        Dispatch.Post_SharedImage(
            session.value,
            idempotencyKey.value,
            session.value.activeUser().json()["user_id"],
            picEditorCanvas.value.readGIBBlob());
}

function save() {
    CanvasSave.promptSaveCompressed(picEditorCanvas.value?.getCanvasElement()).catch(()=>{});
}

function load() {
    CanvasLoad.promptLoadCompressed(picEditorCanvas.value?.getCanvasElement()).catch(()=>{});
}

function clear() {
    picEditorCanvas.value?.clear();
}

function canvasChanged() {
    idempotencyKey.value = crypto.randomUUID();
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