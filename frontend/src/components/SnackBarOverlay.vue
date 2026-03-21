<template>
    <section
        class="snack-bar-overlay"
        aria-label="Status message">
        <transition name="expand" mode="out-in">
            <TextContentBorder
                v-if="!isHidden"
                class="snack-bar-content"
                :data-dismissed-manually="dismissedManually"
                :key="latestMessage.id">
                <span aria-hidden="true" style="display: contents">
                    {{ latestMessage.text }}
                </span>

                <button
                    ref="closeButtonEl"
                    class="
                        close-button 
                        txtr-diag txtr-diag--dk-red
                        shdw shdw--inst-red shdw--elevated-s"
                    aria-label="Hide status"
                    @click="handleClick"
                    @focus="handleFocus">
                    x
                </button>
            </TextContentBorder>
        </transition>

        <!-- SR -->
        <div
            role="status"
            :aria-live="!isHidden && latestMessage ? 'polite' : 'off'"
            class="util-sr-only">
            {{ srStatus }}
        </div>
    </section>
</template>

<script setup>
import {
    computed,
    inject,
    nextTick,
    ref,
    watch
} from 'vue';

import TextContentBorder from './TextContentBorder.vue';
import { Keys }          from '@/core/di/keys.js';
import { useTimer }      from '@/composables/useTimer';

const { latestMessage } = inject(Keys.SnackBarStore);
const dismissedManually = ref(false);
let lastFocusedElement  = null;

const {
    isCompleted: isHidden,
    reset:       resetHiddenTimer,
    restart:     restartHiddenTimer,
    cancel:      cancelHiddenTimer
} = useTimer({
    duration:           7000,
    initiallyCompleted: true
});

const srStatus = computed(() => {
    return (
        !latestMessage.value
            ? 'Status messages will appear here'
            : !isHidden.value
                ? latestMessage.value.text
                : `Last status was: ${latestMessage.value.text}`);
});

watch(latestMessage, async newValue => {
    if (!newValue)
        return;

    resetHiddenTimer(false);
    dismissedManually.value = false;
    restartHiddenTimer();
}, {
    immediate: true
});

async function handleClick() {
    dismissedManually.value = true;
    await nextTick();
    cancelHiddenTimer();
    resetHiddenTimer(true);

    lastFocusedElement?.focus();
}

function handleFocus(e) {
    if (e.relatedTarget)
        lastFocusedElement = e.relatedTarget;
}
</script>

<style scoped>
.snack-bar-overlay {
    contain:        layout;
    container-name: snack-bar;
    container-type: inline-size;

    --container-height: 180px;

    display:        grid;
    place-content:  end center;
    position:       fixed;
    inset:          auto 0 0;
    pointer-events: none;

    height:         var(--container-height);
    padding-bottom: calc(var(--vp-margin) + var(--shadow-dist-l));

    & > .snack-bar-content {
        pointer-events: auto;
    }
}

.snack-bar-content {
    position:    relative;

    --transition-dur: 600ms;
    --translate-dist: var(--container-height);
    --fade-delay:     100ms;
    --easing-pop:     cubic-bezier(.58, -0.22, .69, 1.22);

    &.expand-enter-active,
    &.expand-leave-active {
        transition:
            translate var(--transition-dur) var(--easing-pop),
            opacity   var(--transition-dur) var(--fade-delay) ease;
    }

    &.expand-enter-from,
    &.expand-leave-to {
        translate: 0 var(--translate-dist);
        opacity:   0;
    }

    &[data-dismissed-manually=true] {
        --transition-dur: 200ms;
        --fade-delay:     0ms;
    }

    /*
        Work around bug in firefox where animations lag when box is
        rendered partially offscreen.
    */
    @supports (-moz-appearance: none) {
        --translate-dist: 0px;
        --fade-delay:     300ms;
    }
}

.close-button {
    --transition-dur: 50ms;
    --press-depth:    0px;

    position: absolute;
    inset:    -16px -16px auto auto;

    width:  1.4em;
    height: 1.4em;

    display:       grid;
    place-content: center;

    padding-bottom: 3px;

    -webkit-text-stroke: initial;
    font-size:           2.2rem;
    color:               black;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    text-shadow:
        -1.5px -1.5px var(--col-red-1),
         1.5px  1.5px var(--col-red-5);

    translate: 0 calc(var(--shadow-dist-s) - var(--shdw-dist-elevation));

    &:hover {
        filter: var(--filter-hl-1);
    }

    &:active {
        --shdw-dist-elevation: 0px;
    }
}
</style>