<template>
    <div
        tabindex="-1"
        class="
            canvas-frame
            shdw-after shdw-after--recessed shdw-after--otst-green"
        :disabled="isClearing"
        @pointerdown="dragMouse"
        @pointermove="dragMouse"
        @touchstart="dragTouch"
        @touchmove="dragTouch"
        @mouseleave="stopDragging"
        @touchend="stopDragging"
        @mouseup="stopDragging">
        <canvas ref="canvasEl" class="canvas" :width="resolutionX" :height="resolutionY"></canvas>
    </div>
</template>

<script setup>
import {
    computed,
    onMounted,
    ref,
    useTemplateRef,
    watch
} from 'vue';

import { CanvasClear } from '@/core/canvas/canvasClear.js';
import { CanvasUtils } from '@/core/canvas/canvasUtils.js';
import { CanvasToGIB } from '@/core/canvas_gib/canvasToGIB.js';
import { GIBToCanvas } from '@/core/canvas_gib/gibToCanvas.js';
import { delay }       from '@/core/async/delay.js';

const props = defineProps({
    penSize: { 
        type:    String, 
        default: 'small',
        validator(value) {
            return [
                'small',
                'medium',
                'large'
            ].includes(value);
        }
    },

    isErasing: { 
        type:    Boolean, 
        default: false
    }
});

const emit = defineEmits([ 'canvasChanged' ]);

const resolutionX = 400;
const resolutionY = 220;

const canvasEl = useTemplateRef('canvasEl');
const ctx = computed(() => canvasEl.value?.getContext('2d', {
    alpha:              false,
    desynchronized:     false,
    willReadFrequently: true
}));

const isClearing = ref(false);
const isBlank    = ref(true);

const lineWidth = computed(() => {
    const baseWidth = {
        'small':  3,
        'medium': 5,
        'large':  7
    }[props.penSize] ?? 5;

    return baseWidth * (props.isErasing ? 2 : 1);
});

const penColor = computed(() =>
    props.isErasing
        ? 'white'
        : 'black');

/*
    queuedLines is an array of coordinates. These coordinates
    get queued, and lines are drawn between them each
    animation frame.
*/
let queuedLines          = [];
let animationFrameHandle = null;

watch([ lineWidth, ctx ], () => {
    if (ctx.value)
        ctx.value.lineWidth = lineWidth.value;
});

watch([ penColor, ctx], () => {
    if (ctx.value)
        ctx.value.strokeStyle = penColor.value;
});

onMounted(() => {
    if (!ctx.value) {
        console.error('Could not get canvas context on mount.');
        return;
    }

    ctx.value.fillStyle             = 'white';
    ctx.value.lineCap               = 'round';
    ctx.value.imageSmoothingEnabled = false;
    clear(false);
});

function clientToPixelCoords({ clientX, clientY }) {
    if (!canvasEl.value)
        return { x: -1, y: -1 };

    const rect = canvasEl.value.getBoundingClientRect();

    return {
        x: Math.round((clientX - rect.left) * (canvasEl.value.width  / canvasEl.value.clientWidth)),
        y: Math.round((clientY - rect.top)  * (canvasEl.value.height / canvasEl.value.clientHeight))
    };
}

function dragMouse(e) {
    /*
        Handling coalesced events results in smoother curves when the
        mouse is moving quickly
    */
    if (e.getCoalescedEvents)
        for (const extra of e.getCoalescedEvents())
            dragMouse(extra);

    if (e.buttons !== 1) {
        stopDragging();
        return;
    }

    queueLine(clientToPixelCoords(e));
}

function dragTouch(e) {
    if (e.touches.length !== 1) {
        stopDragging();
        return;
    }

    queueLine(clientToPixelCoords(e.touches[0]));

    /*
        Some drag events aren't cancelable
    */
    if (e.cancelable)
        e.preventDefault();
}

function queueLine(pixelCoords) {
    queuedLines.push(pixelCoords);

    if (!animationFrameHandle)
        animationFrameHandle = requestAnimationFrame(drawQueuedLines);
}

function drawQueuedLines() {
    animationFrameHandle = null;

    if (!ctx.value || !queuedLines.length) {
        return;
    }

    /*
        The area of the screen that is redrawn
    */
    const dirtyRect = {
           t:0,
        l:0,  r:0,
           b:0
    };

    let previousX = queuedLines[0].x;
    let previousY = queuedLines[0].y;

    dirtyRect.l = dirtyRect.r = previousX;
    dirtyRect.t = dirtyRect.b = previousY;

    for (const { x, y } of queuedLines) {
        isBlank.value = false;

        dirtyRect.t = Math.min(y, dirtyRect.t);
        dirtyRect.b = Math.max(y, dirtyRect.b);
        dirtyRect.l = Math.min(x, dirtyRect.l);
        dirtyRect.r = Math.max(x, dirtyRect.r);

        ctx.value.beginPath();
        ctx.value.moveTo(previousX, previousY);
        ctx.value.lineTo(x, y);
        ctx.value.stroke();

        previousX = x;
        previousY = y;
    }

    queuedLines[0]     = queuedLines[queuedLines.length - 1];
    queuedLines.length = 1;

    dirtyRect.t -= ctx.value.lineWidth + 5;
    dirtyRect.b += ctx.value.lineWidth + 5;
    dirtyRect.l -= ctx.value.lineWidth + 5;
    dirtyRect.r += ctx.value.lineWidth + 5;

    /*
        Prevent anti-aliasing
    */
    CanvasUtils.snapToPureBitonal(
        ctx.value,
        dirtyRect.l,
        dirtyRect.t,
        dirtyRect.r - dirtyRect.l,
        dirtyRect.b - dirtyRect.t);

    emit('canvasChanged');
}

function stopDragging() {
    queuedLines.length = 0;
}

async function clear(animate = true) {
    if (!canvasEl.value)
        return;

    if (!animate) {
        CanvasClear.clearWithoutAnimation(canvasEl.value);
        return;
    }

    isClearing.value = true;
    CanvasClear.clear1(canvasEl.value);
    await delay(50);
    CanvasClear.clear2(canvasEl.value);
    await delay(50);
    CanvasClear.clear3(canvasEl.value);
    isClearing.value = false;
}

defineExpose({
    getCTX: () => ctx.value,
    getCanvasElement: () => canvasEl.value,
    clear,
    readGIBBlob:  () => CanvasToGIB.readBlob(canvasEl.value),
    writeGIBBlob: blob => GIBToCanvas.writeBlob(canvasEl.value, blob),
    isBlank
});
</script>

<style scoped>
.canvas-frame {
    contain: inline-size;
    
    display:        flex;
    flex-direction: column;
    gap:            12px;
    overflow:       hidden;
    position:       relative;

    padding: var(--shadow-dist-m);
    margin:  calc(var(--shadow-dist-m) * -1);

    &:active {
        cursor: crosshair;
    }

    &::after  { z-index: 1; }
    > .canvas { z-index: 0; }

    /*
        Using border-radius with overflow: none killed canvas
        performance due to off-screen rendering, so I'm using a
        pseudo-element to simulate it.
    */
    &::after {
        content:  '';

        position: absolute;
        inset:    var(--shadow-dist-m);
        
        --shdw-etc: 
            0 0 0 7px var(--col-green-3);

        border:        var(--border-s);
        border-radius: var(--radius-s);
    }

    > .canvas {
        background:      white;
        image-rendering: pixelated;
        transform:       translateZ(0);
    }
}
</style>