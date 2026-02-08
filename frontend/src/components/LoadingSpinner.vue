<template>
    <i class="spinner hn hn-spinner-solid"></i>
</template>

<style scoped>
.spinner {
    display:   inline-block;
    color:     inherit;
    font-size: 2.3rem;
    animation: rotate 700ms infinite steps(8, end);
}

@keyframes rotate {
    from { transform: rotate(0deg);   }
    to   { transform: rotate(360deg); }
}
</style>