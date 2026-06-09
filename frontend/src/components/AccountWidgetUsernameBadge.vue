<template>
    <div class="account-widget-username-badge">
        <div
            class="username"
            :data-pressed="isInputActive">
            <div
                v-show="isInputActive"
                class="
                    username-editor
                    shdw shdw--inst-gray shdw--elevated-s
                    txtr-vert txtr-vert--gray">
                <span class="at-symbol">@</span>

                <BaseInput
                    v-model="inputText"
                    ref="inputEl"
                    class="base-input"
                    variant="no-box"
                    :temporarily-disabled="isLoading"
                    :maxlength="maxUsernameLength"
                    :char-predicate="isValidUsernameChar"
                    name="username"
                    autocomplete="off"
                    v-auto-highlight
                    @blur="blur"
                    @keydown.enter="submit"
                    @keydown.esc="cancel" />
            </div>

            <button
                v-show="isInputActive"
                ref="submitButtonEl"
                class="
                    submit-button
                    shdw shdw--inst-green shdw--elevated-s"
                :data-pressed="isLoading"
                :disabled="!isValidInput || isLoading"
                :data-is-spinning="isSpinning"
                aria-label="Submit username"
                @blur="blur"
                @click="submit">
            </button>

            <button
                v-show="!isInputActive"
                ref="editButtonEl"
                class="
                    username-button
                    shdw shdw--inst-gray shdw--elevated-s
                    txtr-vert txtr-vert--gray"
                :aria-label="`Edit username ${session.activeUser.value.username}`"
                :disabled="isLoading"
                @click="edit">
                <div class="username-text">
                    <template v-if="!isLoading">
                        {{ session.activeUser.value.username }}
                    </template>
                    <template v-else>
                        {{ inputText }}
                    </template>
                </div>
            </button>
        </div>

        <span
            role="status"
            v-show="isInputActive"
            class="error-message">
            {{ errorMessage }}
        </span>
    </div>
</template>

<script setup>
import {
    computed,
    inject,
    nextTick,
    ref,
    useTemplateRef,
    watch
} from 'vue';

import BaseInput from './BaseInput.vue';

import { useLoadingState } from '@/composables/useLoadingState.js';

import {
    HttpError,
    NetworkError
} from '@/core/errors/errors.js';

import {
    isValidUsername,
    isValidUsernameChar,
    maxUsernameLength
} from '@/core/validation/validateUsername.js';

import { Keys } from '@/core/di/keys.js';

const session  = inject(Keys.SessionStore);
const snackBar = inject(Keys.SnackBarStore);

const isEditing = ref(false);
const inputText = ref('');
const errorMessage = ref('');

const submitButtonEl = useTemplateRef('submitButtonEl');
const inputEl        = useTemplateRef('inputEl');
const editButtonEl   = useTemplateRef('editButtonEl');

const {
    isLoading,
    isSpinning,
    startedLoading,
    stoppedLoading
} = useLoadingState();

const isInputActive = computed(() => {
    return isEditing.value || isLoading.value;
});

const isValidInput = computed(() => {
    return (
        inputText.value !== session.activeUser.value.username &&
        isValidUsername(inputText.value));
});

watch(() => session.activeUser.value.username, newValue => {
    if (!isEditing.value)
        inputText.value = newValue;
}, {
    immediate: true
});

watch(isInputActive, newValue => {
    if (!newValue)
        inputText.value = session.activeUser.value.username;
});

watch(isEditing, newValue => {
    if (!newValue)
        errorMessage.value = '';
});

async function edit() {
    isEditing.value = true;
    await nextTick();
    inputEl.value?.innerElement?.focus();
}

function cancel() {
    isEditing.value = false;
}

async function blur(e) {
    if (e.target === inputEl.value?.innerElement &&
        !isInputActive.value) {
        editButtonEl.value?.focus({
            focusVisible: false
        });
    }

    if (e.relatedTarget !== submitButtonEl.value &&
        e.relatedTarget !== inputEl.value?.innerElement) {
        cancel();
    }
}

async function submit() {
    if (!isValidInput.value)
        return;

    isEditing.value = false;
    inputEl.value?.innerElement?.focus();

    startedLoading();

    try {
        await session.requestChangeUsername(inputText.value);
        snackBar.pushMessage('Updated username successfully');
    } catch (e) {
        if (e instanceof HttpError && e.body.error.reason === 'username_taken')
            errorMessage.value = 'Username is taken';
        else if (e instanceof NetworkError)
            errorMessage.value = `Couldn't connect`;
        else
            errorMessage.value = `Error updating username`;

        edit();
    } finally {
        stoppedLoading();
    }
}
</script>

<style scoped>
.account-widget-username-badge {
    /*
        Space between @ symbol and first username character
    */
    --at-gap: 2px;

    align-items: stretch;
    display:     grid;
    height:      calc(var(--user-button-ht) - 8px);
    position:    relative;

    font-size:      2.6rem;
    letter-spacing: 0.06em;
    line-height:    1;

    max-width: 240px;

    filter:
        drop-shadow(
            var(--shadow-dist-m)
            var(--shadow-dist-m)
            var(--col-shadow-alpha));
}

.error-message {
    bottom:         -24px;
    left:           12px;
    pointer-events: none;
    position:       absolute;

    font-size:           1.8rem;
    color:               var(--col-red-6);
    -webkit-text-stroke: var(--text-stroke-s);
}

.username {
    display: flex;

    filter:
        var(--_fx-aura,)
        var(--_fx-hl,);

    @media (hover: hover) {
        &:has(> .username-button:hover) {
            --_fx-hl: var(--filter-hl-1);
        }
    }

    &[data-pressed=true] {
        --_fx-hl:   var(--filter-hl-1);
        --_fx-aura: var(--filter-aura-s);
    }

    > .username-button,
    > .username-editor {
        grid-area: 1 / 1;
    }
}

.username-button {
    padding-right: 16px;
    position:      relative;
    width:         100%;

    border-radius: var(--radius-s);
    border:        var(--border-s);
}

.username-text {
    overflow:     hidden;
    padding-left: 16px;

    -webkit-text-stroke: var(--text-stroke-s);
    color:               black;
    letter-spacing:      0.03em;
    text-overflow:       ellipsis;
    white-space:         nowrap;

    &::before {
        content:           '@';
        display:           inline-block;
        margin-inline-end: var(--at-gap);
    }
}

.username-editor {
    align-self:   stretch;
    display:      flex;
    flex-flow:    row nowrap;
    padding-left: 16px;

    border: var(--border-s);

    border-radius:
        var(--radius-s) 0 0
        var(--radius-s);
}

.at-symbol {
    align-self:   center;
    cursor:       default;
    margin-right: -3px;

    -webkit-text-stroke: var(--text-stroke-s);
    color:               black;
}

.base-input {
    margin-right: 3px;
    width:        100%;
    text-indent:  calc(var(--at-gap) + 2.2px);
}

.submit-button {
    align-self:      stretch;
    display:         flex;
    justify-content: center;
    place-items:     center;
    width:           60px;

    font: 3rem var(--font-main);

    background:
        linear-gradient(
            var(--col-green-2) 50%,
            var(--col-green-4) 50%);

    border:      var(--border-s);
    border-left: 0;

    border-radius:
        0
        var(--radius-s)
        var(--radius-s)
        0;

    @supports not (corner-shape: notch) {
        border-radius:
            0
            calc(var(--radius-s) - 1px)
            calc(var(--radius-s) - 1px)
            0;
    }

    @media (hover: hover) {
        &:hover,
        &:active,
        &[data-pressed=true] {
            filter: var(--filter-hl-0);
        }
    }

    &:active,
    &[data-pressed=true] {
        translate: 0 var(--shadow-dist-s);
        --shdw-dist-elevation: 0;
    }

    &:disabled:not([data-is-spinning=true])::after {
        opacity: 0.3;
    }

    &[data-is-spinning=true]::after {
        animation: username-submit-loading 300ms steps(1) infinite;
    }

    &::after {
        content:   '\F514';
        translate: 1px 0;

        text-shadow:
            calc(-1 * var(--shadow-dist-xs)) calc(-1 * var(--shadow-dist-xs)) var(--col-green-1),
                        var(--shadow-dist-xs)            var(--shadow-dist-xs)  var(--col-green-5);
    }
}

@keyframes username-submit-loading {
    0%  { content: '\F510'; }
    33% { content: '\F511'; }
    66% { content: '\F512'; }
}
</style>