<template>
    <div class="recipient-select-device-signal">
        <RecipientSelectStrokedText>
            <transition name="transition" mode="out-in">
                <template v-if="statusOn">
                    <div class="status-text--on">
                    </div>
                </template>

                <template v-else-if="statusOff">
                    <div class="status-text--off">
                    </div>
                </template>

                <template v-else-if="statusPending">
                    <div class="status-text--pending">
                    </div>
                </template>

                <template v-else>
                    <div class="status-text--loading">
                    </div>
                </template>
            </transition>
        </RecipientSelectStrokedText>
    </div>
</template>

<script setup>
import { computed } from 'vue';
import RecipientSelectStrokedText from './RecipientSelectStrokedText.vue';

const props = defineProps({
    // 'online'  -> device is online
    // 'offline' -> device is offline
    // 'pending' -> connection status not yet known by server
    // 'loading' -> connection status not yet known by client
    status: { type: String, required: true }
});

const statusOn      = computed(() => props.status == 'online');
const statusOff     = computed(() => props.status == 'offline');
const statusPending = computed(() => props.status == 'pending');
const statusLoading = computed(() => !statusOn && !statusOff && !statusPending);
</script>

<style scoped>
    .recipient-select-device-signal {
        font-size: 1.3rem;
        line-height: 1.3;
        position: relative;
        top: 1px;
        height: 17px;
    }

    .status-text--on::after {
        color: green;
        content: 'ONLINE';
    }

    .status-text--off::after {
        color: crimson;
        content: 'OFFLINE';
    }

    .status-text--pending::after {
        color: dodgerblue;
        content: 'PENDING';
    }

    .status-text--loading::after {
        color: dodgerblue;
        content: '...';
        animation: loading 300ms steps(3) infinite;
        font-size: 1.5em;
        line-height: 0;
        position: relative;
        top: -2px;
    }

    .transition-enter-active,
    .transition-leave-active {
        transition: transform 50ms ease;
    }

    .transition-enter-from,
    .transition-leave-to {
        transform: scale(0.9);
    }

    @keyframes loading {
        0%  { content: '...'; }
        33% { content: '.'; }
        66% { content: '..'; }
    }
</style>