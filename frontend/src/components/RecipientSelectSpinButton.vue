<template>
    <div
        class="recipient-select-carousel"
        :class="`recipient-select-carousel--${variant}`">
        <BaseLabel 
            :id="labelElId"
            class="label"
            variant="no-box">
            <slot name="label"></slot> {{ curSelectionIndex + 1 }}/{{ options.length }}
        </BaseLabel>

        <button
            tabindex="-1"
            class="arrow arrow--left txtr-diag txtr-diag--green"
            :disabled="!hasPrev"
            aria-label="Previous"
            @click="carouselPrev">
            &lt;
        </button>

        <button
            tabindex="-1"
            class="arrow arrow--right txtr-diag txtr-diag--green"
            :disabled="!hasNext"
            aria-label="Next"
            @click="carouselNext">
            &gt;
        </button>

        <span 
            class="selection"
            ref="spinButtonEl"
            role="spinbutton"
            tabindex="0"
            :aria-label="ariaLabel"
            :aria-labelledby="ariaLabel ? null : labelElId"
            aria-valuemin="0"
            :aria-valuemax="options.length"
            :aria-valuenow="curSelectionIndex"
            :aria-valuetext="`${selectedOptionLabel}, ${selectedOptionStatusLabel?.ariaText ?? ''}`">
            {{ selectedOptionLabel }} 
        </span>

        <div
            v-if="variant === 'with-status' && selectedOptionStatusLabel"
            class="status"
            :class="`status--${selectedOptionStatusLabel.color}`"
            :key="selectedOptionStatusLabel.text"
            aria-hidden="true">
            <span class="status-dot"></span>
            <span class="status-text">
                {{ selectedOptionStatusLabel.text }}
            </span>
        </div>
    </div>
</template>

<script setup>
import {
    computed,
    watch,
    useId,
    useTemplateRef
} from 'vue';

import BaseLabel              from './BaseLabel.vue';
import { useArrowNavigation } from '@/composables/useArrowNavigation';

const props = defineProps({
    variant: {
        type:    String,
        default: 'normal',
        validator(value) {
            return [
                'normal',
                'with-status'
            ].includes(value);
        }
    },
    
    options: {
        type:     Array,
        required: true
    },

    getOptionLabel: {
        type:     Function,
        required: true
    },

    /*
        Function signature:

        (option) => ({
            color:    'red' | 'green' | 'blue',
            text:     String,
            ariaText: String
        })
    */
    getOptionStatusLabel: {
        type:     Function,
        required: false,
        validator(value, props) {
            return (
                value == null ^
                props.variant === 'with-status');
        }
    },

    ariaLabel: {
        type:     String,
        required: false
    }
});

const selectedOption = defineModel({
    validator(value, props) {
        return props.options.includes(value);
    }
});

const labelElId = useId();

const {
    onArrowLeft,
    onArrowRight,
    onArrowUp,
    onArrowDown
} = useArrowNavigation(useTemplateRef('spinButtonEl'));

onArrowLeft (carouselPrev);
onArrowRight(carouselNext);
onArrowUp   (carouselPrev);
onArrowDown (carouselNext);

const curSelectionIndex = computed(() => props.options?.indexOf(selectedOption.value) ?? -1);
const hasNext           = computed(() => curSelectionIndex.value + 1 < props.options.length);
const hasPrev           = computed(() => curSelectionIndex.value > 0);

const selectedOptionLabel = computed(() => {
    return props.getOptionLabel(selectedOption.value);
});

const selectedOptionStatusLabel = computed(() => {
    return props.variant === 'with-status'
        ? props.getOptionStatusLabel(selectedOption.value)
        : null;
});

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

function carouselPrev() { tryMoveSelection(-1); }
function carouselNext() { tryMoveSelection(+1); }

function tryMoveSelection(by) {
    const newSel = curSelectionIndex.value + by;
    if (newSel >= 0 && newSel < props.options.length)
        selectedOption.value = props.options[newSel];
}
</script>

<style scoped>
.recipient-select-carousel {
    --ht-label:      14px;
    --ht-selection:  34px;
    --ht-status:     26px;

    contain:   content;
    isolation: isolate;

    display:        grid;
    padding-bottom: var(--shadow-dist-s);
    user-select:    none;

    &.recipient-select-carousel--normal {
        grid-template:
            ".          label          ."           var(--ht-label)
            "arrow-left selection      arrow-right" var(--ht-selection) /
            auto        minmax(0, 1fr) auto;
    }

    &.recipient-select-carousel--with-status {
        grid-template:
            ".          label          ."           var(--ht-label)
            "arrow-left selection      arrow-right" var(--ht-selection)
            ".          status         ."           var(--ht-status) /
            auto        minmax(0, 1fr) auto;
    }

    .label { place-self: end left;     z-index: 2; }
    .selection  { place-self: stretch;      z-index: 1; }
    .arrow      { place-self: stretch;      z-index: 1; }
    .status     { place-self: start center; z-index: 0; }
}

.label {
    grid-area: label;
    translate: 4px 4px;
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

    &.arrow--left {
        grid-area:                  arrow-left;
        border-top-right-radius:    0;
        border-bottom-right-radius: 0;
    }

    &.arrow--right {
        grid-area:                 arrow-right;
        border-top-left-radius:    0;
        border-bottom-left-radius: 0;
    }

    &:hover,
    &:active {
        filter: var(--filter-hl-1);
    }

    &:active {
        --arrow-offset: var(--shadow-dist-s);
    }

    &:disabled {
        cursor:         default;
        pointer-events: none;
        color:          var(--col-green-6);
    }
}

.status {
    grid-area: status;

    align-items:     center;
    display:         flex;
    gap:             6px;
    justify-content: center;
    padding-bottom:  4px;
    padding-top:     calc(3px + var(--shadow-inst-dist));
    width:           110px;
    height:          100%;

    -webkit-text-stroke: var(--text-stroke-s);
    font-size:           1.7rem;
    letter-spacing:      0.04em;
    line-height:         0.8;
    text-transform:      uppercase;

    background:
        linear-gradient(
            var(--col-lt-gray-2) 50%,
            var(--col-lt-gray-4) 50%);

    border-radius:
        0               0
        var(--radius-s) var(--radius-s);

    box-shadow: 
        var(--shadow-s),
        inset 3px 3px var(--col-lt-gray-0),
        inset -3px -3px var(--col-lt-gray-6);

    border: var(--border-s);

    translate: 0 calc(var(--shadow-inst-dist) * -1 - 0.5px);

    corner-shape: notch;
    paint-order:  stroke;

    &.status--red   { color: var(--col-red-7); }
    &.status--green { color: var(--col-green-7); }
    &.status--blue  { color: var(--col-blue-4); }

    & > .status-dot {
        display: inline-block;
        height:  0.4em;
        width:   0.4em;

        box-shadow:    
            0 0 
            0 calc(var(--text-stroke-width-s) / 2) white;

        background:    currentColor;
        border-radius: 1.5px;

        corner-shape: notch;

        @supports not (corner-shape: notch) {
            height: 0.35em;
            width:  0.35em;

            border-radius: 2px;
            rotate:        45deg;
        }
    }
}
</style>