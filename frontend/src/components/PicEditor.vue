<template>
    <div class="pic-editor">
        <div class="
            canvas-border
            txtr-vert txtr-vert--green
            shdw shdw--inst-green shdw--elevated-l">
            <PicEditorCanvas
                class="pic-editor-canvas"
                ref="picEditorCanvas"
                :pen-size="penSize"
                :is-erasing="isErasing"
                @canvas-changed="canvasChanged" />
        </div>

        <ToolBar
            class="tool-bar"
            :is-send-disabled="recipientDevice == null"
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

const session  = inject(Keys.SessionStore);
const snackBar = inject(Keys.SnackBarStore);

const picEditorCanvas = useTemplateRef('picEditorCanvas');

const penSize   = ref('medium');
const isErasing = ref(false);

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
            picEditorCanvas.value.readGIBBlob())
            .onSuccess(() => {
                snackBar.pushMessage('Image sent successfully');
            })
            .onHttpError((_, status) => {
                snackBar.pushMessage(`Couldn't upload image: Status ${status}`);
            })
            .onNetworkError(() => {
                snackBar.pushMessage(`Couldn't upload image: Connection failed`);
            });
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
    isolation: isolate;

    display:        flex;
    flex-direction: column;

    > .canvas-border { z-index: 0; align-self: stretch; }
    > .tool-bar      { z-index: 1; align-self: center; }
}

.canvas-border {
    padding:        12px;
    padding-bottom: 26px;

    border-radius: var(--radius-s);
    border:        var(--border-s);
}

.tool-bar {
    translate: 0 -8px;
}
</style>