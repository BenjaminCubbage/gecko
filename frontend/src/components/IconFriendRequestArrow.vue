<template>
    <svg
        role="presentation"
        class="icon-friend-request-arrow"
        :data-variant="variant"
        viewBox="-3 -3 39.3 23.5">
        <path class="st0 stroke" d="M31.9 8.2V6.3H30v-2h-2v-2h-2v2 2h-1.9-2v1.9h-2V6.3h-2v-2h-1.9-2
            v2h-2-1.9v1.9h-2V6.3h-2v-2H4.4v2h-2v1.9 2h2v2h1.9 2v2h2 1.9v-2h2v-2h2
            v2h1.9 2v2h2 2v-2H26v2 1.9h2v-1.9h2v-2h1.9v-2h2v-2z"/>
        <g class="st1">
            <path d="M30 8.2V6.3h-2-2v1.9h-1.9-2v2h-2v2h2 2v-2H26v2h2 2v-2h1.9v-2z"/>
            <path d="M18.1 8.2V6.3h-1.9-2v1.9h2v2h1.9 2v-2z"/>
            <path d="M10.3 8.2v2h-2v2h2 1.9v-2h2v-2h-2z"/>
            <path d="M6.3 8.2V6.3H4.4v1.9 2h1.9 2v-2z"/>
        </g>
        <g class="st2">
            <path d="M31.9 8.2h2v2h-2z"/><path d="M30 10.2h2v2h-2z"/>
            <path d="M26 12.2v2 1.9h2v-1.9h2v-2h-2z"/><path d="M24.1 10.2h2v2h-2z"/>
            <path d="M20.1 12.2v2h2 2v-2h-2z"/><path d="M18.1 10.2h-1.9v2h1.9 2v-2z"/>
            <path d="M14.2 8.2h2v2h-2z"/><path d="M12.2 10.2h2v2h-2z"/>
            <path d="M8.3 12.2v2h2 1.9v-2h-1.9z"/><path d="M6.3 10.2H4.4v2h1.9 2v-2z"/>
        </g>
    </svg>
</template>

<script setup>
defineProps({
    variant: {
        type:     String,
        required: true,
        validator(value) {
            return [
                'incoming',
                'outgoing'
            ].includes(value);
        }
    }
});
</script>

<style scoped>
.icon-friend-request-arrow {
    & > .stroke {
        stroke-width: 5px;
    }

    &[data-variant=incoming] {
        & > .st0 { fill: var(--col-green-0); }
        & > .st1 { fill: var(--col-green-3); }
        & > .st2 { fill: var(--col-green-5); }
    }

    &[data-variant=outgoing] {
        & > .st0 { fill: var(--col-lt-blue-1); }
        & > .st1 { fill: var(--col-lt-blue-4); }
        & > .st2 { fill: var(--col-lt-blue-7); }
        
        scale: -1 1;
    }
}
</style>