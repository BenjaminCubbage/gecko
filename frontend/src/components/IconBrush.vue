<template>
    <svg viewBox="0 0 155 155">
        <g class="brush">
            <path class="st1 stroke" d="M86.3 78.2v-5.4H37.7v-5.4h-5.4V62h-5.4V45.8h10.8V29.6H16.1V35h-5.4v32.4h5.4v5.4h5.4v5.4h5.4v5.4h5.4V89h43.2v10.8h16.2V78.2z" />
            <path class="st2" d="M86.5 78.2v21.6h5.4V78.2zm-5.4-5.4H37.9v5.4h48.6v-5.4zm-48.6-5.4h5.4v5.4h-5.4zm5.4-21.6v-5.4H27.1v5.4zM27.1 62h5.4v5.4h-5.4zm0-10.8v-5.4h-5.4V62h5.4z" />
            <path class="st0" d="M75.7 89v10.8h5.4V89zm-5.4-5.4H32.5V89h43.2v-5.4zm-43.5-5.4h5.4v5.4h-5.4zm-5.4-5.4h5.4v5.4h-5.4zM27.1 35h10.8v-5.4H16.3V35zM16 67.4h5.4v5.4H16z" />
            <path class="st3 stroke" d="M91.9 107.8H75.7v5.4h-5.4v27h5.4v5.4h16.2v-5.4h5.4v-27h-5.4z" />
            <path class="st4 stroke" d="M139 20.2v-5.4h-5.4V9.4H49.3v5.4h-5.4v5.4h-5.4v27h5.4v5.4h5.4V58h84.3v-5.4h5.4v-5.4h5.4v-27z" />
            <path class="st5" d="M91.9 113.2V124h-5.4v10.8h-5.4v5.4h-5.4v5.4h16.2v-5.4h5.4v-27z" />
            <path class="st6" d="M86.5 107.8H75.7v5.4h-5.4v27h5.4V124h5.4v-10.8h10.8v-5.4z" />
            <path class="st7" d="M139 20.2v-5.4h-5.4v27h-5.4v5.4H54.7v5.4h-5.4V58h84.3v-5.4h5.4v-5.4h5.4v-27z" />
            <path class="st8" d="M128.2 9.4H49.3v5.4h-5.4v5.4h-5.4v27h5.4v5.4h10.8V41.8h-5.4V25.6h5.4v-5.4h78.9V9.4z" />
            <path class="st0" d="M16 56.6V35h-5.4v32.4H16z" />
        </g>
    </svg>
</template>

<script setup>
defineProps({
    size: {
        type:    String,
        default: 'medium',
        validator(value) {
            return [
                'small',
                'medium',
                'large'
            ].includes(value);
        }
    }
})
</script>

<style scoped>
.stroke {
    stroke:       black;
    stroke-width: 20px;
}

.st0 { fill: #eff3f7; }
.st1 { fill: #eaecf1; }
.st2 { fill: #8e94ac; }
.st3 { fill: #bc9c62; }
.st4 { fill: #ffc928; }
.st5 { fill: #846320; }
.st6 { fill: #fce2be; }
.st7 { fill: #d99d12; }
.st8 { fill: #fffcee; }
.st9 { fill: #e5a81f; }

.brush {
    transform-origin: 50% 100%;
    scale: 0.98;
}
</style>