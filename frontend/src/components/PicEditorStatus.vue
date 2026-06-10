<template>
    <div
        class="pic-editor-status">
        <transition name="swap-status" mode="out-in">
            <span 
                v-if="hasStatus"
                :key="`${statusIcon}:${statusText}`"
                class="status-text"
                :data-icon="statusIcon"
                aria-hidden="true">
                {{ statusText }}
            </span>
        </transition>
        
        <output
            class="util-sr-only"
            aria-atomic="true">
            {{ statusText }}
        </output>
    </div>
</template>

<script setup>
import {
    computed,
    onUnmounted,
    ref
} from 'vue';

const statusType = ref(null);
const statusText = ref(null);

const expiresMS = 5000;
let expireTimer = null;

const hasStatus = computed(() =>
    statusType.value?.length > 0 &&
    statusText.value?.length > 0);

const statusIcon = computed(() =>
    statusType.value === 'success'
        ? '\uF514'   // Check mark icon
        : '\uF515'); // X icon

function pushStatus({ statusType: newStatusType, statusText: newStatusText } = {}) {
    if (import.meta.env.DEV && ![
            'success',
            'error'
        ].includes(newStatusType)) {
        console.error(`[PicEditorStatus]: Unexpected status type: ${newStatusType}`);
    }

    statusType.value = newStatusType;
    statusText.value = newStatusText;

    clearTimeout(expireTimer);
    expireTimer = setTimeout(() => {
        statusType.value = null;
        statusText.value = null;
    }, expiresMS);
}

onUnmounted(() =>
    clearTimeout(expireTimer));

defineExpose({
    pushStatus
});
</script>

<style scoped>
.status-text {
    display:     flex;
    align-items: center;

    gap:     6px;
    padding: 3px 10px;

    pointer-events: none;

    color: white;
    font:  2.3rem/0.8 var(--font-main);

    text-transform: uppercase;

    border-radius: var(--radius-s);
    background:    black;

    &::before {
        content: attr(data-icon) / '';

        font-size: 1.2em;
        translate: 0 -1px;
    }
}

.swap-status-enter-active,
.swap-status-leave-active {
    transition: translate 150ms, opacity 150ms;
}

.swap-status-enter-from { opacity: 0; translate: 0  5px; }
.swap-status-leave-to   { opacity: 0; translate: 0 -5px; }
</style>