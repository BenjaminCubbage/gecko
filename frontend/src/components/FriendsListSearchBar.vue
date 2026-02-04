<template>
    <div
        class="friends-list-search-bar"
        :class="{ 'disabled': disabled }">
        <div class="border"
            @keydown.enter="enterPressed = true"
            @keyup.enter="() => {
                enterPressed = false;
                submit();
            }"
            @focusout="enterPressed = false">
            <UsernameInput
                v-model="inputText"
                class="search-input"
                :disabled="disabled"
                @validityChanged="v => inputIsValid = v" />

            <button
                class="search-button"
                :class="{ 'pressed-down': inputIsValid && mode == 'normal' && enterPressed }"
                :disabled="disabled || !inputIsValid || mode == 'loading'"
                @click="submit">
                <div                 v-if="mode == 'normal'" class="search-icon hn hn-search"></div>
                <LoadingSpinner v-else-if="mode == 'loading'" />
            </button>
        </div>
    </div>
</template>

<script setup>
import { ref } from 'vue';
import UsernameInput  from './UsernameInput.vue';
import LoadingSpinner from './LoadingSpinner.vue';

defineProps({
    // 'normal' | 'loading'
    mode: { type: String, default: 'normal' },
    disabled: { type: Boolean, default: false }
});

const emit = defineEmits([ 'submit' ]);

const inputText = defineModel({ required: true });

const enterPressed = ref(false);
const inputIsValid = ref(false);

function submit() {
    if (inputIsValid.value)
        emit('submit');
}
</script>

<style>
.friends-list-search-bar {
    padding: 8px;
}

.border {
    display:               grid;
    grid-template-columns: 1fr auto;

    border:        var(--border-s);
    border-radius: 999px;
    box-shadow:    inset var(--shadow-s);
}

.search-input {
    grid-column: 1;
    margin:      0;
    padding:     5px 24px 4px 24px;
    width:       100%;

    appearance:         none;
    -webkit-appearance: none;

    background:     transparent;
    border:         0;
    color:          black;
    font-family:    inherit;
    font-size:      2.2rem;
    letter-spacing: 0.02em;
    outline:        none;
}

.search-button {
    grid-column: 2;
    width:       70px;

    background:                 #91df43;
    border-left:                var(--border-s);
    border-top-right-radius:    999px;
    border-bottom-right-radius: 999px;
    box-shadow:                 inset 0 3px 0 #adf467;
}

.search-icon {
    position: relative;

    color:     black;
    font-size: 2.2rem;
}

.search-button:active,
.search-button.pressed-down {
    box-shadow: inset 0 2px 0 black;
}

.search-button:active .search-icon,
.search-button.pressed-down .search-icon {
    transform: translateY(2px);
}

.friends-list-search-bar.disabled {
    opacity:        0.5;
    pointer-events: none;
}

.search-input:disabled {
    opacity:        0;
    pointer-events: none;
}

.search-button:disabled {
    pointer-events: none;
}

.search-button:disabled .search-icon {
    opacity: 0.4;
}
</style>