<template>
    <svg class="icon-trash" viewBox="0 0 155 155">
        <g
            class="trash-bin"
            :data-animate-shaking="animateShaking"
            @animationend="animateShaking = false"
            @animationcancel="animateShaking = false">
            <g class="lid">
                <path d="M118.3 24.6V19H84.8v-5.6H56.9V19H23.4v5.6h-5.6v16.8H124V24.6z" class="st0 stroke"/>
                <path d="M112.7 24.6H95.9V19H84.7v5.6H62.3V19h-5.6v5.6h-5.6v5.6H28.7v5.6h89.4V24.6zM68 13.4h5.6V19H68z" class="st1"/>
                <path d="M118.3 24.6v11.2h-95v5.6h100.6V24.6zM73.5 24.6h11.2V13.4H73.5V19H62.3v5.6z" class="st2" />
            </g>
            <path d="M107.1 52.2H23.3v22.4h5.6v27.9h5.6V136H40v5.6h61.5V136h5.6v-33.5h5.6V74.6h5.6V52.2z" class="st2 stroke"/>
            <path d="M40 68.9h-5.6V57.7H23.2v16.8h5.6v27.9h5.6v33.5h11.2V96.8H40z" class="st0"/>
            <path d="M95.9 57.8H34.4V69H40v27.9h5.6v33.5h50.3V96.9h5.6V69h5.6V57.8zm-30 64.2H54.7V66.2h11.2zm20.9 0H75.6V66.2h11.2z" class="st1"/>
        </g>
    </svg>
</template>

<script setup>
import { ref } from 'vue';
const animateShaking = ref(false);
defineExpose({
    animateShaking() {
        animateShaking.value = true;
    }
});
</script>

<style scoped>
.icon-trash {
    overflow: visible;
}

.trash-bin {
    translate: 4% 2%;
    rotate:    calc(var(--shaking-degrees) * -0.5);
}

.lid {
    translate: 0 -2.5%;
    rotate:    var(--shaking-degrees);
}

/*
    Animation
*/

.trash-bin[data-animate-shaking=true] {
    animation: shaking 300ms 1 forwards steps(2, start);
}

@property --shaking-degrees {
    inherits:      true;
    initial-value: 0deg;
    syntax:        '<angle>';
}

@keyframes shaking {
    0%   { --shaking-degrees:   0deg; }
    50%  { --shaking-degrees: -10deg; }
    100% { --shaking-degrees:  10deg; }
}

/*
    Colors
*/

.stroke {
    stroke-width: 20.5px;
}

.st0{ fill: #ffffff; }
.st1{ fill: #bfc5d1; }
.st2{ fill: #8e94ac; }
.st3{ fill: #ffdfdf; }
.st5{ fill: #b60b2d; }
.st6{ fill: #fd4d4d; }
</style>