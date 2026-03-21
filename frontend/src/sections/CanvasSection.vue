<template>
    <section>
        <RecipientSelect
            @selection-changed="recipientDeviceChanged" />
        <PicEditor :recipient-device="recipientDevice" />
    </section>
</template>

<script setup>
import { ref }         from 'vue';
import PicEditor       from '@/components/PicEditor.vue';
import RecipientSelect from '@/components/RecipientSelect.vue';

const recipientDevice = ref(null);

function recipientDeviceChanged(device) {
    recipientDevice.value = device;
}
</script>