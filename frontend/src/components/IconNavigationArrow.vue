<template>
    <svg
        role="presentation"
        class="icon-navigation-arrow"
        viewBox="0 0 141.4 86.4">
        <template v-if="variant === 'left'">
            <path class="st0 stroke" d="M121.2 23v-5.1H95.9V23H70.6v5H60.5V17.9h-5.1v-5.1H45.3v5.1H35.2V23h-5.1v5H25
                v5.1h-5.1v5.1h-5.1v10.1h5.1v5.1H25v5.1h5.1v5.1h5.1v5.1h10.1v5.1h10.1v-5.1h5.1V58.6h10.1v5.1h25.3v5.1
                h25.3v-5.1h5.1V23.3h-5.1z"/>
            <path class="st1" d="M20.2 38.1V33h5.1v-5h5.1v-5h5.1v-5.1h10.1v-5.1h10.1v5.1h5.1V28h10.1v-5h25.3v-5.1h20.2
                v15.2h-5.1V28h-5.1v5.1H70.8v5.1H60.7v-5.1h-5.1V28H45.5v5.1H30.3v5.1h-5.1v15.2h-5.1v-5.1H15V38.2h5.2z" />
            <path class="st2" d="M30.3 48.2v5.1h5.1v5.1h10.1v5.1h10.1V48.3h15.2v5.1h25.3v5.1h15.2v-5.1h5.1V18h5.1v5
                h5.1v40.4h-5.1v5.1H96.2v-5.1H70.9v-5.1H60.8v10.1h-5.1v5.1H45.6v-5.1H35.5v-5.1h-5.1v-5.1h-5.1V48.1h5z" />
        </template>
        <template v-else>
            <path class="st0 stroke" d="M121.2 38.1V33h-5.1v-5H111v-5h-5.1v-5.1H95.8v-5.1H85.7v5.1h-5.1V28H70.5v-5
                H45.2v-5.1H19.9V23h-5.1v40.4h5.1v5.1h25.3v-5.1h25.3v-5.1h10.1v10.1h5.1v5.1h10.1v-5.1h10.1
                v-5.1h5.1v-5.1h5.1v-5.1h5.1V48h5.1V37.9h-5.1z" />
            <path class="st1" d="M111.1 23H106v-5.1H95.9v-5.1H85.8v5.1h-5.1V28H70.6v-5H45.3v-5.1H20V23h-5.1
                v40.4H20v-5.1h5.1V33h5.1v-5h5.1v5.1h35.4v5.1h10.1v-5.1h5.1V28H96v5.1h15.2v5.1h5.1V28.1h-5.1V23z" />
            <path class="st2" d="M121.2 33.1h-5.1v15.2H111v5.1h-5.1v5.1H95.8v5.1H85.7V48.4H70.5v5.1H45.2v5.1H19.9
                v10.1h25.3v-5.1h25.3v-5.1h10.1v10.1h5.1v5.1h10.1v-5.1h10.1v-5.1h5.1v-5.1h5.1v-5.1h5.1v-5.1h5.1V38.1h-5.1z" />
        </template>
    </svg>
</template>

<script setup>
defineProps({
    variant: {
        type:    String,
        required: true,
        validator(value) {
            return [
                'left',
                'right'
            ].includes(value);
        }
    }
})
</script>

<style scoped>
.icon-navigation-arrow {
    > .st0{ fill:var(--col-orange-3); }
    > .st1{ fill:var(--col-orange-0); }
    > .st2{ fill:var(--col-orange-6); }

    > .stroke {
        stroke-width: 15.5px;
    }
}
</style>