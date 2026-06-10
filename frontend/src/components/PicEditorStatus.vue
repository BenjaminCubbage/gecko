<template>
    <transition name="swap-status" mode="out-in">
        <output
            class="pic-editor-status"
            v-if="statusType?.length > 0 && statusText?.length > 0"
            :key="`${statusType}:${statusText}`">
            {{ statusText }}
        </output>
    </transition>
</template>

<script setup>
import {
    computed,
    onUnmounted,
    ref
} from 'vue';

const isVisible = ref(false);

const statusType = ref(null);
const statusText = ref(null);

const statusDur = 4000;
let expireTimer = null;

const statusIcons = {
    'success': '\uF514',
    'error':   '!'
};

function pushStatus({ statusType: newStatusType, statusText: newStatusText }) {
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
    }, statusDur);
}

onUnmounted(() =>
    clearTimeout(expireTimer));

defineExpose({
    pushStatus
});
</script>

<style scoped>
.pic-editor-status {
    display:     flex;
    gap:         6px;
    align-items: center;

    pointer-events: none;

    padding: 3px 10px 3px 10px;

    color: white;
    font:  2.3rem/0.8 var(--font-main);

    text-transform: uppercase;

    border-radius: var(--radius-s);
    background:    black;

    box-shadow:
        var(--shadow-dist-m)
        var(--shadow-dist-m)
        var(--col-shadow-alpha);

    &::before {
        content: v-bind('statusType === "success" ? "\'\uF514\'" : "\'\uF515\'"') / '';

        font-size: 1.2em;
        translate: 0 -1px;
    }
}

.swap-status-enter-active { animation: swap-in  150ms; }
.swap-status-leave-active { animation: swap-out 150ms; }

@keyframes swap-in {
    from { opacity: 0; translate: 0px 5px; }
    to   { opacity: 1; translate: 0px 0px; }
}

@keyframes swap-out {
    from { opacity: 1; translate: 0px 0px; }
    to   { opacity: 0; translate: 0px -5px; }
}
</style>