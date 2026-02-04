<template>
    <div class="recipient-select-device-signal">
        <StrokedText>
            <transition name="transition" mode="out-in">
                <template v-if="status === 'online'">
                    <div class="status-text--on" key="on"></div>
                </template>

                <template v-else-if="status === 'offline'">
                    <div class="status-text--off" key="off"></div>
                </template>

                <template v-else-if="status === 'pending'">
                    <div class="status-text--pending" key="pending"></div>
                </template>

                <template v-else>
                    <div class="status-text--loading" key="loading"></div>
                </template>
            </transition>
        </StrokedText>
    </div>
</template>

<script setup>
import StrokedText from './StrokedText.vue';

defineProps({
    status: { required: true }
});
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

.status-text--error::after {
    content: 'COULDN\'T GET STATUS';
    color:   crimson;
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