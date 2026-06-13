<template>
    <div
        class="recipient-select-carousel"
        :class="`recipient-select-carousel--${variant}`">
        <BaseLabel
            :id="labelElId"
            class="label">
            <slot name="label"></slot> {{ selectedOptionDisplayNumber }}
        </BaseLabel>

        <button
            tabindex="-1"
            class="
                arrow arrow--left
                txtr-diag txtr-diag--green
                shdw shdw--inst-green shdw--elevated-s"
            :disabled="!hasPrev"
            aria-label="Previous"
            @click="carouselPrev">
            &lt;
        </button>

        <button
            tabindex="-1"
            class="
                arrow arrow--right
                txtr-diag txtr-diag--green
                shdw shdw--inst-green shdw--elevated-s"
            :disabled="!hasNext"
            aria-label="Next"
            @click="carouselNext">
            &gt;
        </button>

        <span
            class="
                selection
                shdw shdw--inst-gray shdw--elevated-s
                txtr-vert txtr-vert--gray"
            ref="spinButtonEl"
            role="spinbutton"
            tabindex="0"
            :aria-label="ariaLabel"
            :aria-labelledby="ariaLabel ? null : labelElId"
            aria-valuemin="0"
            :aria-valuemax="options.length"
            :aria-valuenow="curSelectionIndex"
            :aria-valuetext="`${selectedOptionLabel}, ${selectedOptionStatusLabel?.ariaText ?? ''}`">
            <template v-if="selectedOptionLabel">
                {{ selectedOptionLabel }}
            </template>

            <LoadingSpinner v-else class="loading-spinner" />
        </span>

        <div
            v-if="variant === 'with-status' && !hideOptionStatus"
            class="
                status
                shdw shdw--inst-lt-gray shdw--elevated-s"
            :class="`status--${selectedOptionStatusLabel?.color ?? 'blue'}`"
            aria-hidden>
            <template v-if="selectedOptionStatusLabel">
                <span class="status-dot"></span>
                <span class="status-text">
                    {{ selectedOptionStatusLabel.text }}
                </span>
            </template>

            <LoadingSpinner v-else class="loading-spinner" />
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
import LoadingSpinner         from './LoadingSpinner.vue';
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
        If this is false but the variant is with-status, the
        space for the status bar is reserved but hidden.
    */
    hideOptionStatus: {
        type: Boolean,
        default: false,
        validator(value, props) {
            return !value || props.variant === 'with-status';
        }
    },

    /*
        Function signature:

        (option) => ({
            color:    'red' | 'green' | 'blue',
            text:     String,
            ariaText: String
        } | null)
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
    return props.variant === 'with-status' && curSelectionIndex.value !== -1
        ? props.getOptionStatusLabel(selectedOption.value)
        : null;
});

const selectedOptionDisplayNumber = computed(() => {
    return curSelectionIndex.value !== -1
        ? `${curSelectionIndex.value + 1}/${props.options.length}`
        : '-/-';
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
    --ht-label:     14px;
    --ht-selection: 36px;
    --ht-status:    26px;

    isolation: isolate;

    display:        grid;
    padding-bottom: var(--shadow-dist-s);

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

    .label      { place-self: end left;     z-index: 2; }
    .selection  { place-self: stretch;      z-index: 1; }
    .arrow      { place-self: stretch;      z-index: 1; }
    .status     { place-self: stretch center; z-index: 0; }
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
    white-space:         nowrap;
    line-height:         0;
    user-select:         none;
    cursor:              default;

    border: var(--border-s);
}

.arrow {
    width: 2.2em;

    text-shadow:
        -1.5px -1.5px 0 var(--col-green-1),
         1.5px  1.5px 0 var(--col-green-5);

    color:       black;
    font-size:   2rem;
    line-height: 0;
    font-weight: bold;

    border-radius: var(--radius-s);
    border:        var(--border-s);

    translate:
        0
        calc(var(--shadow-dist-s) - var(--shdw-dist-elevation));

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
        --shdw-dist-elevation: 0px;
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
    width:           110px;
    user-select:     none;

    -webkit-text-stroke: var(--text-stroke-s);
    font-size:           1.88rem;
    letter-spacing:      0.05em;
    line-height:         0.8;
    text-transform:      uppercase;

    background: var(--col-lt-gray-3);

    border-radius:
        0               0
        var(--radius-s) var(--radius-s);

    border: var(--border-s);

    &.status--red   { color: var(--col-red-7); }
    &.status--green { color: var(--col-green-7); }
    &.status--blue  { color: var(--col-blue-4); }

    > .status-dot {
        display: inline-block;
        height:  0.5em;
        width:   0.5em;
        cursor:  default;

        box-shadow:
            0 0
            0 calc(var(--text-stroke-width-s) / 2) white;

        background:    currentColor;
        border-radius: 2.3px;

        @supports not (corner-shape: notch) {
            height: 0.35em;
            width:  0.35em;

            border-radius: 2px;
            rotate:        45deg;
        }
    }

    > .status-text {
        cursor: default;
    }
}

.loading-spinner {
    font-size: 2.6rem;
    color:     black;
}
</style>