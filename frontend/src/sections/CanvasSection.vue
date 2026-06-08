<template>
    <section>
        <RecipientSelect
            v-if="session.state.value === 'ready'"
            class="recipient-select"
            @selection-changed="recipientDeviceChanged" />

        <PicEditor
            class="pic-editor"
            :recipient-device="recipientDevice" />
    </section>
</template>

<script setup>
import { 
    ref,
    inject
} from 'vue';

import PicEditor       from '@/components/PicEditor.vue';
import RecipientSelect from '@/components/RecipientSelect.vue';
import { Keys }        from '@/core/di/keys.js';

const session = inject(Keys.SessionStore);

const recipientDevice = ref(null);

function recipientDeviceChanged(device) {
    recipientDevice.value = device;
}
</script>