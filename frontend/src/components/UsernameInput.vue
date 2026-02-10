<template>
    <input
        spellcheck="false"
        v-model="inputText"
        ref="inputEl"
        type="text"
        @input="inputChanged()"
        :maxlength="maxUsernameLength" />
</template>

<script setup>
import {
    computed,
    nextTick,
    useTemplateRef,
    watch
} from 'vue';

import { characterIsAlphaNumeric } from '@/core/string/characterIsAlphaNumeric.js';

const inputEl = useTemplateRef('inputEl');
const inputText = defineModel({ required: true });

const props = defineProps({
    forbiddenUsernames: { type: Array, default: () => [] }
});

const emit = defineEmits([
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

defineExpose({
    innerElement: inputEl
});
</script>