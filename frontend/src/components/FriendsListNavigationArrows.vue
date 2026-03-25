<template>
    <div class="friends-list-navigation-arrows">
        <div
            role="spinbutton"
            tabindex="0"
            class="
                number
                txtr-vert txtr-vert--gray
                shdw shdw--inst-gray shdw--elevated-s
                text-stroke--s"
            :aria-valuetext="`${currentValue + 1} of ${maxValue + 1}`"
            aria-valuemin="1"
            :aria-valuemax="maxValue + 1"
            :aria-valuenow="currentValue + 1"
            aria-label="Friends list page"
            @keydown.left="tryDecrement"
            @keydown.down="tryIncrement"
            @keydown.up="tryDecrement"
            @keydown.right="tryIncrement">
            {{ currentValue + 1 }} / {{ maxValue + 1 }}
        </div>

        <button
            :disabled="!canDecrement"
            class="arrow arrow--left"
            tabindex="-1"
            aria-hidden
            @click="tryDecrement">
            <IconNavigationArrow
                class="icon-navigation-arrow"
                variant="left" />
        </button>

        <button
            :disabled="!canIncrement"
            class="arrow arrow--right"
            tabindex="-1"
            aria-hidden
            @click="tryIncrement">
            <IconNavigationArrow
                class="icon-navigation-arrow"
                variant="right" />
        </button>
    </div>
</template>

<script setup>
import {
    computed,
    watch
} from 'vue';

import IconNavigationArrow from './IconNavigationArrow.vue';

const props = defineProps({
    maxValue: {
        type:     Number,
        required: true,
        validator(value) {
            return value >= 0;
        }
    }
});

const currentValue = defineModel('currentValue', {
    type:     Number,
    required: true,
    validator(value, props) {
        return value >= 0 && value <= props.maxValue;
    }
});

watch(() => props.maxValue, newValue => {
    if (currentValue.value > newValue)
        currentValue.value = newValue;
});

const canIncrement = computed(() => currentValue.value < props.maxValue);
const canDecrement = computed(() => currentValue.value > 0);

function tryIncrement() { currentValue.value = clamp(currentValue.value + 1, 0, props.maxValue); }
function tryDecrement() { currentValue.value = clamp(currentValue.value - 1, 0, props.maxValue); }

function clamp(v, vmin, vmax) {
    return Math.min(Math.max(v, vmin), vmax);
}
</script>

<style scoped>
.friends-list-navigation-arrows {
    grid-template:
        "arrow-left number arrow-right" 33px /
         50px       90px   50px;

    display: grid;
    gap: 2px;

    & > .arrow--left  { grid-area: arrow-left;  place-self: center; }
    & > .number       { grid-area: number;      place-self: stretch; }
    & > .arrow--right { grid-area: arrow-right; place-self: center; }
}

.number {
    display:       grid;
    place-content: center;
    white-space:   nowrap;
    user-select:   none;

    font-size: 2.2rem;

    border:        var(--border-s);
    border-radius: var(--radius-s);
}

.arrow {
    --elevation: var(--shadow-dist-s);
    --hl:        brightness(1);

    filter:
        drop-shadow(0 var(--elevation) black)
        var(--hl);

    &:hover {
        --hl: var(--filter-hl-1);
    }

    &:active {
        --elevation: 0px;
        translate:   0 var(--shadow-dist-s);
    }

    &:disabled {
        opacity: 0.5;
    }
}

.icon-navigation-arrow {
    height:    30px;
    translate: 0 2px;
}
</style>