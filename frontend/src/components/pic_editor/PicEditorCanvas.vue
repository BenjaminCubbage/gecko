<template>
    <div class="canvas-frame" @pointerdown="dragMouse" @pointermove="dragMouse" @touchmove="dragTouch" @mouseleave="stopDragging" @touchend="stopDragging" @mouseup="stopDragging">
        <canvas ref="canvas" class="canvas" width="480" height="288"></canvas>
    </div>
</template>

<script setup>
import { watch, computed, onMounted, useTemplateRef } from 'vue';
import { CanvasUtils } from '@/core/canvas/CanvasUtils.js';
import { CanvasToGIB } from '@/core/canvas_gib/CanvasToGIB.js';
import { GIBToCanvas } from '@/core/canvas_gib/GIBToCanvas.js';

const props = defineProps({
    /* 'small' | 'medium' | 'large' */
    penSize: { type: String, default: 'small' },
    isErasing: { type: Boolean, default: false }
});

const emit = defineEmits([ 'canvasChanged' ]);

const canvas = useTemplateRef('canvas');
const ctx    = computed(() => canvas.value?.getContext('2d', { willReadFrequently: true }));

const lineWidth = computed(() => {
    const baseWidth = {
        'small':  3,
        'medium': 5,
        'large':  8
    }[props.penSize] ?? 5;

    return baseWidth * (props.isErasing ? 2 : 1);
});

const penColor = computed(() => 
    props.isErasing
        ? 'white'
        : 'black');

watch([ lineWidth, ctx ], () => {
    if (ctx.value)
        ctx.value.lineWidth = lineWidth.value;
});

watch([ penColor, ctx], () => {
    if (ctx.value)
        ctx.value.strokeStyle = penColor.value;
});

const draggingState = {
    isDragging: false,
    previousDragPosition: { x: 0, y: 0 }
}

onMounted(() => {
    if (!ctx.value) {
        console.error('Could not get canvas context on mount.');
        return;
    }

    ctx.value.fillStyle             = 'white';
    ctx.value.lineCap               = 'round';
    ctx.value.imageSmoothingEnabled = false;
    clear();
});

function dragMouse(e) {
    // Recursively handle coalesced events for smoother drawing
    if (e.getCoalescedEvents)
        for (const extra of e.getCoalescedEvents())
            dragMouse(extra);

    if (e.buttons !== 1) {
        stopDragging();
        return;
    }

    const rect = canvas.value.getBoundingClientRect();
    const x = e.clientX - rect.left;
    const y = e.clientY - rect.top;

    drag(x, y);
}

function dragTouch(e) {
    if (e.touches.length !== 1) {
        stopDragging();
        return;
    }

    const touch = e.touches[0];
    const rect = canvas.value.getBoundingClientRect();
    const x = touch.clientX - rect.left;
    const y = touch.clientY - rect.top;
    drag(x, y);

    /*
         * Some drag events aren't cancelable (like zooming)
         */
    if (e.cancelable && e.target === canvas.value)
        e.preventDefault();
}

function drag(x, y) {
    if (!ctx.value) return;

    if (!draggingState.isDragging) {
        draggingState.previousDragPosition.x = x;
        draggingState.previousDragPosition.y = y;
    }

    ctx.value.beginPath();
    ctx.value.moveTo(draggingState.previousDragPosition.x, draggingState.previousDragPosition.y);
    ctx.value.lineTo(x, y + 0.5); // +0.5 to fix subpixel rendering issues
    ctx.value.stroke();

    const changedX = Math.min(draggingState.previousDragPosition.x, x) - ctx.value.lineWidth - 10;
    const changedY = Math.min(draggingState.previousDragPosition.y, y) - ctx.value.lineWidth - 10;
    const changedWidth = Math.abs(draggingState.previousDragPosition.x - x) + ctx.value.lineWidth + 20;
    const changedHeight = Math.abs(draggingState.previousDragPosition.y - y) + ctx.value.lineWidth + 20;

    /*
         * Prevent anti-aliasing by snapping to pure bitonal in the changed area
         */
    CanvasUtils.snapToPureBitonal(ctx.value, changedX, changedY, changedWidth, changedHeight);

    draggingState.isDragging = true;
    draggingState.previousDragPosition.x = x;
    draggingState.previousDragPosition.y = y;

    emit('canvasChanged');
}

function stopDragging() {
    draggingState.isDragging = false;
}

function clear() {
    if (!ctx.value) {
        console.warn('Could not clear canvas because ctx is missing.');
        return;
    }

    ctx.value.save();
    ctx.value.fillStyle = 'white';
    ctx.value.fillRect(0, 0, canvas.value.width, canvas.value.height);
    ctx.value.restore();
}

defineExpose({
    getCTX: () => ctx.value,
    getCanvasElement: () => canvas.value,
    clear,
    readGIBBlob:  () => CanvasToGIB.readBlob(canvas.value),
    writeGIBBlob: blob => GIBToCanvas.writeBlob(canvas.value, blob)
});
</script>

<style>
    .canvas-frame {
        display: flex;
        flex-direction: column;
        align-items: center;
        gap: 12px;
        overflow: hidden;
        box-shadow: 6px 6px;

        border: var(--border-large);
        border-radius: var(--border-radius-large);
        box-shadow: var(--border-shadow-small);
    }

    .canvas {
        background: white;
        image-rendering: pixelated;
        image-rendering: crisp-edges;
    }
</style>