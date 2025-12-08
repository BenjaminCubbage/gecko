<template>
    <div class="active-user-strip">
        <i class="user-icon hn hn-user-solid"></i>

        <div class="username-section">
            <div class="username">
                <div class="username-editor" v-show="isEditing || isPending">
                    <div class="at-symbol"
                        :style="{
                            'visibility': isPending ? 'hidden' : 'visible'
                        }">@</div>
                    <input
                        ref="inputEl" 
                        type="text" 
                        class="username-text-input" 
                        :style="{
                            'visibility': isPending ? 'hidden' : 'visible'
                        }"
                        @blur="editorLostFocus"
                        @input="inputChanged"
                        @keydown.enter="submit"
                        :maxlength="maxUsernameLength"
                        v-model="inputTextValue" />

                    <button 
                        v-show="isEditing || isPending"
                        class="submit-button"
                        :class="{ 
                            'invalid': !isValidUsernameInput(inputTextValue)
                        }"
                        ref="submitButtonEl"
                        @click="submit"
                        @blur="editorLostFocus">
                        <i v-if="isPending" class="pending-icon hn hn-spinner-solid"></i>
                        <i v-else class="submit-icon hn hn-check-solid"></i>
                    </button>
                </div>

                <button 
                    class="username-button"
                    :class="{ 'pending': isPending }"
                    @click="emit('requestEdit')" 
                    :disabled="isPending || isEditing"
                    @mousemove.self="teleportPencilToMouse"
                    @mouseenter="teleportPencilToMouse"
                    v-show="!isEditing || isPending">
                    <div class="username-text">
                        <span v-if="!isPending">
                            @{{ username }}
                        </span>
                        <span v-else>
                            @{{ inputTextValue }}
                        </span>
                    </div>

                    <div
                        class="pencil-icon-container" 
                        v-show="!isPending"
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
    import { toRefs, ref, defineProps, defineEmits, 
             useTemplateRef, nextTick, watch, computed } from 'vue';
    import { characterIsAlphaNumeric } from '@/core/string/CharacterIsAlphanumeric.js';

    const minUsernameLength = 3;
    const maxUsernameLength = 18;
    const isValidUsernameChar 
        = c => characterIsAlphaNumeric(c) || c == '_';

    const propsObj = defineProps({
        username: { type: String, required: true },
        forbiddenUsernames: { type: Array, default: [] },

        // 'normal' | 'editing' | 'pending'
        status: { type: String, required: true }
    });

    const props = toRefs(propsObj);

    const emit = defineEmits([
        'requestEdit',
        'submit',
        'cancel'
    ]);

    const inputEl        = useTemplateRef('inputEl');
    const submitButtonEl = useTemplateRef('submitButtonEl');

    const isEditing = computed(() => props.status.value === 'editing');
    const isPending = computed(() => props.status.value === 'pending');

    const pencilPosition = ref({ x: 0, y: 0 });
    const inputTextValue = ref(props.username.value);

    const pencilDimensions = ref({
        w: 28,
        h: 40
    });

    function teleportPencilToMouse(e) {
        pencilPosition.value.x = e.offsetX - pencilDimensions.value.w / 2;
        pencilPosition.value.y = e.offsetY - pencilDimensions.value.h / 2;
    }

    function editorLostFocus(e) {
        if (e.relatedTarget != submitButtonEl.value &&
            e.relatedTarget != inputEl.value &&
            isEditing.value) {
            inputTextValue.value = props.username.value;
            emit('cancel');
        }
    }

    function inputChanged() {
        inputTextValue.value = inputTextValue.value.split('')
            .filter(isValidUsernameChar)
            .join('');
    }

    function isValidUsernameInput() {
        return inputTextValue.value.length >= minUsernameLength &&
               inputTextValue.value.length <= maxUsernameLength &&
               !inputTextValue.value.split('').some(c => !isValidUsernameChar(c)) &&
               inputTextValue.value !== props.username.value &&
               !props.forbiddenUsernames.value.includes(inputTextValue.value);
    }

    function submit() {
        if (isValidUsernameInput(inputTextValue.value))
            emit('submit', inputTextValue.value);
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

    .username-button.pending {
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

        /* 1em because input doesn't respect line-height: 1 */
        height: 1em;
    }

    .submit-button {
        font-size: 2.4rem;
        padding: 2px 30px;
        margin: -6px;
        background: hsl(113, 68%, 58%);
        border: var(--border-small);
        border: 2.5px solid black;
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

    .pending-icon {
        animation: rotate-pending 600ms infinite steps(8, end);
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
        background: linear-gradient(42deg, black 20%, #f2d198 20%, #f2d198 34%, #ff9c08 34%, #ff9c08 63%, #ff899a 63%);
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

    @keyframes rotate-pending {
        from { transform: rotate(0deg); }
        to { transform: rotate(360deg); }
    }
</style>