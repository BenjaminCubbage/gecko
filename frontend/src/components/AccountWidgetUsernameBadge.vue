<template>
    <div class="account-widget-username-badge">
        <div
            class="username"
            :class="{ 'username--pressed': isEditing || isLoading }">
            <div class="username-editor" v-show="isEditing || isLoading">
                <div class="at-symbol">@</div>

                <BaseInput
                    v-model="inputText"
                    ref="inputEl"
                    class="username-text-input"
                    :disabled="isLoading"
                    :maxlength="maxUsernameLength"
                    :charPredicate="isValidUsernameChar"
                    @blur="blur"
                    @keydown.enter="submit" />

                <button
                    class="submit-button"
                    :class="{ 'submit-button--pressed': isLoading }"
                    ref="submitButtonEl"
                    @blur="blur"
                    :disabled="!isValidInput || isLoading"
                    @click="submit">
                    <LoadingSpinner
                        v-if="isSpinning"
                        style="position: relative; top: 2px" />
                    <i v-else class="submit-icon hn hn-check-solid"></i>
                </button>
            </div>

            <button
                v-show="!isEditing && !isLoading"
                class="username-button"
                :disabled="isLoading"
                @click="edit">
                <div class="username-text">
                    <template v-if="!isLoading">
                        @{{ session.activeUser.value.username }}
                    </template>
                    <template v-else>
                        @{{ inputText }}
                    </template>
                </div>
            </button>
        </div>

        <div v-show="isEditing || isLoading" class="error-message">
            {{ errorMessage }}
        </div>
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

const session = inject(Keys.SessionStore);

const isEditing = ref(false);
const inputText = ref('');
const errorMessage = ref('');

const submitButtonEl = useTemplateRef('submitButtonEl');
const inputEl        = useTemplateRef('inputEl');

useAutoHighlightTextInput(() => inputEl.value?.innerElement);

const {
    isLoading,
    isSpinning,
    startedLoading,
    stoppedLoading
} = useLoadingState();

const isValidInput = computed(() => {
    return inputText.value !== session.activeUser.value.username
        && isValidUsername(inputText.value);
});

watch(() => session.activeUser.value.username, newValue => {
    if (!isEditing.value)
        inputText.value = newValue;
}, {
    immediate: true
});

watch(() => isEditing.value || isLoading.value, newValue => {
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
    inputEl.value.innerElement.focus();
}

function blur(e) {
    if (e.relatedTarget != submitButtonEl.value &&
        e.relatedTarget != inputEl?.value.innerElement) {
        isEditing.value = false;
    }
}

async function submit() {
    if (!isValidInput.value)
        return;

    isEditing.value = false;
    startedLoading();

    try {
        await session.requestChangeUsername(inputText.value);
    } catch (e) {
        if (e instanceof HttpError && e.body.error.reason === 'username_taken')
            errorMessage.value = 'Username is taken';
        else if (e instanceof NetworkError)
            errorMessage.value = `Couldn't connect`;
        else
            errorMessage.value = `Error updating username`;

        edit();
    }
    finally {
        stoppedLoading();
    }
}
</script>

<style scoped>
.account-widget-username-badge {
    align-items: stretch;
    display:     flex;
    position:    relative;

    color:          var(--text);
    font-family:    var(--font-heading);
    font-size:      2.6rem;
    letter-spacing: 0.06em;
    line-height:    1;
}

.error-message {
    bottom:         -24px;
    left:           12px;
    pointer-events: none;
    position:       absolute;

    font-size:           1.8rem;
    color:               var(--col-red-5);
    -webkit-text-stroke: var(--text-stroke-s);

    paint-order: stroke;
}

.username {
    display:       grid;
    justify-items: start;
    overflow:      hidden;

    --username-aura:   0 0;
    --username-offset: 0px;

    background:
        linear-gradient(
            var(--col-gray-2) 50%,
            var(--col-gray-3) 50%);

    box-shadow:
        var(--username-aura),
        calc(var(--shadow-dist-s) - var(--username-offset))
        calc(var(--shadow-dist-s) - var(--username-offset))
        0 black,
        inset 0  3px 0 var(--col-gray-0),
        inset 0 -3px 0 var(--col-gray-4);

    border:        var(--border-s);
    border-radius: var(--radius-s);

    transform:
        translate(
            var(--username-offset),
            var(--username-offset));

    transition:
        box-shadow 50ms ease,
        transform  50ms ease;
}

.username:has(> .username-button:hover) {
    --username-offset: calc(var(--shadow-dist-s) / 2);
}

.username--pressed {
    width: clamp(0px, 300px, 100%);
}

.username:has(> .username-button:active),
.username--pressed,
.username--pressed:has(> .username-button:hover) {
    --username-aura:   var(--shadow-aura);
    --username-offset: var(--shadow-dist-s);
}

.username-button,
.username-editor {
    grid-area: 1 / 1;
    width:     100%;
}

.username-button {
    cursor:   text;
    padding:  0 16px;
    position: relative;
    overflow: hidden;
}

.username-button:disabled {
    cursor: default;
}

.username-button > * {
    pointer-events: none;
}

.username-text {
    -webkit-text-stroke: var(--text-stroke-s);
    color:               black;
    letter-spacing:      0.03em;
    paint-order:         stroke;
    overflow:            hidden;
    white-space:         nowrap;
    text-overflow:       ellipsis;
    width:               100%;
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
    paint-order:         stroke;
}

.username-text-input {
    margin:        0;
    padding-right: 16px;
    width:         100%;

    -webkit-appearance:  none;
    -webkit-text-stroke: var(--text-stroke-s);
    appearance:          none;
    box-sizing:          border-box;

    background:     transparent;
    border:         0;
    color:          black;
    font-family:    inherit;
    font-size:      inherit;
    letter-spacing: 0.03em;
    outline:        none;

    paint-order: stroke;
}

.submit-button {
    align-self:      stretch;
    display:         flex;
    justify-content: center;
    padding:         0 13px;
    place-items:     center;
    width:           55px;

    color: var(--col-green-9);
    font-size: 2.4rem;

    background:
        linear-gradient(
            var(--col-green-2) 50%,
            var(--col-green-4) 50%);

    box-shadow:
        inset 0  3px 0 var(--col-green-0),
        inset 0 -3px 0 var(--col-green-5);
            
    border-left: var(--border-s);

    paint-order: stroke;
}

.submit-button:active,
.submit-button--pressed {
    background: linear-gradient(
        var(--col-green-2) 56%,
        var(--col-green-4) 56%);

    box-shadow: inset 0 2px 0 black;
}

.submit-button:active .submit-icon {
    transform: translateY(2px);
}

.submit-button:disabled {
    pointer-events: none;
}

.submit-button:disabled .submit-icon {
    opacity: 0.3;
}

.submit-icon {
    left:      1px;
    top:       1px;
    position:  relative;
}

.submit-button--pressed .submit-icon {
    transform: translateY(2px);
}
</style>