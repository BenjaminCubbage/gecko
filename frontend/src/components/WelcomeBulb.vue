<template>
    <button 
        class="welcome-bulb efct-rays-before"
        aria-label="Open welcome dialog"
        :data-is-urgent="session.state.value === 'loggedout'"
        :data-is-open="isModalOpen"
        @click="clicked">
        <IconLightBulb 
            class="icon-light-bulb" 
            :variant="isModalOpen ? 'outline' : 'normal'"
            height="100%"
            inert />
    </button>

    <ModalWelcome 
        v-model:is-open="isModalOpen"
        :variant="
            session.state.value === 'loggedout'
                ? 'loggedout'
                : 'loggedin'"
        :is-logging-in="isLoggingIn"
        @log-in="logIn" />
</template>

<script setup>
import {
    inject,
    ref
} from 'vue';

import ModalWelcome  from './ModalWelcome.vue';
import IconLightBulb from './IconLightBulb.vue';
import { Keys }      from '@/core/di/keys.js';

const session  = inject(Keys.SessionStore);
const snackBar = inject(Keys.SnackBarStore);

const isModalOpen = ref(false);
const isLoggingIn = ref(false);

function clicked() {
    isModalOpen.value = true;
}

function logIn() {
    isLoggingIn.value = true;

    if (!session.requestLogIn()) {
        snackBar.pushMessage('Already logged in!');
        isLoggingIn.value = false;
    }
}
</script>

<style scoped>
.welcome-bulb {
    position: relative;

    line-height:  0;
    aspect-ratio: 1 / 1;
    
    width:  50px;
    height: 50px;

    @media (hover: hover) {
        &:hover {
            &::before {
                --rays-alpha: 35%;
            }

            & > .icon-light-bulb {
                scale: 1.05;
            }
        }
    }

    /*
        ::before: Sun rays
        ::after:  Exclamation marks 
    */

    &:not([data-is-urgent=true]),
    &[data-is-open=true] {
        &::after {
            display: none;
        }
    }

    &:not([data-is-urgent=true]) {
        &::before {
            opacity: 0.5;
        }
    }

    &[data-is-open=true] {
        &::before {
            opacity: 0;
        }
    }

    &::before {
        --rays-alpha: 15%;
    }

    &::after {
        content: '!!';

        position: absolute;
        top:      0;
        right:    -3px;

        display:       grid;
        place-content: center;

        font-size:           2.5rem;
        letter-spacing:      0.1em;
        -webkit-text-stroke: var(--text-stroke-s);

        width:  20px;
        height: 20px;

        border-radius: var(--radius-s);
    }
}

.icon-light-bulb {
    translate: 0 4px;

    filter: drop-shadow(
        var(--shadow-dist-m)
        var(--shadow-dist-m)
        var(--col-shadow-alpha));
}
</style>