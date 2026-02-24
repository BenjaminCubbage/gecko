<template>
    <div class="pic-editor txtr-diag txtr-diag--green">
        <PicEditorCanvas
            ref="picEditorCanvas"
            :pen-size="penSize  "
            :is-erasing="isErasing"
            @canvas-changed="canvasChanged" />

        <ToolBar
            :send-disabled="recipientDevice == null"
            v-model:pen-size="penSize"
            v-model:is-erasing="isErasing"
            @send="send"
            @clear="clear" />
    </div>
</template>

<script setup>
import {
    inject,
    ref,
    useTemplateRef
} from 'vue';

import PicEditorCanvas from './PicEditorCanvas.vue';
import ToolBar         from './ToolBar.vue';

import { Dispatch } from '@/core/dispatch/Dispatch.js';
import { Keys }     from '@/core/di/keys.js';

const props = defineProps({
    recipientDevice: {
        type:     null,
        required: true
    }
});

const picEditorCanvas = useTemplateRef('picEditorCanvas');

const penSize   = ref('medium');
const isErasing = ref(false);

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

<style scoped>
.pic-editor {
    display:        flex;
    flex-direction: column;
    gap:            10px;
    padding:        9px;

    box-shadow:
        var(--shadow-l),
        inset  3px  3px 0 var(--col-green-0),
        inset -3px -3px 0 var(--col-green-6);

    border-radius: var(--radius-s);
    border:        var(--border-s);

    corner-shape: notch;
}
</style>