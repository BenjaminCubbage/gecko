<template>
    <div class="canvas-editor">
        <PicEditorBorder variant="outer">
            <PicEditorCanvas
                ref="picEditorCanvas"
                :penSize="selectedPenSize"
                :isErasing="isErasing"
                @canvasChanged="canvasChanged" />

            <PicEditorBorder variant="inner">
                <div class="toolbar">
                    <ToolbarChipPenSize v-model="selectedPenSize" />
                    <ToolbarChipEraser v-model="isErasing" />
                    <ToolbarChipClear @click="clear" />
                    <ToolbarChipSend :disabled="recipientDevice == null" @click="send" />
                </div>
            </PicEditorBorder>
        </PicEditorBorder>
    </div>
</template>

<script setup>
import {
    inject,
    ref,
    useTemplateRef
} from 'vue';

import ToolbarChipClear   from './ToolbarChipClear.vue';
import ToolbarChipEraser  from './ToolbarChipEraser.vue';
import ToolbarChipPenSize from './ToolbarChipPenSize.vue';
import ToolbarChipSend    from './ToolbarChipSend.vue';

import PicEditorBorder from './PicEditorBorder.vue';
import PicEditorCanvas from './PicEditorCanvas.vue';

import { Dispatch } from '@/core/dispatch/Dispatch.js';
import { Keys }     from '@/core/di/keys.js';

const props = defineProps({
    recipientDevice: { required: true }
});

const picEditorCanvas = useTemplateRef('picEditorCanvas');
const selectedPenSize = ref('small');
const isErasing       = ref(false);

const session = inject(Keys.SessionStore);
let idempotencyKey = crypto.randomUUID();

function send() {
    if (!picEditorCanvas.value)
        throw new Error('Could not resolve templated ref');

    if (props.recipientDevice != null) {
        Dispatch.Post_SharedImage(
            session.activeUserID,
            session.xsrfCookie,
            idempotencyKey,
            props.recipientDevice.deviceID,
            picEditorCanvas.value.readGIBBlob());
    }
}

void getLatest;
function getLatest() {
    Dispatch.Get_LatestImage(session.activeUserID, session.xsrfCookie)
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
    align-items:    center;
    display:        flex;
    flex-direction: column;
    gap:            8px;
}

.buttons {
    display: flex;
    gap:     16px;
}

.toolbar {
    display:         flex;
    gap:             12px;
    justify-content: center;
    padding-top:     2px;
}
</style>