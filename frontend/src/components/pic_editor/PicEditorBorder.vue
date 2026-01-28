<template>
    <div class="pic-editor-border">
        <div class="border-outer">
            <slot />
        </div>
    </div>
</template>

<style scoped>
.pic-editor-border {
    z-index: 0;
    display: grid;
    font-family: var(--font-heading);
    align-self: start;
}

.border-outer {
    background: linear-gradient(-22deg,
        #9adf55 25%,
        #b3e87d 25% 50%,
        #a6ed5e 50% 75%,
        #ccebad 75%
    );
    background-size: 1% 100%;
    background-repeat: repeat;

    padding: 10px;
    border: var(--border-small);
    border-radius: var(--border-radius-large);
    corner-shape: bevel;

    box-shadow: var(--border-shadow-large);

    display: flex;
    flex-flow: column nowrap;

    gap: 8px;
}
</style>