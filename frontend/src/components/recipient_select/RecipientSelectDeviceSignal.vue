<template>
    <div class="recipient-select-device-signal">
        <StrokedText>
            <transition name="transition" mode="out-in">
                <template v-if="statusOn">
                    <div class="status-text--on"></div>
                </template>

                <template v-else-if="statusOff">
                    <div class="status-text--off"></div>
                </template>

                <template v-else-if="statusPending">
                    <div class="status-text--pending"></div>
                </template>

                <template v-else-if="statusLoading">
                    <div class="status-text--loading"></div>
                </template>
            </transition>
        </StrokedText>
    </div>
</template>

<script setup>
import { computed } from 'vue';
import StrokedText from '@/components/stroked_text/StrokedText.vue';

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
const statusLoading = computed(() => props.status == 'loading');
</script>

<style scoped>
.recipient-select-device-signal {
    height:      17px;
    position:    relative;
    top:         1px;
    font-size:   1.3rem;
    line-height: 1.3;
}

.status-text--on::after {
    content: 'ONLINE';
    color:   green;
}

.status-text--off::after {
    content: 'OFFLINE';
    color:   crimson;
}

.status-text--pending::after {
    content: 'STATUS PENDING';
    color:   dodgerblue;
}

.status-text--loading::after {
    content:  '...';
    position: relative;
    top:      -2px;

    color:       dodgerblue;
    font-size:   1.5em;
    line-height: 0;

    animation: loading 300ms steps(3) infinite;
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
    33% { content: '.';   }
    66% { content: '..';  }
}
</style>