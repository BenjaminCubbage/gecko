<template>
    <div class="recipient-select-carousel">
        <button
            class="arrow arrow--left txtr-diag txtr-diag--green"
            @click="carouselPrev"
            :disabled="!hasPrev">
            &lt;
        </button>

        <div class="selection">
            <slot name="label" :option="selectedOption" />
        </div>

        <button
            class="arrow arrow--right txtr-diag txtr-diag--green"
            @click="carouselNext"
            :disabled="!hasNext">
            &gt;
        </button>
    </div>
</template>

<script setup>
import {
    computed,
    watch
} from 'vue';

const props = defineProps({
    options: {
        type:     Array,
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
    contain: strict;
    contain-intrinsic-height: 34px;

    grid-template:
        "arrow-left selection      arrow-right" auto /
         auto       minmax(0, 1fr) auto;

    align-items:    stretch;
    justify-items:  stretch;
    display:        grid;
    padding-bottom: var(--shadow-dist-s);
    user-select:    none;
    z-index:        0;
}

.selection {
    display:       grid;
    grid-area:     selection;
    place-items:   center;
    z-index:       1;

    -webkit-text-stroke: var(--text-stroke-l);
    color:               black;
    font-family:         var(--font-heading);
    overflow:            hidden;
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
        inset  3px  3px 0 var(--col-gray-0),
        inset -3px -3px 0 var(--col-gray-4);

    border: var(--border-s);

    paint-order: stroke;
}

.arrow {
    width:   1.9em;
    z-index: 1;

    text-shadow:
        -1.5px -1.5px 0 var(--col-green-1),
         1.5px  1.5px 0 var(--col-green-5);

    color:       black;
    font-family: var(--font-heading);
    font-size:   2.3rem;
    line-height: 0;

    box-shadow:
        0 calc(var(--shadow-dist-s) - var(--arrow-offset))
        0 black,
        inset  3px  3px 0 var(--col-green-0),
        inset -3px -3px 0 var(--col-green-5);

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