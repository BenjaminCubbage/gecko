<template>
    <div class="
        pic-editor 
        txtr-vert txtr-vert--green
        shdw shdw--inst-green shdw--elevated-l">
        <PicEditorLogInPrompt
            class="pic-editor-log-in-prompt" />

        <PicEditorCanvas
            class="pic-editor-canvas"
            ref="picEditorCanvas"
            :pen-size="penSize"
            :is-erasing="isErasing"
            @canvas-changed="canvasChanged" />

        <ToolBar
            class="tool-bar"
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

import PicEditorCanvas      from './PicEditorCanvas.vue';
import PicEditorLogInPrompt from './PicEditorLogInPrompt.vue';
import ToolBar              from './ToolBar.vue';

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

    grid-template:
        "canvas"   auto
        "tool-bar" auto /
         1fr;

    display:        grid;
    gap:            10px;
    padding:        9px;

    border-radius: var(--radius-s);
    border:        var(--border-s);

    & > .canvas   { grid-area: canvas; }
    & > .tool-bar { grid-area: tool-bar; place-self: stretch; }

    & > .pic-editor-log-in-prompt {
        z-index: 1;
        grid-area: 
            canvas / 
            canvas / 
            tool-bar / 
            tool-bar;
        place-self: center;
    }
}

.pic-editor-log-in-prompt {
    position: absolute;
    inset:    0;
    width:    fit-content;
    height:   fit-content;
}
</style>