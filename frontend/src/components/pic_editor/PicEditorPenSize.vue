<template>
    <div class="pic-editor-pen-size">
        <div class="buttons">
            <button
                class="pen-size pen-size--small"
                :class="{ 'pen-size--selected': modelValue == 'small' }"
                @click="selectedPenSize = 'small'">
                <div class="dot"></div>
            </button>
            <button
                class="pen-size pen-size--medium"
                :class="{ 'pen-size--selected': modelValue == 'medium' }"
                @click="selectedPenSize = 'medium'">
                <div class="dot"></div>
            </button>
            <button
                class="pen-size pen-size--large"
                :class="{ 'pen-size--selected': modelValue == 'large' }"
                @click="selectedPenSize = 'large'">
                <div class="dot"></div>
            </button>
        </div>
    </div>
</template>

<script setup>
const selectedPenSize = defineModel({ default: 'small' });
</script>

<style scoped>
.pic-editor-pen-size {
    align-self: start;
    width: auto;
}

.buttons {
    border-radius: 999px;
    display: flex;
    gap: 4px;
}

.pen-size {
    height: 36px;
    color: black;
    padding: 0 6px;

    line-height: 0;
    background: white;

    border-radius: 3px;
    border: var(--border-small);

    box-shadow: 2px 2px 0 black;

    background: linear-gradient(-30deg,
        #ffb35a 25%,
        #ffd8a3 25% 50%,
        #ffbf73 50% 75%,
        #fff1df 75%
    );

    transition: transform 50ms linear,
                box-shadow 50ms linear;
}

.dot {
    position: relative;
    width: 36px;
}

.dot::after {
    content: "";
    position: absolute;

    width: var(--dot-size);
    height: var(--dot-size);
    left: calc(50% - var(--dot-size) / 2);
    top: calc(50% - var(--dot-size) / 2);

    border-radius: 999px;
    background: black;
    box-shadow: 0 0 0 3px white;
}

.pen-size--small  { --dot-size: 10px; border-top-left-radius: 18px; border-bottom-left-radius: 18px; padding-left: 10px; }
.pen-size--medium { --dot-size: 16px; }
.pen-size--large  { --dot-size: 20px; border-top-right-radius: 18px; border-bottom-right-radius: 18px; padding-right: 10px; }

.pen-size:hover:not(.pen-size--selected) {
    background-color: #ffd13b;
    box-shadow: 1px 1px 0 black;
    transform: translate(1px, 1px);
}

.pen-size--selected {
    background-color: #ffd13b;
    box-shadow: 0 0 0 2px white;
    transform: translate(2px, 2px);
}
</style>