<template>
    <Transition name="bubble">
        <div v-show="isShowingMessage" class="status-bubble-container">
            <div class="status-bubble">
                <div class="status-bubble-box">
                    {{ currentMessage }}
                </div>

                <div class="status-bubble-caret">
                </div>
            </div>
        </div>
    </Transition>
</template>

<script setup>
import { ref } from 'vue';

const currentMessage = ref(':D');
const isShowingMessage = ref(0);

function showMessage(message, milliseconds = 3000) {
    currentMessage.value = message;
    ++isShowingMessage.value;

    setTimeout(() => {
        --isShowingMessage.value;
    }, milliseconds);
}

defineExpose({ showMessage });
</script>

<style scoped>
.status-bubble-container {
    position:  relative;
    animation: throb 2000ms ease infinite;
}

.status-bubble {
    position: absolute;
    top:      14px;
    width:    400px;
}

.status-bubble-box {
    display:    inline-block;
    min-width:  60px;
    padding:    8px 16px;

    color:       black;
    font-family: var(--font-heading);
    font-size:   2.2rem;
    line-height: 0.9;

    background:    hsl(0, 100%, 71%);
    border:        2.5px solid black;
    border-radius: 4px;
    box-shadow:    4px 4px 0 black;
}

.status-bubble-caret {
    content: "";

    height:   16px;
    width:    16px;
    left:     24.5px;
    position: absolute;
    top:      -7.5px;

    background:             hsl(0, 100%, 71%);
    border:                 2.5px solid black;
    border-bottom:          0;
    border-right:           0;
    border-top-left-radius: 3px;
    transform:              rotate(45deg);
}

.bubble-enter-active,
.bubble-leave-active {
    transition: opacity 200ms ease;
}

.bubble-enter-from,
.bubble-leave-to {
    opacity: 0;
}

@keyframes throb {
    0%   { transform: scale(0.98); }
    50%  { transform: scale(1.00); }
    100% { transform: scale(0.98); }
}
</style>