<template>
    <div class="pic-editor-erase-toggle">
        <button
            class="erase"
            :class="{'selected': isToggled}"
            @click="isToggled = !isToggled">
            <svg
                class="icon"
                version="1.1" id="Layer_1"
                xmlns:svgjs="http://svgjs.dev/svgjs"
                viewBox="-80 -80 980 980">
                <polygon class="st0" points="740,500 740,460 700,460 700,420 660,420 660,380 620,380 620,420 580,420 580,460 540,460 540,500
                    500,500 500,540 460,540 460,580 420,580 420,620 380,620 380,660 420,660 420,700 460,700 460,740 500,740 500,780 540,780
                    540,740 580,740 580,700 620,700 620,660 660,660 660,620 700,620 700,580 740,580 740,540 780,540 780,500 	"/>
                <polygon class="st1" points="380,540 420,540 420,500 460,500 460,460 500,460 500,420 540,420 540,380 580,380 580,340 620,340
                    620,300 580,300 580,260 540,260 540,220 500,220 500,180 460,180 460,140 420,140 420,100 380,100 380,60 340,60 340,20 300,20
                    260,20 220,20 220,60 180,60 180,100 140,100 140,140 100,140 100,180 60,180 60,220 20,220 20,260 20,300 20,340 60,340 60,380
                    100,380 100,420 140,420 140,460 180,460 180,500 220,500 220,540 260,540 260,580 300,580 300,620 340,620 340,580 380,580 	"/>
                </svg>
        </button>
    </div>
</template>

<script setup>
import { defineModel } from 'vue';

const isToggled = defineModel({ default: true });
</script>

<style scoped>
.pic-editor-erase-toggle {
    height: 36px;
    display: grid;
    place-content: end;
}

.erase {
    height: 24px;
    margin: 0 4px;
    color: black;
    padding: 0 8px;

    line-height: 0;
    background: white;

    border-radius: 3px;
    border: var(--border-small);

    box-shadow: 2px 2px 0 black;

    background: linear-gradient(-30deg,
        #fc5aff 25%,
        #feb0ff 25% 50%,
        #e573ff 50% 75%,
        #ffdffc 75%
    );

    transform: translate(-1px, -1px);
    transition: transform 50ms ease,
                box-shadow 50ms ease;
}

.icon {
    height: 22px;
    paint-order: stroke;
    stroke: black;
    stroke-width: 150px;
    transform: scale(1.6) translate(0, -6px);
}

.st0{ fill: white; }
.st1{ fill: #f69df4; }

.erase:hover {
    transform: translate(0px, 0px);
    box-shadow: 1px 1px 0 black;
}

.erase.selected {
    transform: translate(1px, 1px);
    box-shadow: none;

    --s: 1px;

    filter: drop-shadow(calc(var(--s) *  1.0) calc(var(--s) *  0.0) 0px white)
            drop-shadow(calc(var(--s) *  1.0) calc(var(--s) *  1.0) 0px white) 
            drop-shadow(calc(var(--s) *  0.0) calc(var(--s) *  1.0) 0px white) 
            drop-shadow(calc(var(--s) * -0.8) calc(var(--s) *  1.0) 0px white) 
            drop-shadow(calc(var(--s) * -0.8) calc(var(--s) *  0.0) 0px white) 
            drop-shadow(calc(var(--s) * -0.8) calc(var(--s) * -0.8) 0px white) 
            drop-shadow(calc(var(--s) *  0.0) calc(var(--s) * -0.8) 0px white) 
            drop-shadow(calc(var(--s) *  1.0) calc(var(--s) * -0.8) 0px white);
}

/*
    Firefox uses rounded linejoins for the stroked-text 
    property, and there's no way to change this.

    Set stroke-linejoin to round on FF to make the SVG
    visually consistent with text-based icons that have
    strokes set using the property.
*/
@supports (-moz-appearance:none) {
    .icon {
        stroke-linejoin: round;
        stroke-linecap: round;
    }
}
</style>