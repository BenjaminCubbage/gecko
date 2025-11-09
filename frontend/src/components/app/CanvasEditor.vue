<template>
    <div class="container">
        <DrawableCanvas ref="drawableCanvas" />
        
        <div class="buttons">
            <Button @click="save">SAVE</Button>
            <Button @click="clear">CLEAR</Button>
        </div>
    </div>
</template>

<script setup>
    import { useTemplateRef } from 'vue';
    import Button from '../ui/Button.vue';
    import DrawableCanvas from '../ui/DrawableCanvas.vue';
    import BMPBuilder from '../../core/BMPBuilder.js';

    const drawableCanvas = useTemplateRef('drawableCanvas');

    function save() {
        const ctx = drawableCanvas.value?.getCTX();

        if (!ctx)
        {
            console.error("Could not get canvas context to save.");
            return;
        }

        BMPBuilder.bmpFromCanvasCTX(ctx).save();
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