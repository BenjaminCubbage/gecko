<template>
    <ToolBarChip
        class="tool-bar-chip-eraser"
        :class="{ 'tool-bar-chip-eraser--selected': isToggled }">
        <template #pad>
            <div class="eraser-pad txtr-diag txtr-diag--magenta"></div>
        </template>

        <template #icons>
            <button
                class="icon"
                @click="isToggled = !isToggled">
                <svg
                    class="icon-svg"
                    version="1.1"
                    viewBox="-80 -80 980 980">
                    <polygon style="fill: var(--eraser-col-head)" points="740,500 740,460 700,460 700,420 660,420 660,380 620,380 620,420 580,420 580,460 540,460 540,500
                        500,500 500,540 460,540 460,580 420,580 420,620 380,620 380,660 420,660 420,700 460,700 460,740 500,740 500,780 540,780
                        540,740 580,740 580,700 620,700 620,660 660,660 660,620 700,620 700,580 740,580 740,540 780,540 780,500 	"/>
                    <polygon style="fill: var(--eraser-col-body)" points="380,540 420,540 420,500 460,500 460,460 500,460 500,420 540,420 540,380 580,380 580,340 620,340
                        620,300 580,300 580,260 540,260 540,220 500,220 500,180 460,180 460,140 420,140 420,100 380,100 380,60 340,60 340,20 300,20
                        260,20 220,20 220,60 180,60 180,100 140,100 140,140 100,140 100,180 60,180 60,220 20,220 20,260 20,300 20,340 60,340 60,380
                        100,380 100,420 140,420 140,460 180,460 180,500 220,500 220,540 260,540 260,580 300,580 300,620 340,620 340,580 380,580 	"/>
                </svg>
            </button>
        </template>
    </ToolBarChip>
</template>

<script setup>
import ToolBarChip from './ToolBarChip.vue';

const isToggled = defineModel({ default: true });
</script>

<style scoped>
.tool-bar-chip-eraser {
    --chip-offset:     0px;
    --eraser-col-body: #f69df4;
    --eraser-col-head: white;

    --filter-1: invert(0);
    --filter-2: invert(0);

    filter: var(--filter-1) var(--filter-2);

    &:is(:hover, :active):not(.tool-bar-chip-eraser--selected) {
        --filter-1: var(--filter-hl-1);
    }

    &:active,
    &.tool-bar-chip-eraser--selected,
    &.tool-bar-chip-eraser--selected:hover {
        --chip-offset: var(--shadow-dist-s);
        --s:           2.2px;

        --filter-2:
            drop-shadow(calc(var(--s) *  1.0) calc(var(--s) *  0.0) 0px white)
            drop-shadow(calc(var(--s) *  0.0) calc(var(--s) *  1.0) 0px white)
            drop-shadow(calc(var(--s) * -0.8) calc(var(--s) *  0.0) 0px white)
            drop-shadow(calc(var(--s) *  0.0) calc(var(--s) * -0.8) 0px white);
    }
}

.eraser-pad {
    box-shadow:
        0 calc(var(--shadow-dist-s) - var(--chip-offset))
        0 black,
        inset  3px  3px var(--col-magenta-0),
        inset -3px -3px var(--col-magenta-6);

    border:        var(--border-s);
    border-radius: var(--radius-s);

    transition:
        box-shadow 50ms linear,
        translate  50ms linear;

    translate: 0 var(--chip-offset);

    corner-shape: notch;
}

.icon {
    padding:    0 14px;
    transition: translate 50ms linear;
    translate:  0 var(--chip-offset);

    & > .icon-svg {
        height:       22px;
        stroke:       black;
        stroke-width: 150px;
        transform:    scale(1.6);
        paint-order:  stroke;
    }
}

/*
    Firefox uses rounded linejoins for the stroked-text
    property, and there's no way to change this.

    Set stroke-linejoin to round on FF to make the SVG
    visually consistent with text-based icons that have
    strokes set using the property.
*/
@supports (-moz-appearance:none) {
    .icon-svg {
        stroke-linecap:  round;
        stroke-linejoin: round;
    }
}
</style>