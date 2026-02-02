<template>
    <div
        class="recipient-select-carousel"
        :style="{
            'font-size': big ? '1.4rem' : '0.95rem'
        }">
        <button
            class="arrow arrow-left txtr-spiky txtr-spiky--green"
            @click="carouselPrev"
            :disabled="mode != 'ready' || !hasPrev">
            &lt;
        </button>

        <div
            class="selected-option"
            :style="{
                'max-width': animMaxWidth,
                'min-width': animMinWidth
            }">
            <!-- Measuring only (pos absolute, for width anim) -->
            <div class="measure-options">
                <template v-if="mode == 'ready'">
                    <div
                        v-for="option in options"
                        ref="measureOptionsEls"
                        style="display: grid; align-items: center;"
                        :key="optionID(option)">
                        <RecipientSelectDeviceSignal
                            v-if="selectedOption != null && signal(selectedOption) != null"
                            :status="signal(selectedOption)" />

                        <StrokedText ellipses>
                            <slot name="label" :option="option"></slot>
                        </StrokedText>
                    </div>
                </template>
                <div v-else
                    class="loading"
                    ref="measureLoadingEl">
                    Loading...
                </div>
            </div>

            <div style="display: grid; align-items: center;">
                <transition name="loaded" mode="out-in">
                    <div v-if="mode == 'ready'">
                        <StrokedText style="margin-bottom: -4px" ellipses>
                            <slot name="label" :option="selectedOption"></slot>
                        </StrokedText>

                        <RecipientSelectDeviceSignal
                            v-if="selectedOption != null && signal(selectedOption) != null"
                            :status="signal(selectedOption)" />
                    </div>

                    <div v-else class="loading">
                        <StrokedText ellipses>
                            Loading...
                        </StrokedText>
                    </div>
                </transition>
            </div>
        </div>

        <button
            class="arrow arrow-right txtr-spiky txtr-spiky--green"
            @click="carouselNext"
            :disabled="mode != 'ready' || !hasNext">
            &gt;
        </button>
    </div>
</template>

<script setup>
import { computed, nextTick, onMounted, ref,
    useTemplateRef, watch, watchEffect } from 'vue';

import RecipientSelectDeviceSignal from './RecipientSelectDeviceSignal.vue';
import StrokedText from '@/components/stroked_text/StrokedText.vue';
import { useOnResize } from '@/composables/UseOnResize';

const props = defineProps({
    options:    { type: Array, required: true },
    optionID:   { type: Function, required: true },
    big:        { type: Boolean, default: false },

    // devices only
    // () => 'online'  -> device is online
    // () => 'offline' -> device is offline
    // () => 'pending' -> connection status not yet known
    // () => null      -> don't show a status
    signal: { type: Function, default: () => null },

    // 'loading' -> loading screen shown
    // 'ready'  -> recipients are loaded and should be displayed
    // 'error'   -> error shown
    mode: { type: String, default: 'loading' }
});

const selectedOption = defineModel({ required: true });

const { addResizeHandler, removeResizeHandler } = useOnResize();

const measureOptionsEls = useTemplateRef('measureOptionsEls');
const measureLoadingEl = useTemplateRef('measureLoadingEl');

watchEffect(() => {
    if (props.options?.length > 0 && !props.options.includes(selectedOption.value))
        selectedOption.value = props.options[0];
});

watch(measureOptionsEls, (newEls, oldEls) => {
    if (!oldEls) {
        for (const v of newEls)
            addResizeHandler(v, updateMinMaxWidthForAnim);
    } else {
        for (const v of oldEls) if (!newEls.includes(v)) removeResizeHandler(v, updateMinMaxWidthForAnim);
        for (const v of newEls) if (!oldEls.includes(v)) addResizeHandler(v, updateMinMaxWidthForAnim);
    }
});

const curSelectionIndex = computed(() => props.options?.indexOf(selectedOption.value) ?? -1);
const hasNext = computed(() => curSelectionIndex.value + 1 < props.options.length);
const hasPrev = computed(() => curSelectionIndex.value > 0);

/*
    Manually setting starting max is a cheap hack to get it
    to smoothly transition on initial render from the loading
    state. I tried only setting the transition after the actual
    element loaded but found that was a race condition. 
*/
const animMaxWidth   = ref(props.big ? '200px' : '200px');
const animMinWidth   = ref('0px');

onMounted(updateMinMaxWidthForAnim);

watch(props, updateMinMaxWidthForAnim);

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

    font-family: var(--font-heading);
    font-size:   1rem;
}

.measure-options {
    display:       grid;
    justify-items: center;
    position:      absolute;
    visibility:    hidden;
}

.selected-option {
    display:     inline-block;
    margin-left: 1.5px;
    padding:     0 3px;

    font-size:   2.4em;
    line-height: 1;
    text-align:  center;
    user-select: none;

    transition:
        max-width 500ms cubic-bezier(.78,-0.01,.32,1),
        min-width 500ms cubic-bezier(.78,-0.01,.32,1);
}

.arrow {
    height: 1.35em;
    width:  1.35em;

    color:     black;
    font-size:   2.3rem;
    line-height: 0;

    border-radius: var(--radius-s);
    border:        2.5px solid black;
    box-shadow:    var(--shadow-s);

    transition: transform 50ms ease;
}

.arrow-right {
    padding-left: 2.5px;
}

.arrow:disabled {
    cursor:  default;
    opacity: 0.5;
}

.arrow-right:hover:not(:disabled) {
    transform: scale(1.06);
}

.arrow-left:hover:not(:disabled) {
    transform: scale(1.06);
}

.arrow:active:not(:disabled) {
    box-shadow: none;
    transform:  scale(0.97);
}

.loaded-enter-active,
.loaded-leave-active {
    transition: transform 50ms ease;
}

.loaded-enter-from,
.loaded-leave-to {
    transform: scale(0.8);
}
</style>