<template>
    <div class="recipient-select-log-in-prompt">
        <TextContentBorder>
            You must
            <TextContentInlineButton
                class="log-in-button" 
                aria-label="Log in"
                @click="logIn">
                log in
            </TextContentInlineButton>
            to send images
        </TextContentBorder>
    </div>
</template>

<script setup>
import {
    inject
} from 'vue';

import TextContentBorder       from './TextContentBorder.vue';
import TextContentInlineButton from './TextContentInlineButton.vue';

import { Keys } from '@/core/di/keys.js';

const session = inject(Keys.SessionStore);

function logIn() {
    session.requestLogIn();
}
</script>

<style scoped>
.log-in-button {
    font-size: 1.1em;
}
</style>