<template>
    <div class="account-widget-username-badge">
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
    computed,
    nextTick,
    ref,
    toRefs,
    useTemplateRef,
    watch
} from 'vue';

import UsernameInput  from './UsernameInput.vue';
import LoadingSpinner from './LoadingSpinner.vue';

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
.account-widget-username-badge {
    display:     flex;
    flex-flow:   row nowrap;
    align-items: center;
    gap:         12px;
    padding:     9px 9px 9px 16px;

    color:          var(--text);
    font-family:    var(--font-heading);
    font-size:      2.6rem;
    letter-spacing: 0.06em;
    line-height:    1;

    background:    white;
    border:        var(--border-s);
    border-radius: 1000px;
    box-shadow:    var(--shadow-s);

    transition: transform 100ms linear;
}

.user-icon {
    font-size:   26px;
    line-height: 1;

    background:      black;
    background-clip: text;

    color: transparent;
}

.username {
    display:        grid;
    justify-items:  start;
    position:       relative;
}

.username-button,
.username-editor {
    grid-area: 1/1;
}

.username-button {
    cursor:        none;
    padding-right: 16px;
}

.username-button.loading {
    cursor: default;
}

.username-button > * {
    pointer-events: none;
}

.username-text {
    --blue:            #4285F4;
    --green:           #34A853;
    --red:             #EA4335;
    --yellow:          #FBBC05;
    --rotate-gradient: 0deg;

    color: transparent;

    background-image: conic-gradient(in oklch from var(--rotate-gradient) at 50% 100%,
        var(--red)    0%,
        var(--blue)   25%,
        var(--green)  50%,
        var(--yellow) 75%,
        var(--red)    100%);
    background-clip: text;

    animation: rotate-gradient 2s linear infinite;
}

.username-editor {
    display:   flex;
    flex-flow: row nowrap;
}

.at-symbol {
    cursor:       default;
    margin-right: -1.5px;
    color:        black;
}

.username-text-input {
    width:      200px;
    height:     1em;
    margin:     0;
    padding:    0;
    border:     0;

    appearance:         none;
    -webkit-appearance: none;
    box-sizing:         border-box;

    background:   transparent;
    color:        black;
    font-family:  inherit;
    font-size:    inherit;
    line-height:  10px;
    outline:      none !important;
}

.submit-button {
    display:         flex;
    align-items:     center;
    justify-content: center;
    width:           40px;
    margin:          -6px;
    padding:         2px 30px;

    color:     black;
    font-size: 2.4rem;

    background:    hsl(113, 68%, 58%);
    border:        var(--border-s); /*???*/
    border:        2.5px solid black;
    border-radius: 999px;
    box-shadow:    1px 1px 0 0 black;

    transition: transform 200ms ease;

    transform: scale(1) rotate(0deg);
}

.submit-button:hover:not(.invalid) {
    box-shadow: 2px 2px 0 0 black;
    transform:  scale(1.02) rotate(-1deg);
}

.submit-button:active:not(.invalid) {
    box-shadow: 1px 1px 0 0 black;
    background: hsl(113, 56%, 53%);
    transform:  scale(0.98) rotate(0deg);
}

.submit-button.invalid {
    cursor:  not-allowed;
    opacity: 0.5;
}

.submit-icon {
    font-size: 2.3rem;
    left:      1px;
    position:  relative;
    top:       1px;
}

.loading-icon {
    animation: rotate-loading 600ms infinite steps(8, end);
}

.pencil-icon-container {
    display:     grid;
    left:        0;
    place-items: center;
    position:    absolute;
    top:         0;

    animation: bob-pencil 2s infinite;
}

.pencil-icon {
    --pencil-eraser:   #ff899a;
    --pencil-tip:      black;
    --pencil-wood:     #f2d198;
    --pencil-wrapper:  #ff9c08;

    grid-area: 1/1;

    background: linear-gradient(42deg,
        var(--pencil-tip)     20%,
        var(--pencil-wood)    20% 34%,
        var(--pencil-wrapper) 34% 63%,
        var(--pencil-eraser)  63%);
    background-clip: text;

    color:       transparent;
    line-height: 1.5;
}

.pencil-icon-stroke {
    grid-area: 1/1;

    -webkit-text-stroke: 4px white;
    line-height:         1.5;
}

.username-section:not(:hover) .pencil-icon-container {
    display: none;
}

@property --rotate-gradient {
    inherits:      false;
    initial-value: 0deg;
    syntax:        '<angle>';
}

@keyframes rotate-gradient {
    from { --rotate-gradient: 0deg; }
    to   { --rotate-gradient: 360deg; }
}

@keyframes bob-pencil {
    0%   { transform: rotate(-6deg) scale(1.04); }
    25%  { transform: rotate( 6deg) scale(0.99); }
    50%  { transform: rotate(-6deg) scale(1.04); }
    75%  { transform: rotate( 6deg) scale(0.99); }
    100% { transform: rotate(-6deg) scale(1.04); }
}

@keyframes rotate-loading {
    from { transform: rotate(0deg); }
    to   { transform: rotate(360deg); }
}
</style>