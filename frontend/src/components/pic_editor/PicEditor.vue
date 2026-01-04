<template>
    <div class="canvas-editor">
        <PicEditorCanvas ref="picEditorCanvas" @canvasChanged="canvasChanged" />

        <div class="buttons">
            <PicEditorButton @click="send">SEND</PicEditorButton>
            <PicEditorButton @click="getLatest">GET LATEST</PicEditorButton>
            <PicEditorButton @click="clear">CLEAR</PicEditorButton>
        </div>
    </div>
</template>

<script setup>
import { useTemplateRef, inject } from 'vue';
import PicEditorButton from './PicEditorButton.vue';
import PicEditorCanvas from './PicEditorCanvas.vue';
import { Dispatch } from '@/core/dispatch/Dispatch.js';
import { Keys } from '@/core/store/Keys.js';

const picEditorCanvas = useTemplateRef('picEditorCanvas');
const session = inject(Keys.SessionStore);

let idempotencyKey = crypto.randomUUID();

function send() {
    if (!picEditorCanvas.value)
        throw new Error('Could not resolve templated ref');

    Dispatch.Post_SharedImage(
        session.activeUserID(),
        session.xsrfCookie(),
        idempotencyKey,
        session.activeUserID(),
        picEditorCanvas.value.readGIBBlob());
}

function getLatest() {
    Dispatch.Get_LatestImage(session.activeUserID(), session.xsrfCookie())
        .onSuccess(async body => {
            const arr = new Uint8Array(await body.arrayBuffer());
            picEditorCanvas.value.writeGIBBlob(arr);
        })
        .onHttpError((body, status) => {
            console.warn(`Get_LatestImage failed: ${status}`, body)
        });
}

// function save() {
//     CanvasSave.promptSaveCompressed(picEditorCanvas.value?.getCanvasElement()).catch(()=>{});
// }

// function load() {
//     CanvasLoad.promptLoadCompressed(picEditorCanvas.value?.getCanvasElement()).catch(()=>{});
// }

function clear() {
    picEditorCanvas.value?.clear();
}

function canvasChanged() {
    idempotencyKey = crypto.randomUUID();
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