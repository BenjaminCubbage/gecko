<template>
    <div class="canvas-editor">
        <PicEditorBorder>
            <PicEditorCanvas 
                ref="picEditorCanvas" 
                :penSize="selectedPenSize"
                :isErasing="isErasing"
                @canvasChanged="canvasChanged" />
        </PicEditorBorder>

        <PicEditorBorder>
            <div class="toolbar">
                <PicEditorPenSize v-model="selectedPenSize" />
                <PicEditorEraseToggle v-model="isErasing" />
            </div>
        </PicEditorBorder>
    </div>
</template>

<script setup>
import { ref, useTemplateRef, inject } from 'vue';
import PicEditorBorder from './PicEditorBorder.vue';
import PicEditorPenSize from './PicEditorPenSize.vue';
import PicEditorEraseToggle from './PicEditorEraseToggle.vue';
import PicEditorCanvas from './PicEditorCanvas.vue';
import { Dispatch } from '@/core/dispatch/Dispatch.js';
import { Keys } from '@/core/store/Keys.js';

const picEditorCanvas = useTemplateRef('picEditorCanvas');
const selectedPenSize = ref('small');
const isErasing       = ref(false);

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
        gap: 8px;
    }

    .buttons {
        display: flex;
        gap: 16px;
    }

    .toolbar {
        display: flex;
        flex-direction: row wrap;
        gap: 12px;
    }
</style>