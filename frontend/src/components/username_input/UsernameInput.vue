<template>
    <input
        spellcheck="false"
        v-model="inputText"
        ref="inputEl"
        type="text"
        @input="inputChanged()"
        @blur="emit('blur', $event)"
        @keydown.enter="trySubmit()"
        :maxlength="maxUsernameLength" />
</template>

<script setup>
import { computed, watch, useTemplateRef, nextTick } from 'vue';
import { characterIsAlphaNumeric } from '@/core/string/CharacterIsAlphaNumeric.js';

const inputEl = useTemplateRef('inputEl');
const inputText = defineModel({ required: true });

const props = defineProps({
    forbiddenUsernames: { type: Array, default: () => [] }
});

const emit = defineEmits([
    'submit',
    'blur',
    'validityChanged'
]);

const minUsernameLength = 3;
const maxUsernameLength = 18;
const isValidUsernameChar
    = c => characterIsAlphaNumeric(c) || c == '_';

const isValid = computed(() => {
    return inputText.value.length >= minUsernameLength &&
           inputText.value.length <= maxUsernameLength &&
           !props.forbiddenUsernames.includes(inputText.value);
});

watch(isValid, (newValue, oldValue) => {
    if (newValue != oldValue)
        emit('validityChanged', isValid.value);
}, { immediate: true });

function inputChanged() {
    /*
        note(ben): nextTick seems to be needed on Firefox
    */
    nextTick(() => {
        inputText.value = inputText.value.split('')
            .filter(isValidUsernameChar)
            .join('');
    });
}

function trySubmit() {
    if (isValid.value)
        emit('submit');
}

defineExpose({
    focus() {
        inputEl.value?.focus()
    }
});
</script>