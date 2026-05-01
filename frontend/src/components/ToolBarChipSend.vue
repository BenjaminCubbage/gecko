<template>
    <ToolBarChip
        ref="toolBarChipEl"
        color="green"
        class="tool-bar-chip-send"
        :class="`tool-bar-chip-send--size-${size}`"
        @click="click">
        <template v-if="size === 'normal'" #label>
            <span class="letter letter--1">S</span>
            <span class="letter letter--2">E</span>
            <span class="letter letter--3">N</span>
            <span class="letter letter--4">D</span>
        </template>

        <template #icon>
            <IconRocket class="icon" />
        </template>
    </ToolBarChip>
</template>

<script setup>
import {
    computed,
    onUnmounted,
    ref,
    useTemplateRef
} from 'vue';

import ToolBarChip from './ToolBarChip.vue';
import IconRocket  from './IconRocket.vue';

defineProps({
    size: {
        type:    String,
        required: true,
        validator(value) {
            return [
                'normal',
                'small'
            ].includes(value);
        }
    },

    disabled: {
        type:    Boolean,
        default: false
    }
});

const emit = defineEmits([
    'click'
]);

const isLaunching = ref(false);
let launchTimeout = null;

const toolBarChipEl = useTemplateRef('toolBarChipEl');

const srStatus = computed(() => {
    return isLaunching.value
        ? 'Sending image'
        : null;
});

function click() {
    isLaunching.value = true;
    launchTimeout = setTimeout(() => {
        isLaunching.value = false;
    }, 5700);
    emit('click');
}

function animationCancelled() {
    isLaunching.value = false;
    clearTimeout(launchTimeout);
}

onUnmounted(() => {
    clearTimeout(launchTimeout);
});

defineExpose({
    innerElement: computed(() => toolBarChipEl.value?.innerElement)
});
</script>

<style scoped>
.icon {
    height: 46.5px;
}

.tool-bar-chip-send {
    &.tool-bar-chip-send--size-normal {
        gap: 6px;
    }

    /*
        Animations
    */

    &[data-pressed=true] .letter {
        animation-name:     letter-jump;
        animation-duration: 400ms;
    }

    &[data-pressed=true] {
        .rocket-svgs {
            animation:
                rocket-bob    200ms  steps(2)                    infinite 0ms,
                fly-away      5000ms cubic-bezier(.68,.01,.62,1) 1        500ms,
                rocket-return 200ms  ease                        1        5500ms;

            pointer-events: none;
        }

        .rocket-svg-flame {
            animation: flame-bob 200ms steps(5) 24;
        }
    }

    /*
        Texture promotion
    */

    &:hover,
    &[data-pressed=true] {
        .rocket-svgs,
        .rocket-svg-rocket,
        .rocket-svg-flame,
        .pad,
        .letter {
            will-change: transform;
        }

        .rocket-svg-flame {
            /* Disallow culling */
            opacity: 0.001;
        }
    }
}

.rocket-svgs {
    display:        grid;
    isolation:      isolate;
    opacity:        var(--rocket-opacity);
    pointer-events: none;

    width: 30px;
    translate: 0 0.2px;

    & > * {
        grid-area: 1 / 1;
    }

    & > .rocket-svg-rocket {
        z-index: 1;
        scale:   1.45;
    }

    & > .rocket-svg-flame {
        opacity:   0;
        z-index:   0;
        translate: 0 24px;
    }

    .stroke {
        stroke-width: 62px;
        stroke:       black;
    }

    & > svg > .st0{ fill:#FF0600; }
    & > svg > .st1{ fill:#FFFFFF; }
    & > svg > .st2{ fill:#ff7b00; }
}

.letter {
    display:   inline-block;
    transform: translateY(calc(var(--letter-jump-arc) * -1));
    color:     var(--col-green-6);

    @supports (order: sibling-index()) {
        animation-delay: calc(sibling-index() * 25ms);
    }

    @supports not (order: sibling-index()) {
        &.letter--1 { animation-delay: calc(1 * 25ms); }
        &.letter--2 { animation-delay: calc(2 * 25ms); }
        &.letter--3 { animation-delay: calc(3 * 25ms); }
        &.letter--4 { animation-delay: calc(4 * 25ms); }
    }
}
</style>

<style>
@property --letter-jump-arc {
    inherits:      false;
    initial-value: 0;
    syntax:        "<length>";
}

@keyframes letter-jump {
    0%   { --letter-jump-arc:  0px; }
    50%  { --letter-jump-arc:  3px; }
    85%  { --letter-jump-arc: -1px; }
    100% { --letter-jump-arc:  0px; }
}

@keyframes flame-bob {
    0%   { rotate:  5deg; opacity: 100%; }
    50%  { rotate: -5deg; }
    100% { rotate:  5deg; opacity: 100%; }
}

@keyframes rocket-bob {
    0%   { rotate:  1.5deg; }
    50%  { rotate: -1.5deg; }
    100% { rotate:  1.5deg; }
}

@keyframes fly-away {
    0% {
        opacity:   100%;
        translate: 0 0;
    }

    94% {
        opacity:   100%;
        translate: 0 -800px;
    }

    95%  { opacity: 0%; }
    100% { opacity: 0%; }
}

@keyframes rocket-return {
    0% {
        opacity: 0%;
        translate: 0 2px;
    }

    100% {
        opacity:   100%;
        translate: 0 0;
    }
}
</style>