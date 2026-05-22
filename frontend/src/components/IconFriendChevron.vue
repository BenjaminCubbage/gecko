<template>
    <svg viewBox="0 0 256 256">
        <path class="st0 stroke" d="M192 115.2V89.6h-25.6V64h-25.6
            V38.4H38.4v51.2H64v25.6h25.6v25.6H64v25.6H38.4v51.2
            h102.4V192h25.6v-25.6H192v-25.6h25.6v-25.6z" />
        <path class="st1" d="M38.4 192v25.6h102.4V192zM89.6 89.6v25.6H64
            V89.6zM192 140.8v25.6h-25.6v-25.6zM166.4 166.4V192h-25.6v-25.6z
            M192 89.6v25.6h-25.6V89.6zM217.6 115.2v25.6H192v-25.6z" />
        <path class="st2" d="M38.4 38.4v51.2H64V64h76.8V38.4zM64 166.4
            V192H38.4v-25.6zM89.6 140.8v25.6H64v-25.6zM166.4 64v25.6h-25.6V64z" />
    </svg>
</template>

<script setup>
defineProps({
    variant: {
        type:    String,
        default: 'incoming',
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
.stroke {
    stroke-width: 78px;
}

.st0 { fill: var(--col-green-3); }
.st1 { fill: var(--col-green-5); }
.st2 { fill: var(--col-green-0); }
</style>