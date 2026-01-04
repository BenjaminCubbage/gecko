<template>
    <i class="spinner hn hn-spinner-solid"></i>
</template>

<script setup>
</script>

<style scoped>
.spinner {
    display: inline-block;

    font-size: 2.3rem;
    color: black;
    animation: rotate 700ms infinite steps(8, end);
}

@keyframes rotate {
    from { transform: rotate(0deg); }
    to   { transform: rotate(360deg); }
}
</style>