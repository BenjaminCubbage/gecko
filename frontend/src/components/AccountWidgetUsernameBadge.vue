<template>
    <div class="account-widget-username-badge">
        <div
            class="
                username
                shdw shdw--inst-gray shdw--elevated-s
                txtr-vert txtr-vert--gray"
            :data-pressed="isInputActive">
            <div class="username-editor" v-show="isInputActive">
                <div class="at-symbol">@</div>

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
                    @blur="blur"
                    @keydown.enter="submit"
                    @keydown.esc="cancel" />

                <button
                    ref="submitButtonEl"
                    class="
                        submit-button
                        shdw shdw--inst-green
                        shdw-filter shdw-filter--xs shdw-filter--green"
                    :data-pressed="isLoading"
                    :disabled="!isValidInput || isLoading"
                    aria-label="Submit username"
                    @blur="blur"
                    @click="submit">
                    <LoadingSpinner v-if="isSpinning" class="loading-spinner" />
                    <i v-else class="submit-icon hn hn-check-solid"></i>
                </button>
            </div>

            <button
                v-show="!isInputActive"
                ref="editButtonEl"
                class="username-button"
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

import BaseInput      from './BaseInput.vue';
import LoadingSpinner from './LoadingSpinner.vue';

import { useAutoHighlightTextInput } from '@/composables/useAutoHighlightTextInput.js';
import { useLoadingState }           from '@/composables/useLoadingState.js';

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

useAutoHighlightTextInput(() => inputEl.value?.innerElement);

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
    height:      calc(var(--user-button-ht) - 7px);
    position:    relative;

    font-size:      2.6rem;
    letter-spacing: 0.06em;
    line-height:    1;

    max-width: 250px;
        
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
    display:           grid;
    grid-auto-columns: minmax(0, 1fr);
    grid-auto-rows:    minmax(0, 1fr);
    justify-items:     start;
    justify-self:      stretch;

    border-radius: var(--radius-s);
    border:        var(--border-s);

    translate: 
        0 
        calc(var(--shadow-dist-s) - var(--shdw-dist-elevation));

    @media (hover: hover) {
        &:has(> .username-button:hover) {
            filter: var(--filter-hl-1);
        }
    }

    &[data-pressed=true],
    &[data-pressed=true]:has(> .username-button:hover) {
        --shdw-dist-elevation: 0px;
        --shdw-etc: var(--shadow-aura);
    }

    & > .username-button,
    & > .username-editor {
        grid-area: 1 / 1;
    }
}

.username-button {
    padding-right: 16px;
    position:      relative;
    width:         100%;
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
}

.at-symbol {
    align-self:          center;
    cursor:              default;
    margin-right:        -3px;

    -webkit-text-stroke: var(--text-stroke-s);
    color:               black;
}

.base-input {
    margin:      0;
    padding:     0 4px 0 2.5px;
    width:       100%;
    text-indent: var(--at-gap);
}

.submit-button {
    align-self:      stretch;
    display:         flex;
    justify-content: center;
    place-items:     center;
    width:           60px;

    color:     var(--col-green-9);
    font-size: 2.4rem;

    background:
        linear-gradient(
            var(--col-green-2) 50%,
            var(--col-green-4) 50%);

    border-left: var(--border-s);

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

    &:active,
    &[data-pressed=true] {
        --shdw-dist-mult: -1;
    }

    &:disabled > .submit-icon {
        opacity: 0.3;
    }

    & > .submit-icon {
        font-size: 1.8rem;
        translate: 1px 1.5px;
    }

    & > .loading-spinner {
        font-size: 3.2rem;
    }
}
</style>