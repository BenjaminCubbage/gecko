<template>
    <section
        class="canvas-section">
        <RecipientSelect
            class="recipient-select"
            @selection-changed="recipientDeviceChanged" />

        <PicEditor
            class="pic-editor"
            :recipient-device="recipientDevice" />
    </section>
</template>

<script setup>
import { ref } from 'vue';

import PicEditor       from '@/components/PicEditor.vue';
import RecipientSelect from '@/components/RecipientSelect.vue';

const recipientDevice = ref(null);

function recipientDeviceChanged(device) {
    recipientDevice.value = device;
}
</script>

<style scoped>
</style>