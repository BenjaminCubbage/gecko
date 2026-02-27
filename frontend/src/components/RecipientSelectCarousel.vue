<template>
    <fieldset class="recipient-select-carousel">
        <StrokedText is="legend" class="legend" stroke-color="black" stroke-thickness="4px">
            <slot name="label"></slot>
        </StrokedText>

        <button
            class="arrow arrow--left txtr-diag txtr-diag--green"
            :aria-label="ariaLabelPrev"
            :disabled="!hasPrev"
            @click="carouselPrev">
            &lt;
        </button>

        <output class="selection" aria-status="true">
            <slot name="option" :option="selectedOption" />
        </output>

        <button
            class="arrow arrow--right txtr-diag txtr-diag--green"
            :aria-label="ariaLabelNext"
            :disabled="!hasNext"
            @click="carouselNext">
            &gt;
        </button>
    </fieldset>
</template>

<script setup>
import {
    computed,
    watch
} from 'vue';

import StrokedText from './StrokedText.vue';

const props = defineProps({
    options: {
        type:     Array,
        required: true
    },

    ariaLabelPrev: {
        type:     String,
        required: true
    },

    ariaLabelNext: {
        type:     String,
        required: true
    }
});

const selectedOption = defineModel({
    validator(value, props) {
        return props.options.includes(value);
    }
});

const curSelectionIndex = computed(() => props.options?.indexOf(selectedOption.value) ?? -1);
const hasNext           = computed(() => curSelectionIndex.value + 1 < props.options.length);
const hasPrev           = computed(() => curSelectionIndex.value > 0);

watch(selectedOption, () => {
    if (!props.options.includes(selectedOption.value))
        selectedOption.value = props.options[0];
});

watch(() => props.options, () => {
    if (!props.options.includes(selectedOption.value))
        selectedOption.value = props.options[0];
}, {
    immediate: true
});

function carouselPrev() {
    tryMoveSelection(-1);
}

function carouselNext() {
    tryMoveSelection(+1);
}

function tryMoveSelection(by) {
    const newSel = curSelectionIndex.value + by;
    if (newSel >= 0 && newSel < props.options.length)
        selectedOption.value = props.options[newSel];
}
</script>

<style scoped>
.recipient-select-carousel {
    contain:   strict;
    isolation: isolate;

    grid-template:
        ".          legend         ."           14px
        "arrow-left selection      arrow-right" 1fr /
         auto       minmax(0, 1fr) auto;

    display:        grid;
    gap:            1px 0;
    height:         52px;
    padding-bottom: var(--shadow-dist-s);
    user-select:    none;

    .legend    { place-self: end left; z-index: 1; }
    .selection { place-self: stretch;  z-index: 0; }
    .arrow     { place-self: stretch;  z-index: 0; }
}

.legend {
    grid-area: legend;

    overflow:      hidden;
    padding:       0 var(--text-stroke-width-s);
    text-overflow: ellipsis;
    user-select:   none;
    white-space:   nowrap;

    color:          transparent;
    font-size:      2rem;
    letter-spacing: 0.04em;
    line-height:    1;

    background:
        linear-gradient(to right,
            var(--col-green-0) 2.5px,
            transparent        2.5px),
        linear-gradient(
            var(--col-green-0) 6px,
            var(--col-green-2) 6px calc(100% - 6px),
            var(--col-green-4) calc(100% - 6px));

    background-clip: text;

    filter:
        drop-shadow(0 var(--shadow-dist-s) black);

    translate: 4px 6px;
}

.selection {
    grid-area: selection;

    display:     grid;
    place-items: center;

    -webkit-text-stroke: var(--text-stroke-s);
    color:               black;
    overflow-x:          auto;
    scrollbar-width:     none;
    text-overflow:       ellipsis;
    user-select:         none;
    white-space:         nowrap;
    line-height:         0;

    background:
        linear-gradient(
            var(--col-gray-2) 50%,
            var(--col-gray-3) 50%);

    box-shadow:
        0 var(--shadow-dist-s)
        0 black,
        var(--shadow-inst-gray);

    border: var(--border-s);

    paint-order: stroke;
}

.arrow {
    width: 1.9em;

    text-shadow:
        -1.5px -1.5px 0 var(--col-green-1),
         1.5px  1.5px 0 var(--col-green-5);

    color:       black;
    font-size:   2.3rem;
    line-height: 0;

    box-shadow:
        0 calc(var(--shadow-dist-s) - var(--arrow-offset))
        0 black,
        var(--shadow-inst-green);

    border-radius: var(--radius-s);
    border:        2.5px solid black;

    --arrow-offset: 0px;

    transition:
        box-shadow 50ms ease,
        translate  50ms ease;

    translate: 0 var(--arrow-offset);

    corner-shape: notch;
}

.arrow:disabled {
    cursor:         default;
    pointer-events: none;
    color:          var(--col-green-6);
}

.arrow:hover,
.arrow:active {
    filter: var(--filter-hl-1);
}

.arrow:active {
    --arrow-offset: var(--shadow-dist-s);
}

.arrow--left {
    grid-area:                  arrow-left;
    border-top-right-radius:    0;
    border-bottom-right-radius: 0;
}

.arrow--right {
    grid-area:                 arrow-right;
    border-top-left-radius:    0;
    border-bottom-left-radius: 0;
}
</style>