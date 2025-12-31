<template>
    <div
        class="recipient-select-carousel"
        :style="{
            'font-size': big ? '1.4rem' : '0.95rem'
        }">
        <button
            class="arrow arrow-left"
            @click="carouselPrev"
            :disabled="mode != 'loaded' || !hasPrev">
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
                <div
                    v-if="mode == 'loaded'"
                    v-for="option in options"
                    ref="measureOptionsEls"
                    style="display: grid; align-items: center;">
                    <RecipientSelectDeviceSignal
                        v-if="signal(modelValue) != null"
                        :status="signal(modelValue)" />

                    <StrokedText>
                        <slot name="label" :option="option"></slot>
                    </StrokedText>
                </div>

                <div v-else
                    class="loading"
                    ref="measureLoadingEl">
                    Loading...
                </div>
            </div>

            <div style="display: grid; align-items: center;">
                <transition name="loaded" mode="out-in">
                    <div v-if="mode == 'loaded'">
                        <StrokedText style="margin-bottom: -4px">
                            <slot name="label" :option="modelValue"></slot>
                        </StrokedText>

                        <RecipientSelectDeviceSignal
                            v-if="signal(modelValue) != null"
                            :status="signal(modelValue)" />
                    </div>

                    <div v-else class="loading">
                        <StrokedText>
                            Loading...
                        </StrokedText>
                    </div>
                </transition>
            </div>
        </div>

        <button
            class="arrow arrow-right"
            @click="carouselNext"
            :disabled="mode != 'loaded' || !hasNext">
            &gt;
        </button>
    </div>
</template>

<script setup>
import { computed, nextTick, onMounted, ref,
         useTemplateRef, watch } from 'vue';

import RecipientSelectDeviceSignal from './RecipientSelectDeviceSignal.vue';
import StrokedText from '@/components/stroked_text/StrokedText.vue';
import { useOnResize } from '@/composables/UseOnResize';

const { addResizeHandler, removeResizeHandler } = useOnResize();

const measureOptionsEls = useTemplateRef('measureOptionsEls');
const measureLoadingEl = useTemplateRef('measureLoadingEl');

watch(measureOptionsEls, (newEls, oldEls) => {
    if (!oldEls) {
        for (let v of newEls)
            addResizeHandler(v, updateMinMaxWidthForAnim);
    } else {
            for (let v of oldEls) if (!newEls.includes(i)) removeResizeHandler(v, updateMinMaxWidthForAnim);
            for (let v of newEls) if (!oldEls.includes(i)) addResizeHandler(v, updateMinMaxWidthForAnim);
    }
});

const props = defineProps({
    modelValue: { type: null, required: true },
    options: { type: Array, required: true },
    big: { type: Boolean, default: false },

    // devices only
    // () => 'online'  -> device is online
    // () => 'offline' -> device is offline
    // () => 'pending' -> connection status not yet known
    // () => null      -> don't show a status
    signal: { type: Function, default: () => null },

    // FSM
    // 'loading' -> loading screen shown
    // 'loaded'  -> recipients are loaded and should be displayed
    // 'error'   -> error shown
    mode: { type: String, default: 'loading' }
});

const emit = defineEmits([
    'update:modelValue'
]);

const curSelectionIndex = computed(() => props.options?.indexOf(props.modelValue) ?? -1);
const hasNext = computed(() => curSelectionIndex.value + 1 < props.options.length);
const hasPrev = computed(() => curSelectionIndex.value > 0);

// Manually setting starting max is a cheap hack to get it
// to smoothly transition on initial render from the loading
// state. I tried only setting the transition after the actual
// element loaded but found that was a race condition.
const animMaxWidth   = ref(props.big ? '124px' : '80px');
const animMinWidth   = ref('0px');

onMounted(updateMinMaxWidthForAnim);

watch(props, updateMinMaxWidthForAnim);

function updateMinMaxWidthForAnim() {
    // Give option elements a chance to be loaded into the DOM
    nextTick(() => {
        let element = props.mode == 'loading'
            ? measureLoadingEl?.value
            : measureOptionsEls.value[curSelectionIndex.value];

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
        emit('update:modelValue', props.options[newSel]);
}
</script>

<style scoped>
.recipient-select-carousel {
    font-size: 1rem;

    display: grid;
    grid-template-columns: auto 1fr auto;
    font-family: var(--font-heading);

    align-items: center;
    gap: 12px;

    height: 36px;
}

.loading {
    line-height: 1;
}

.measure-options {
    position: absolute;
    visibility: hidden;
    display: grid;
    justify-items: center;
}

.selected-option {
    display: inline-block;

    margin-left: 1.5px;
    line-height: 1;
    font-size: 2.4em;
    padding: 0 3px;

    user-select: none;
    text-align: center;

    transition: max-width 500ms cubic-bezier(.78,-0.01,.32,1),
                min-width 500ms cubic-bezier(.78,-0.01,.32,1);
}

.arrow {
    width: 1.35em;
    height: 1.35em;

    font-size: 2.3rem;
    line-height: 0;

    border: 2.5px solid black;
    box-shadow: 2px 2px 0 black;

    border-radius: 2px;
    color: black;

    background: linear-gradient(30deg, #91df43 25%, #b3e87d 25%, #b3e87d 50%, #a6ed5e 50%, #a6ed5e 75%, #ccebad 75%);
    background-size: 25% 100%;
    background-repeat: repeat;

    transition: transform 50ms ease;
}

.arrow-right {
    padding-left: 2.5px;
}

.arrow:disabled {
    cursor: default;
    opacity: 0.5;
}

.arrow-right:hover:not(:disabled) {
    transform: scale(1.1) rotateZ(-3deg);
}

.arrow-left:hover:not(:disabled) {
    transform: scale(1.1) rotateZ(3deg);
}

.arrow:active:not(:disabled) {
    transform: scale(0.97);
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