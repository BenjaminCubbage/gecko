<template>
    <section>
        <RecipientSelect
            :reveal="revealRecipientSelect"
            @selection-changed="recipientDeviceChanged" />
        <PicEditor :recipient-device="recipientDevice" />
    </section>
</template>

<script setup>
import { ref }         from 'vue';
import PicEditor       from '@/components/PicEditor.vue';
import RecipientSelect from '@/components/RecipientSelect.vue';

defineProps({
    revealRecipientSelect: {
        type:    Boolean,
        default: true
    }
});

const recipientDevice = ref(null);

function recipientDeviceChanged(device) {
    recipientDevice.value = device;
}
</script>