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
}

.border-outer {
    background: linear-gradient(
        #ccebad 50%,
        #9adf55 50%
    );

    padding: 10px;
    border: var(--border-small);
    border-radius: var(--border-radius-large);
    corner-shape: bevel;

    box-shadow: var(--border-shadow-large),
                inset 0 3px 0 #ebf6de;

    display: flex;
    flex-flow: column nowrap;

    gap: 8px;
}
</style>