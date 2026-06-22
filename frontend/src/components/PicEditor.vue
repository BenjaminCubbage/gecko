<template>
    <div class="pic-editor">
        <div class="
            canvas-border
            txtr-vert txtr-vert--green
            shdw shdw--inst-green shdw--elevated-l">
            <PicEditorCanvas
                ref="canvasEl"
                class="pic-editor-canvas"
                :brush-size="brushSize"
                :eraser-size="eraserSize"
                :is-erasing="isErasing"
                @canvas-changed="canvasChanged" />

            <PicEditorStatus
                ref="statusEl"
                :isLoading="showIsLoading"
                class="pic-editor-status" />
        </div>

        <ToolBar
            class="tool-bar"
            :is-send-disabled="recipientDevice == null"
            v-model:brush-size="brushSize"
            v-model:eraser-size="eraserSize"
            v-model:is-erasing="isErasing"
            @send="send"
            @clear="clear" />

        <PicEditorUndoRedoButtons
            :canUndo="canvasEl?.canUndo ?? false"
            :canRedo="canvasEl?.canRedo ?? false"
            @undo="canvasEl?.undo"
            @redo="canvasEl?.redo" />
    </div>
</template>

<script setup>
import {
    inject,
    ref,
    useTemplateRef
} from 'vue';

import PicEditorCanvas          from './PicEditorCanvas.vue';
import PicEditorStatus          from './PicEditorStatus.vue';
import PicEditorUndoRedoButtons from './PicEditorUndoRedoButtons.vue';
import ToolBar                  from './ToolBar.vue';

import { useThrottledRef } from '@/composables/useThrottledRef.js';

import { Dispatch } from '@/core/dispatch/Dispatch.js';
import { Keys }     from '@/core/di/keys.js';

const props = defineProps({
    recipientDevice: {
        type:     null,
        required: true
    }
});

const session = inject(Keys.SessionStore);

const canvasEl = useTemplateRef('canvasEl');
const statusEl = useTemplateRef('statusEl');

const brushSize  = ref('medium');
const eraserSize = ref('medium');
const isErasing  = ref(false);

/* Show loading status as we're posting image. */
const isLoading     = ref(false);
const showIsLoading = useThrottledRef(isLoading, 500);

let idempotencyKey = crypto.randomUUID();

function send() {
    if (!canvasEl.value)
        throw new Error('Could not resolve templated ref');

    if (props.recipientDevice != null) {
        isLoading.value = true;

        Dispatch.Post_SharedImage(
            session.activeUserID,
            session.xsrfCookie,
            idempotencyKey,
            props.recipientDevice.deviceID,
            canvasEl.value.readGIBBlob())
            .onSuccess(() => {
                statusEl.value?.pushStatus({
                    statusType: 'success',
                    statusText: 'Sent'
                });
                isLoading.value = false;
            })
            .onHttpError((_, status) => {
                statusEl.value?.pushStatus({
                    statusType: 'error',
                    statusText: `Error Sending: ${status}`
                });
                isLoading.value = false;
            })
            .onNetworkError(() => {
                statusEl.value?.pushStatus({
                    statusType: 'error',
                    statusText: `Couldn\'t connect`
                });
                isLoading.value = false;
            });
    }
}

function clear() {
    canvasEl.value?.clear();
}

function canvasChanged() {
    idempotencyKey = crypto.randomUUID();
}
</script>

<style scoped>
.pic-editor {
    position: relative;

    display:        flex;
    flex-direction: column;

    /* I'm giving a wide bottom margin to account for the
       */
    margin-bottom: var(--ht-undo-redo);

    > .canvas-border { z-index: 0; align-self: stretch; }
    > .tool-bar      { z-index: 1; align-self: center; }
}

.canvas-border {
    display: grid;

    padding:        12px;
    padding-bottom: 26px;

    border-radius: var(--radius-s);
    border:        var(--border-s);

    & > .pic-editor-canvas { z-index: 0; grid-area: 1 / 1; place-self: stretch; }
    & > .pic-editor-status { z-index: 1; grid-area: 1 / 1; place-self: end; }
}

.pic-editor-status {
    --_inset-status: 8px;

    margin-bottom: var(--_inset-status);
    margin-right:  var(--_inset-status);
}

.tool-bar {
    translate: 0 -8px;
}
</style>