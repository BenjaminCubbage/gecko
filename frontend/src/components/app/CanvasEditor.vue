<template>
    <div class="container">
        <DrawableCanvas ref="drawableCanvas" />
        
        <div class="buttons">
            <Button @click="save">SAVE</Button>
            <Button @click="load">LOAD</Button>
            <Button @click="clear">CLEAR</Button>
        </div>
    </div>
</template>

<script setup>
    import { useTemplateRef } from 'vue';
    import Button from '../ui/Button.vue';
    import DrawableCanvas from '../ui/DrawableCanvas.vue';
    import { CanvasSave } from '../../core/canvas/CanvasSave.js';
    import { CanvasLoad } from '../../core/canvas/CanvasLoad.js';

    const drawableCanvas = useTemplateRef('drawableCanvas');

    function save() {
        CanvasSave.promptSaveCompressed(drawableCanvas.value?.getCanvasElement()).catch(_=>{});
    }

    function load() {
        var xmlHttp = new XMLHttpRequest();
        xmlHttp.open("GET", '/api/hi', false);
        xmlHttp.send(null);
        console.log(xmlHttp.responseText);

        CanvasLoad.promptLoadCompressed(drawableCanvas.value?.getCanvasElement()).catch(_=>{});
    }

    function clear() {
        drawableCanvas.value?.clear();
    }
</script>

<style>
    .container {
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