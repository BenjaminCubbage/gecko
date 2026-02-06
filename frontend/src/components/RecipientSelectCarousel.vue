<template>
    <!-- Measuring only (for width anim) -->
    <teleport to="body">
        <div class="measure-options">
            <div
                v-for="option in options"
                ref="measureOptionsEls"
                class="text"
                :style="`font-size: ${fontSize}`"
                :key="optionID(option)">
                <RecipientSelectDeviceSignal
                    v-if="isDevicesVariant"
                    :status="selectedOption.status" />
                {{ optionContent(option) }}
            </div>
        </div>
    </teleport>

    <div class="recipient-select-carousel">
        <button
            class="arrow arrow-left txtr-diag txtr-diag--green"
            @click="carouselPrev"
            :disabled="!hasPrev">
            &lt;
        </button>

        <div
            class="selected-option text text--display"
            :style="{
                'max-width': animMaxWidth,
                'min-width': animMinWidth,
                'font-size': fontSize
            }">

            {{ optionContent(selectedOption) }}

            <RecipientSelectDeviceSignal
                v-if="isDevicesVariant"
                :status="selectedOption.status" />
        </div>

        <button
            class="arrow arrow-right txtr-diag txtr-diag--green"
            @click="carouselNext"
            :disabled="!hasNext">
            &gt;
        </button>
    </div>
</template>

<script setup>
import {
    computed,
    nextTick,
    onMounted,
    ref,
    useTemplateRef,
    watch
} from 'vue';

import RecipientSelectDeviceSignal from './RecipientSelectDeviceSignal.vue';

import { useDetectResize } from '@/composables/useDetectResize';

import { Device } from '@/models/device.js';
import { User }   from '@/models/user.js';

const props = defineProps({
    variant: {
        type:      String,
        required:  true,
        validator(value) {
            return [
                'users',
                'devices'
            ].includes(value);
        }
    },

    options: {
        type:     Array,
        required: true,
        validator(value, props) {
            return value?.length && value.every(v => {
                return props.variant === 'users'
                    ? v instanceof User
                    : v instanceof Device;
            });
        }
    }
});

const selectedOption = defineModel({
    validator(value, props) {
        return props.options.includes(value);
    }
});

const measureOptionsEls = useTemplateRef('measureOptionsEls');
const measureLoadingEl  = useTemplateRef('measureLoadingEl');

const {
    addResizeHandler,
    removeResizeHandler
} = useDetectResize();

const isUsersVariant   = computed(() => props.variant === 'users');
const isDevicesVariant = computed(() => !isUsersVariant.value);

const fontSize = computed(() =>
    isUsersVariant.value
        ? '3.2rem'
        : '2.2rem'
);

const curSelectionIndex = computed(() => props.options?.indexOf(selectedOption.value) ?? -1);
const hasNext           = computed(() => curSelectionIndex.value + 1 < props.options.length);
const hasPrev           = computed(() => curSelectionIndex.value > 0);

watch(selectedOption, () => {
    if (!props.options.includes(selectedOption.value))
        selectedOption.value = props.options[0];
    else
        updateMinMaxWidthForAnim();
});

watch(() => props.options, () => {
    if (!props.options.includes(selectedOption.value))
        selectedOption.value = props.options[0];
}, {
    immediate: true
});

watch(measureOptionsEls, (newEls, oldEls) => {
    if (!oldEls) {
        for (const v of newEls)
            addResizeHandler(v, updateMinMaxWidthForAnim);
    } else {
        for (const v of oldEls) if (!newEls.includes(v)) removeResizeHandler(v, updateMinMaxWidthForAnim);
        for (const v of newEls) if (!oldEls.includes(v)) addResizeHandler(v,    updateMinMaxWidthForAnim);
    }
});

/*
    Manually setting starting max is a cheap hack to get it
    to smoothly transition on initial render from the loading
    state. I tried only setting the transition after the actual
    element loaded but found that was a race condition.
*/
const animMaxWidth = ref('26px');
const animMinWidth = ref('26px');

onMounted(updateMinMaxWidthForAnim);

function optionID(option) {
    return props.variant === 'users'
        ? option.userID
        : option.deviceID;
}

function optionContent(option) {
    return props.variant === 'users'
        ? option.username
        : option.name;
}

function updateMinMaxWidthForAnim() {
    /*
        Give option elements a chance to be loaded into the DOM.
    */
    nextTick(() => {
        /*
            Vue does not gaurantee order of v-for ref arrays, so we must
            use compareDocumentPosition to get true ordering.
        */
        const element = props.mode == 'loading'
            ? measureLoadingEl?.value
            : measureOptionsEls.value?.toSorted((el1, el2) =>
                el1.compareDocumentPosition(el2) & 4 ? -1 : 1)[curSelectionIndex.value];

        if (element && element.offsetWidth) {
            animMaxWidth.value = `${element.offsetWidth + 12}px`;
            animMinWidth.value = `${element.offsetWidth + 12}px`;
        }
    });
}

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
    align-items:           center;
    display:               grid;
    gap:                   12px;
    grid-template-columns: auto 1fr auto;
    height:                36px;
}

.measure-options {
    display:     grid;
    place-items: start;
    position:    fixed;
    visibility:  hidden;
    z-index:     100;
}

.selected-option {
    display:       inline-block;
    justify-items: center;
    margin-left:   1.5px;

    transition:
        max-width 500ms cubic-bezier(.78,-0.01,.32,1),
        min-width 500ms cubic-bezier(.78,-0.01,.32,1);
}

.arrow {
    height: 1.35em;
    width:  1.35em;

    color:       black;
    font-family: var(--font-heading);
    font-size:   2.3rem;
    line-height: 0;

    --arrow-offset: 0px;

    box-shadow:
        calc(var(--shadow-dist-s) - var(--arrow-offset))
        calc(var(--shadow-dist-s) - var(--arrow-offset))
        0
        black;

    border-radius: var(--radius-s);
    border:        2.5px solid black;

    transform: translate(
        var(--arrow-offset),
        var(--arrow-offset));

    transition: transform 80ms ease;
}

.arrow-right {
    padding-left: 2.5px;
}

.arrow:disabled {
    cursor:  default;
    opacity: 0.5;
}

.arrow:hover:not(:disabled) {
    --arrow-offset: calc(var(--shadow-dist-s) / 2);
}

.arrow:active:not(:disabled) {
    --arrow-offset: var(--shadow-dist-s);
}

.loaded-enter-active,
.loaded-leave-active {
    transition: transform 50ms ease;
}

.loaded-enter-from,
.loaded-leave-to {
    transform: scale(0.8);
}

.text {
    padding:     0 3px;
    font-family: var(--font-heading);
}

.text--display {
    overflow: hidden;
    width:    100%;

    -webkit-text-stroke: 4px white;
    color:               black;
    line-height:         1;
    text-align:          center;
    text-overflow:       ellipsis;
    user-select:         none;
    white-space:         nowrap;

    paint-order:         stroke;
}

@supports (text-overflow: "") {
    .text--display {
        text-overflow: "";
    }
}
</style>