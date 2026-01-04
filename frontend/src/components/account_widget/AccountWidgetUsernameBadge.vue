<template>
    <div class="active-user-strip">
        <i class="user-icon hn hn-user-solid"></i>

        <div class="username-section">
            <div class="username">
                <div class="username-editor" v-show="isEditing || isLoading">
                    <div class="at-symbol"
                        :style="{
                            'visibility': isLoading ? 'hidden' : 'visible'
                        }">@</div>

                    <UsernameInput
                        ref="inputEl"
                        v-model="inputText"
                        class="username-text-input"
                        @blur="editorLostFocus"
                        @submit="submit"
                        @validityChanged="v => inputIsValid = v"
                        :forbiddenUsernames="forbiddenUsernames.concat([username])" />

                    <button
                        v-show="isEditing || isLoading"
                        class="submit-button"
                        :class="{
                            'invalid': !inputIsValid
                        }"
                        ref="submitButtonEl"
                        @click="submit"
                        @blur="editorLostFocus">
                        <LoadingSpinner v-if="isLoading" />
                        <i v-else class="submit-icon hn hn-check-solid"></i>
                    </button>
                </div>

                <button
                    class="username-button"
                    :class="{ 'loading': isLoading }"
                    @click="emit('requestEdit')"
                    :disabled="isLoading || isEditing"
                    @mousemove.self="teleportPencilToMouse"
                    @mouseenter="teleportPencilToMouse"
                    v-show="!isEditing || isLoading">
                    <div class="username-text">
                        <span v-if="!isLoading">
                            @{{ username }}
                        </span>
                        <span v-else>
                            @{{ inputText }}
                        </span>
                    </div>

                    <div
                        class="pencil-icon-container"
                        v-show="!isLoading"
                        :style="{
                            left:   `${pencilPosition.x}px`,
                            top:    `${pencilPosition.y}px`,
                            width:  `${pencilDimensions.x}px`,
                            height: `${pencilDimensions.y}px`
                        }">
                        <i class="pencil-icon-stroke hn hn-pencil-solid"></i>
                        <i class="pencil-icon hn hn-pencil-solid"></i>
                    </div>
                </button>
            </div>
        </div>
    </div>
</template>

<script setup>
import {
    toRefs, ref, useTemplateRef,
    nextTick, watch, computed } from 'vue';
import UsernameInput from '@/components/username_input/UsernameInput.vue';
import LoadingSpinner from '@/components/loading_spinner/LoadingSpinner.vue';

const propsObj = defineProps({
    username: { type: String, required: true },
    forbiddenUsernames: { type: Array, default: () => [] },

    // 'normal' | 'editing' | 'loading'
    status: { type: String, required: true }
});

const props = toRefs(propsObj);

const emit = defineEmits([
    'requestEdit',
    'submit',
    'cancel'
]);

const submitButtonEl = useTemplateRef('submitButtonEl');
const inputEl        = useTemplateRef('inputEl');

const inputIsValid = ref(false);

const isEditing = computed(() => props.status.value === 'editing');
const isLoading = computed(() => props.status.value === 'loading');

const pencilPosition = ref({ x: 0, y: 0 });
const inputText = ref(props.username.value);

const pencilDimensions = ref({ w: 28, h: 40 });

function teleportPencilToMouse(e) {
    pencilPosition.value.x = e.offsetX - pencilDimensions.value.w / 2;
    pencilPosition.value.y = e.offsetY - pencilDimensions.value.h / 2;
}

function editorLostFocus(e) {
    if (e.relatedTarget != submitButtonEl.value &&
        e.relatedTarget != inputEl.value &&
        isEditing.value) {
        inputText.value = props.username.value;
        emit('cancel');
    }
}

function submit() {
    emit('submit', inputText.value);
}

watch (isEditing, () => {
    if (isEditing.value)
        nextTick(() => inputEl.value?.focus());
});
</script>

<style scoped>
.active-user-strip {
    color: var(--text);

    display: flex;
    flex-flow: row nowrap;
    align-items: center;
    gap: 12px;

    font-family: var(--font-heading);
    font-size: 2.4rem;
    padding: 9px 9px 9px 16px;
    letter-spacing: 0.06em;

    background: var(--bg);
    border: var(--border-small);
    border-radius: var(--border-radius-small);
    box-shadow: var(--border-shadow-small);
    transition: transform 100ms linear;

    border-radius: 1000px;
    pointer-events: all;
    font-size: 26px;
    line-height: 1;
}

.user-icon {
    font-size: 26px;
    line-height: 1;

    color: transparent;
    background: black;
    background-clip: text;
}

.username {
    position: relative;
    display: grid;
    justify-items: start;
}

.username-button, .username-editor {
    grid-area: 1/1;
}

.username-button {
    cursor: none;
    padding-right: 16px;
}

.username-button.loading {
    cursor: default;
}

.username-button > * {
    pointer-events: none;
}

.username-text {
    --red:    #EA4335;
    --blue:   #4285F4;
    --green:  #34A853;
    --yellow: #FBBC05;

    --rotate-gradient: 0deg;

    animation: rotate-gradient 2s linear infinite;
    background-image: conic-gradient(in oklch from var(--rotate-gradient) at 50% 100%,
        var(--red)    0%,
        var(--blue)   25%,
        var(--green)  50%,
        var(--yellow) 75%,
        var(--red)    100%);
    color: transparent;
    background-clip: text;
}

.username-editor {
    display: flex;
    flex-flow: row nowrap;
}

.at-symbol {
    margin-right: -1.5px;
    cursor: default;
}

.username-text-input {
    box-sizing: border-box;
    font-family: inherit;
    padding: 0;
    margin: 0;
    border: 0;
    font-size: inherit;
    appearance: none;
    -webkit-appearance: none;
    line-height: 10px;
    outline: none !important;
    width: 200px;
    height: 1em;
    background: transparent;
    color: black;
}

.submit-button {
    font-size: 2.4rem;
    padding: 2px 30px;
    margin: -6px;
    background: hsl(113, 68%, 58%);
    border: var(--border-small);
    border: 2.5px solid black;
    box-shadow: 1px 1px 0 0 black;
    width: 40px;
    border-radius: 999px;
    display: flex;
    justify-content: center;
    align-items: center;

    transition: transform 200ms ease;
    transform: scale(1) rotate(0deg);
}

.submit-button:hover:not(.invalid) {
    transform: scale(1.02) rotate(-1deg);
    box-shadow: 2px 2px 0 0 black;
}

.submit-button:active:not(.invalid) {
    background: hsl(113, 56%, 53%);
    transform: scale(0.98) rotate(0deg);
    box-shadow: 1px 1px 0 0 black;
}

.submit-button.invalid {
    opacity: 0.5;
    cursor: not-allowed;
}

.submit-icon {
    font-size: 2.3rem;
    position: relative;
    top: 1px;
    left: 1px;
}

.loading-icon {
    animation: rotate-loading 600ms infinite steps(8, end);
}

.pencil-icon-container {
    animation: bob-pencil 2s infinite;

    display: grid;
    place-items: center;
    position: absolute;
    left: 0;
    top: 0;
}

.pencil-icon {
    --pencil-tip:     black;
    --pencil-wood:    #f2d198;
    --pencil-wrapper: #ff9c08;
    --pencil-eraser:  #ff899a;

    background: linear-gradient(42deg,
        var(--pencil-tip)     20%,
        var(--pencil-wood)    20% 34%,
        var(--pencil-wrapper) 34% 63%,
        var(--pencil-eraser)  63%);
    background-clip: text;
    color: transparent;
    line-height: 1.5;
    grid-area: 1/1;
}

.pencil-icon-stroke {
    -webkit-text-stroke: 4px white;
    line-height: 1.5;
    grid-area: 1/1;
}

.username-section:not(:hover) .pencil-icon-container {
    display: none;
}

.username-section:hover .username-text {
    text-decoration: underline;
    text-decoration-thickness: 3px;
}

@property --rotate-gradient {
    syntax: '<angle>';
    initial-value: 0deg;
    inherits: false;
}

@keyframes rotate-gradient {
    from { --rotate-gradient: 0deg; }
    to   { --rotate-gradient: 360deg; }
}

@keyframes bob-pencil {
    0%   { transform: rotate(-6deg) scale(1.04); }
    25%  { transform: rotate(6deg) scale(0.99);  }
    50%  { transform: rotate(-6deg) scale(1.04); }
    75%  { transform: rotate(6deg) scale(0.99);  }
    100% { transform: rotate(-6deg) scale(1.04); }
}

@keyframes rotate-loading {
    from { transform: rotate(0deg); }
    to { transform: rotate(360deg); }
}
</style>