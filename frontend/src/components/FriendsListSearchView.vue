<template>
    <div class="friends-list-search-view">
        <div class="view-arrows">
            <span class="arrow">&#x25B2;</span>
            <span class="arrow">&#x25B2;</span>
            <span class="arrow">&#x25B2;</span>
        </div>

        <div
            class="
                view-bar
                shdw shdw--otst-orange">
            <div
                class="bar-field
                    txtr-vert txtr-vert--lt-gray
                    shdw shdw--inst-lt-gray shdw--elevated-s">
                <BaseInput
                    class="field-input"
                    v-model="searchInput"
                    :maxlength="maxUsernameLength"
                    :char-predicate="isValidUsernameChar"
                    variant="no-box"
                    placeholder="username"
                    @keydown.enter="trySubmit" />
            </div>

            <button
                class="
                    bar-btn
                    shdw shdw--inst-green shdw--elevated-s
                    txtr-vert txtr-vert--green"
                :data-is-loading="isLoading"
                :disabled="!isValidInput"
                @click="trySubmit"
                type="button">
            </button>
        </div>
    </div>
</template>

<script setup>
import {
    computed,
    ref,
    toRef,
    watch
} from 'vue';

import IconFriendListArrow from './IconFriendListArrow.vue';
import BaseInput           from './BaseInput.vue';

import { useThrottledRef } from '@/composables/useThrottledRef';

import {
    isValidUsername,
    isValidUsernameChar,
    maxUsernameLength
} from '@/core/validation/validateUsername.js';

const props = defineProps({
    isLoading: {
        type:     Boolean,
        required: true
    }
});

const emit = defineEmits([
    'searchSubmitted'
]);

const searchInput = defineModel('searchInput', {
    type:     String,
    required: true
});

const isValidInput = computed(() => isValidUsername(searchInput.value));

function trySubmit() {
    if (isValidInput.value && !props.isLoading)
        emit('searchSubmitted');
}
</script>

<style scoped>
.friends-list-search-view {
    --ht-bar: 37px;
    --wd-btn: 48px;

    display:   flex;
    flex-flow: column;
    gap:       6px;

    & > *            { align-self: stretch; }
    & > .view-arrows { align-self: center; }
}

.view-arrows {
    display: flex;
    gap:     6px;

    margin: 5px 0 2px;

    font-size:    2rem;
    color:        var(--col-orange-8);
    word-spacing: 0em;

    padding-inline-start: 0.1em;
}

.view-bar {
    position: relative;
    display:  grid;

    grid-template:
        'at   input btn' var(--ht-bar) /
         auto 1fr   var(--wd-btn);

    border-radius: var(--radius-s);

    & > .bar-field { z-index: 1; grid-area: input; }
    & > .bar-btn   { z-index: 0; grid-area: btn; }
}

.bar-field {
    display: flex;

    padding: 0 9px;

    border: var(--border-s);
    border-radius:
        var(--radius-s)
        0
        0
        var(--radius-s);

    -webkit-text-stroke: var(--text-stroke-s);
    font-family:         var(--font-scnd);
    font-size:           1.7rem;
    font-weight:         bold;
    line-height:         1.2;

    translate:
        0 calc(-1 * var(--shdw-dist-elevation));

    --shdw-etc:
        calc(-1 * var(--shadow-dist-m))
        calc(-1 * var(--shadow-dist-m))
        var(--col-orange-0);

    &:focus-visible {
        outline: none;
    }

    &::before {
        content: '@';

        display:       grid;
        place-content: center;
    }
}

.field-input {
    /*
        Text indent (not padding) because otherwise text stroke
        gets clipped
    */
    text-indent: 3px;
}

.bar-btn {
    border: var(--border-s);
    border-radius:
        0
        var(--radius-s)
        var(--radius-s)
        0;

    font-family: var(--font-main);

    font-size: 3.3rem;
    line-height: 1;

    display:       grid;
    place-content: center;

    padding-bottom: 0.03em;

    text-shadow:
        calc(-1 * var(--shadow-dist-xs)) calc(-1 * var(--shadow-dist-xs)) var(--col-green-1),
                  var(--shadow-dist-xs)            var(--shadow-dist-xs)  var(--col-green-5);

    margin-left:
        calc(-1 * var(--border-thickness-s));

    translate:
        0 calc(-1 * var(--shdw-dist-elevation));

    --shdw-etc: -3px -3px var(--col-orange-0);

    &[data-is-loading=false] {
        &::after          { content: '\F50D'; }
        &:disabled::after { opacity: 30%; }
    }

    &[data-is-loading=true] {
        pointer-events: none;

        &::after {
            content:   '\F510';
            animation: friends-search-loading 300ms steps(1) infinite;
        }
    }

    @media (hover: hover) {
        &:hover,
        &:active,
        &[data-is-loading=true] {
            filter: var(--filter-hl-1);
        }
    }

    &:active,
    &[data-is-loading=true] {
        --shdw-dist-elevation: 0px;
    }
}

@keyframes friends-search-loading {
    0%  { content: '\F510'; }
    33% { content: '\F511'; }
    66% { content: '\F512'; }
}
</style>