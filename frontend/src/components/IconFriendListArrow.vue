<template>
    <svg
        class="icon-list-arrow"
        :data-direction="direction"
        :data-variant="variant"
        :data-color="color"
        :viewBox="
            direction === 'up'
                ? '-5 -5 266 237.5'
                : '0 -5 236.5 266'">
        <g class="arrow">
            <path class="st0 stroke" d="M170.7 113.8V85.3h-28.5V56.9h-28.4V28.4H28.4v56.9h28.5v85.4H28.4v56.9h85.4v-28.5h28.4v-28.4h28.5v-28.5h28.4v-28.4z" />
            <path class="st1" d="M56.9 199.1v28.5h56.9v-28.5zM113.8 199.1h28.4v-28.4h-28.4zM142.2 170.7h28.5v-28.5h-28.5zM170.7 113.8v28.4h28.4v-28.4zM142.2 85.3v28.5h28.5V85.3z" />
            <path class="st2" d="M56.9 85.3h28.4v85.3H56.9zM28.4 170.7h28.4v56.9H28.4zM113.8 56.9v28.4h28.4V56.9zM28.4 28.4v56.9h28.5V56.9h56.9V28.4z" />
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
                'right',
                'up'
            ].includes(value);
        }
    },

    variant: {
        type:    String,
        default: 'normal',
        validator(value) {
            return [
                'normal',
                'inactive'
            ].includes(value);
        }
    },

    color: {
        type:    String,
        default: 'orange',
        validator(value) {
            return [
                'green',
                'orange'
            ].includes(value);
        }
    }
});
</script>

<style scoped>
@keyframes rotate-arrow {
    from { rotate: 0turn; }
    to   { rotate: 1turn; }
}

.icon-list-arrow {
    overflow: visible;

    &[data-direction=left] > .arrow {
        rotate: 180deg;
    }

    &[data-direction=up] > .arrow {
        rotate: -90deg;
        scale:  1 -1;
    }

    &[data-direction=right] {
        &[data-color=green] {
            .st0 { fill: var(--col-green-3); }
            .st1 { fill: var(--col-green-6); }
            .st2 { fill: var(--col-green-0); }
        }

        &[data-color=orange] {
            .st0 { fill: var(--col-orange-3); }
            .st1 { fill: var(--col-orange-6); }
            .st2 { fill: var(--col-orange-0); }
        }
    }

    &[data-direction=left],
    &[data-direction=up] {
        &[data-color=green] {
            .st0 { fill: var(--col-green-3); }
            .st1 { fill: var(--col-green-0); }
            .st2 { fill: var(--col-green-6); }
        }

        &[data-color=orange] {
            .st0 { fill: var(--col-orange-3); }
            .st1 { fill: var(--col-orange-0); }
            .st2 { fill: var(--col-orange-6); }
        }
    }

    &[data-variant=inactive] {
        .stroke {
            stroke: var(--col-gray-4);
        }

        &[data-direction=right] {
            .st0 { fill: var(--col-lt-gray-3); }
            .st1 { fill: var(--col-lt-gray-0); }
            .st2 { fill: var(--col-lt-gray-5); }
        }

        &[data-direction=left],
        &[data-direction=up] {
            .st0 { fill: var(--col-lt-gray-3); }
            .st1 { fill: var(--col-lt-gray-5); }
            .st2 { fill: var(--col-lt-gray-0); }
        }
    }
}

.stroke {
    stroke-width: 85px;
}
</style>