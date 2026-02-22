<template>
    <div
        class="pic-editor-border"
        :class="[
            variant === 'outer'
                ? 'pic-editor-border--outer'
                : 'pic-editor-border--inner']">
        <slot />
    </div>
</template>

<script setup>
defineProps({
    variant: { type: String, default: 'outer' }
});
</script>

<style scoped>
.pic-editor-border {
    display: flex;
    flex-direction: column;
    z-index: 0;
    gap:     10px;

    background:
        linear-gradient(
            var(--border-col-2) 50%,
            var(--border-col-3) 50%);

    box-shadow:
        var(--border-shadow),
        inset  3px  3px 0 var(--border-col-1),
        inset -3px -3px 0 var(--border-col-4);

    border-radius: var(--radius-s);
    border:        var(--border-s);
    corner-shape:  notch;
}

.pic-editor-border--outer {
    padding: 12px 8px;

    --border-col-1:  var(--col-green-0);
    --border-col-2:  var(--col-green-2);
    --border-col-3:  var(--col-green-4);
    --border-col-4:  var(--col-green-5);
    --border-shadow: var(--shadow-l);
}

.pic-editor-border--inner {
    padding: 8px 8px;
    --border-col-1:  var(--col-gray-1);
    --border-col-2:  var(--col-gray-2);
    --border-col-3:  var(--col-gray-3);
    --border-col-4:  var(--col-gray-4);
    --border-shadow: var(--shadow-s);
}
</style>