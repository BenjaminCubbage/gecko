<template>
    <div class="pill-toggle">
        <div class="buttons">
            <button
                class="canvas"
                :class="{ 'active-option': modelValue == 'canvas' }"
                @click="selectedTab = 'canvas'">
                <StrokedText>CANVAS</StrokedText>
            </button>

            <button
                class="friends"
                :class="{ 'active-option': modelValue == 'friends' }"
                @click="selectedTab = 'friends'">
                <StrokedText>FRIENDS</StrokedText>
            </button>
        </div>
    </div>
</template>

<script setup>
import StrokedText from '@/components/stroked_text/StrokedText.vue';

const selectedTab = defineModel({ default: 'canvas' });
</script>

<style scoped>
.buttons {
    display: flex;
    flex-flow: row nowrap;
    gap: 10px;

    font-family: var(--font-heading);
    font-size: 2rem;
}

.canvas, .friends {
    width: 116px;

    border-radius: 999px;
    border: var(--border-small);
    box-shadow: var(--border-shadow-small);
    padding: 0px 20px;
    background: white;

    transition: transform 100ms ease, box-shadow 100ms ease;
}

.canvas {
    background: linear-gradient(-30deg,
        #91df43 25%,
        #b3e87d 25% 50%,
        #a6ed5e 50% 75%,
        #ccebad 75%
    );
}

.friends {
    background: linear-gradient(-30deg,
        #ffb35a 25%,
        #ffd8a3 25% 50%,
        #ffbf73 50% 75%,
        #fff1df 75%
    );
}

.active-option {
    box-shadow: 0 0 0 2px white;
    transform: translate(2px, 2px);
}

:is(.canvas, .friends):not(.active-option):hover {
    box-shadow: 1px 1px 0 black;
    transform: translate(1px, 1px);
}
</style>