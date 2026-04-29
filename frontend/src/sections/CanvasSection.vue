<template>
    <section
        class="canvas-section"
        :data-modal-open="showWelcomeModal">
        <!-- Backdrop -->
        <div
            v-if="showWelcomeModal" 
            style="position: absolute; inset: 0"
            @click="welcomeModalDismissed = true"></div>

        <AsideModalWelcome
            v-if="showWelcomeModal"
            class="aside-modal-welcome"
            @log-in="session.requestLogIn()"
            @dismiss="welcomeModalDismissed = true" />

        <RecipientSelect
            class="recipient-select"
            :inert="showWelcomeModal"
            @selection-changed="recipientDeviceChanged" />

        <PicEditor
            class="pic-editor"
            :recipient-device="recipientDevice"
            :inert="showWelcomeModal" />
    </section>
</template>

<script setup>
import {
    computed,
    inject,
    ref,
    watch
} from 'vue';
import AsideModalWelcome from '@/components/AsideModalWelcome.vue';
import PicEditor         from '@/components/PicEditor.vue';
import RecipientSelect   from '@/components/RecipientSelect.vue';

import { Keys } from '@/core/di/keys.js';

const session = inject(Keys.SessionStore);

const recipientDevice       = ref(null);
const welcomeModalDismissed = ref(false);

const notLoggedIn = computed(() => {
    return session.state.value === 'loggedout';
});

const showWelcomeModal = computed(() => {
    return notLoggedIn.value && !welcomeModalDismissed.value;
});

function recipientDeviceChanged(device) {
    recipientDevice.value = device;
}
</script>

<style scoped>
.canvas-section {
    display:  grid;
    position: relative;

    grid-template:
        "recipient" auto
        "editor" /
         1fr;

    & > .aside-modal-welcome { z-index: 1; grid-area: recipient / recipient / editor / editor; }
    & > .recipient-select    { z-index: 0; grid-area: recipient; }
    & > .pic-editor          { z-index: 0; grid-area: editor; }

    &[data-modal-open=true] {
        & > .recipient-select,
        & > .pic-editor {
            filter: brightness(0.7);
        }
    }
}

.aside-modal-welcome {
    place-self: center;
}
</style>