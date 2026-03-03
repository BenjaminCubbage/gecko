<template>
    <ToolBarChip
        ref="toolBarChipEl"
        class="tool-bar-chip-eraser"
        color="magenta"
        :pressed="isToggled"
        aria-label="Eraser"
        highlight-style="filter"
        @click="isToggled = !isToggled">
        <template #icon>
            <svg
                class="icon-svg"
                version="1.1"
                viewBox="-80 -80 980 980">
                <polygon class="st0" points="740,500 740,460 700,460 700,420 660,420 660,380 620,380 620,420 580,420 580,460 540,460 540,500
                    500,500 500,540 460,540 460,580 420,580 420,620 380,620 380,660 420,660 420,700 460,700 460,740 500,740 500,780 540,780
                    540,740 580,740 580,700 620,700 620,660 660,660 660,620 700,620 700,580 740,580 740,540 780,540 780,500 	"/>
                <polygon class="st1" points="380,540 420,540 420,500 460,500 460,460 500,460 500,420 540,420 540,380 580,380 580,340 620,340
                    620,300 580,300 580,260 540,260 540,220 500,220 500,180 460,180 460,140 420,140 420,100 380,100 380,60 340,60 340,20 300,20
                    260,20 220,20 220,60 180,60 180,100 140,100 140,140 100,140 100,180 60,180 60,220 20,220 20,260 20,300 20,340 60,340 60,380
                    100,380 100,420 140,420 140,460 180,460 180,500 220,500 220,540 260,540 260,580 300,580 300,620 340,620 340,580 380,580 	"/>
            </svg>
        </template>
    </ToolBarChip>
</template>

<script setup>
import { 
    computed,
    useTemplateRef
} from 'vue';

import ToolBarChip from './ToolBarChip.vue';

const isToggled = defineModel({ default: true });

const toolBarChipEl = useTemplateRef('toolBarChipEl');

defineExpose({
    innerElement: computed(() => toolBarChipEl.value?.innerElement)
});
</script>

<style scoped>
.icon-svg {
    width:       30px;
    stroke:       black;
    stroke-width: 150px;
    transform:    scale(1.2);
    paint-order:  stroke;
    translate:    0.2px 0.5px;

    & > .st0 { fill: #FFFFFF; }
    & > .st1 { fill: #F69DF4; }
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