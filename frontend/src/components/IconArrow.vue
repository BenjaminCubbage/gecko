<template>
    <svg 
        class="icon-arrow"
        :data-direction="direction"
        viewBox="0 0 256 232.7">
        <g class="arrow">
            <path class="st0 stroke" d="M197.8 104.7V81.5h-23.3V58.2h-23.2V34.9H128v46.6H34.9v69.8H128v46.5h23.3v-23.3h23.2v-23.2h23.3V128h23.3v-23.3z" />
            <path class="st1" d="M197.8 104.7h23.3V128h-23.3zM174.5 128h23.3v23.3h-23.3zM174.5 81.5h23.3v23.3h-23.3zM151.3 151.3h23.3v23.3h-23.3zM128 174.5h23.3v23.3H128zM58.2 128H128v23.3H58.2z" />
            <path class="st2" d="M151.3 58.2h23.3v23.3h-23.3zM128 34.9h23.3v23.3H128zM34.9 81.5v69.8h23.3v-46.6H128V81.5z" />
        </g>
    </svg>
</template>

<script setup>
defineProps({
    direction: {
        type:    String,
        default: 'right',
        validator(value) {
            return [
                'left',
                'right'
            ].includes(value);
        }
    }
});
</script>

<style scoped>
.icon-arrow {
    &[data-direction=left] {
        > .arrow {
            rotate: 180deg;
        }

        .st0 { fill: var(--col-orange-2); }
        .st1 { fill: var(--col-orange-0); }
        .st2 { fill: var(--col-orange-6); }
    }

    &[data-direction=right] {
        .st0 { fill: var(--col-orange-2); }
        .st1 { fill: var(--col-orange-6); }
        .st2 { fill: var(--col-orange-0); }
    }
}

.stroke {
    stroke-width: 70px;
}
</style>