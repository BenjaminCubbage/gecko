<template>
    <svg
        class="icon-rocket"
        viewBox="0 0 155 155"
        :data-animating-launch="animatingLaunch"
        @animationend="animationEnded"
        @animationcancel="animationEnded">
        <g class="flame">
            <path d="M108.5 52.7V40.3h-6.2V21.7h-6.2v-6.2H58.9v6.2h-6.2v6.2h-6.2v12.4h-6.2v43.4h6.2v12.4h6.2v6.2h6.2v18.6h6.2v12.4h6.2v-6.2h6.2v6.2h6.2v6.2h6.2v-18.6h6.2v-6.2h6.2V89.9h6.2V77.5h6.2V52.7z" class="stroke" style="fill:#d84f19"/>
            <path d="M96.1 52.7V40.3h-6.2V27.9H65.1v6.2h-6.2v6.2h-6.2v43.4h6.2v6.2h6.2v12.4h6.2v12.4h18.6V89.9h6.2V77.5h6.2V52.7z" style="fill:#f2c50a"/>
            <path d="M89.9 58.9V46.5h-6.2v-6.2h-6.2v-6.2h-6.2v6.2h-6.2v6.2h-6.2v12.4h6.2v18.6h6.2v6.2h6.2v6.2h6.2V77.5h6.2v-6.2h6.2V58.9z" style="fill:#fffcee"/>
        </g>

        <g class="rocket">
            <path d="M124.9 123.4v-9h-4.5v-9.1h-4.6v-4.5H39.2v4.5h-4.6v9.1h-4.5v9h-4.5v18h4.5v4.5h4.5v-4.5h4.6v-9h4.5v-4.5h18v4.5h4.5v4.5h22.6v-4.5h4.5v-4.5h18v4.5h4.5v9h4.6v4.5h4.5v-4.5h4.5v-18z" class="st0 stroke"/>
            <path d="M39.2 100.8v4.5h-4.6v9.1h-4.5v9h-4.5v18h4.5v-13.5h4.5v-9h4.6v-9.1h4.5l.1-9z" class="st2"/>
            <path d="M106.3 46.7V35.1H48.7v11.6h-5.8v51.9h5.8v5.7h57.6v-5.8h5.8V46.7z" class="st5 stroke" />
            <path d="M60.2 35.1H48.7v11.6h-5.8v51.8h5.8v5.8-5.8h5.7V46.7h5.8z" class="st6" />
            <path d="M106.3 46.7V35.1H94.8v11.6h5.8V87h-5.8v5.8H42.9v5.8h5.8v5.7h57.6v-5.8h5.8V46.7z" class="st4" />
            <path d="M94.8 20.6v-5.8H89V9.1H66v5.8h-5.8v5.8h-5.8v11.5h46.1V20.6z" class="st2 stroke"/>
            <path d="M94.8 20.6H71.7v5.8H66v5.7h34.6V20.6z" class="st0"/>
            <path d="M124.9 123.4v-9h-4.5v-9.1h-4.6v-4.5h-4.5v18.1H88.7v4.5h-4.5v4.5H70.7v-4.5h-4.5v-4.5H34.7l-.1 9h-4.5v18h4.5v-4.5h4.6v-9h4.5v-4.5h18v4.5h4.5v4.5h22.6v-4.5h4.5v-4.5h18v4.5h4.5v9h4.6v4.5h4.5v-4.5h4.5v-18zm-41.6-97H89v5.7h11.6V20.6h-5.8v-5.8H89V9.1H77.5v11.5h5.8z" class="st1" />
        </g>
    </svg>
</template>

<script setup>
import { ref } from 'vue';

const animatingLaunch  = ref(false);
let   animationResolve = null;
let   animationPromise = null;

function animateLaunch() {
    if (animatingLaunch.value)
        return animationPromise;

    animatingLaunch.value = true;
    return (animationPromise = new Promise(resolve =>
        animationResolve = resolve
    ));
}

function animationEnded() {
    if (animatingLaunch.value) {
        animationResolve();
        animatingLaunch.value = false;
        animationResolve = null;
        animationPromise = null;
    }
}

defineExpose({ animateLaunch });
</script>

<style scoped>
.icon-rocket {
    overflow: visible;

    transform:
        perspective(200px) 
        rotateX(var(--rocket-wiggle-pitch));
}

.rocket,
.flame {
    transform-box:   fill-box;
    transform-origin: center;
}

.rocket {
    rotate: calc(var(--rocket-wiggle-rotation) * -1);
}

.flame {
    opacity:   var(--rocket-flame-opacity);
    scale:     var(--rocket-flame-scale-x) 1;
    translate: 0 80%;
    rotate:    var(--rocket-wiggle-rotation);
}

/*
    Animation
*/

.icon-rocket[data-animating-launch=true] {
    pointer-events: none;

    animation:
        rocket-launch      5000ms 1        cubic-bezier(.68,.01,.62,1) 500ms,
        rocket-wiggle      300ms  infinite steps(1, end),
        rocket-flame-scale 300ms  infinite steps(1, end);
}

@property --rocket-flame-scale-x {
    initial-value: 0;
    inherits:      true;
    syntax:        '<number>';
}

@property --rocket-flame-opacity {
    initial-value: 100%;
    inherits:      true;
    syntax:        '<percentage>';
}

@property --rocket-wiggle-rotation {
    initial-value: 0deg;
    inherits:      true;
    syntax:        '<angle>';
}

@property --rocket-wiggle-pitch {
    initial-value: 0deg;
    inherits:      true;
    syntax:        '<angle>';
}

@keyframes rocket-launch {
    0% {
        translate: 0;
    }

    90% {
        --rocket-flame-opacity: 100%;
        translate: 0 -1000px;
        scale:     1;
        animation-timing-function: steps(1);
    }

    90.1% {
        --rocket-flame-opacity: 0%;
        translate: 0;
        scale:     0;
    }

    100% {
        --rocket-flame-opacity: 0%;
        scale:     1;
        translate: 0;
    }
}

@keyframes rocket-flame-scale {
    0%, 100% { --rocket-flame-scale-x:  0.9; }
    50%      { --rocket-flame-scale-x: -1; }
}

@keyframes rocket-wiggle {
    0% {
        --rocket-wiggle-pitch:     10deg;
        --rocket-wiggle-rotation: 0deg;
    }

    25% {
        --rocket-wiggle-pitch:     0deg;
        --rocket-wiggle-rotation: -2deg;
    }

    75% {
        --rocket-wiggle-pitch:     -10deg;
        --rocket-wiggle-rotation: 2deg;
    }

    100% {
        --rocket-wiggle-pitch:     0deg;
        --rocket-wiggle-rotation: 0deg;
    }
}

/*
    Colors
*/

.stroke {
    stroke-width: 19.5px;
}

.st0 { fill: var(--col-red-3); }
.st1 { fill: var(--col-red-6); }
.st2 { fill: var(--col-red-0); }
.st4 { fill: var(--col-gray-3); }
.st5 { fill: var(--col-lt-gray-3); }
.st6 { fill: var(--col-lt-gray-0); }
</style>