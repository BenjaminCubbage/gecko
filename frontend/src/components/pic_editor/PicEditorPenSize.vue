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

    box-shadow: inset 0 2.5px 0 #ffcc00,
                var(--border-shadow-small);

    background-color: #ffc400;


    --dot-color: #ecad1b;
    background-image: radial-gradient(circle at top,    var(--dot-color) 2px, transparent 2px),
                        radial-gradient(circle at bottom, var(--dot-color) 2px, transparent 2px),
                        radial-gradient(circle at right,  var(--dot-color) 2px, transparent 2px),
                        radial-gradient(circle at left,   var(--dot-color) 2px, transparent 2px);

    background-size: 18px 18px;
    background-repeat: repeat;
    background-position-x: 50%;
    background-position-y: 50%;

    transition: transform 100ms ease,
                box-shadow 100ms ease;
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
    border: 2px solid #ffc400;
}

.pen-size--small  { --dot-size: 12px; border-top-left-radius: 18px; border-bottom-left-radius: 18px; padding-left: 12px; }
.pen-size--medium { --dot-size: 18px; }
.pen-size--large  { --dot-size: 24px; border-top-right-radius: 18px; border-bottom-right-radius: 18px; padding-right: 12px; }

.pen-size:hover:not(.pen-size--selected) {
    background-color: #ffd13b;
}

.pen-size--selected {
    background-color: #ffd13b;
    box-shadow: inset 0 2.5px 0 #ffdd00;
    transform: translate(2px, 2px);
}
</style>