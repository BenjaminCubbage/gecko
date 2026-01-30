<template>
    <div class="pic-editor-border">
        <slot />
    </div>
</template>

<style scoped>
.pic-editor-border {
    padding: 10px;

    background:
        linear-gradient(
            var(--col-green-1) 50%,
            var(--col-green-4) 50%);

    box-shadow:
        var(--shadow-l),
        inset 0 3px 0 var(--col-green-0);

    border-radius: var(--radius-s);
    border:        var(--border-s);
    corner-shape:  notch;
}
</style>