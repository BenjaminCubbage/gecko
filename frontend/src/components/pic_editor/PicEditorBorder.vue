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
    align-items:    stretch;
    display:        flex;
    flex-direction: column;
    gap:            10px;
    padding:        10px;

    background:
        linear-gradient(
            var(--border-col-2) 50%,
            var(--border-col-3) 50%);

    box-shadow:
        var(--border-shadow),
        inset 0 3px 0 var(--border-col-1);

    border-radius: var(--radius-s);
    border:        var(--border-s);
    corner-shape:  notch;
}

.pic-editor-border--outer {
    --border-col-1:  var(--col-green-0);
    --border-col-2:  var(--col-green-1);
    --border-col-3:  var(--col-green-4);
    --border-shadow: var(--shadow-l);
}

.pic-editor-border--inner {
    --border-col-1:  var(--col-gray-1);
    --border-col-2:  var(--col-gray-2);
    --border-col-3:  var(--col-gray-3);
    --border-shadow: var(--shadow-s);
}
</style>