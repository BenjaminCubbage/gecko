<template>
    <div class="bg">
        <div class="dotted">
            <div class="header-strip-container">
                <Navbar class="navigation-bar" @selectedTabChanged="v => selectedTab = v" />
            </div>

            <div class="front-and-center" v-show="selectedTab == 'canvas'">
                <RecipientSelect style="margin-bottom: 4px;" />
                <PicEditor />
            </div>
        </div>
    </div>
</template>

<script setup>
import { ref, provide } from 'vue';
import PicEditor from './components/pic_editor/PicEditor.vue';
import RecipientSelect from './components/recipient_select/RecipientSelect.vue';
import Navbar from './components/navbar/Navbar.vue';
import { ActiveUser } from '@/core/session/ActiveUser.js';
import { Dispatch } from '@/core/dispatch/Dispatch.js';
import { Cookies } from '@/core/storage/Cookies.js';
import { Session } from '@/core/session/Session.js';

const selectedTab = ref('canvas');

const session = ref(new Session());
const xsrfToken = () => Cookies.byName('__Host-xsrf_token');
session.value.setXSRFCookie(xsrfToken());

if (!session.value.xsrfCookie())
    Dispatch.Get_XSRF()
        .onSuccess(() => session.value.setXSRFCookie(xsrfToken()))
        .onNetworkError(() => console.warn('Couldn\'t GET XSRF token: server didn\'t respond.'))
        .onHttpError((body, status) => console.warn(`Couldn't GET XSRF Token. code: ${status} body: ${body}`));

Dispatch.Get_UsersMe()
    .onSuccess(body => session.value.setActiveUser(new ActiveUser(body['user'])))
    .onNetworkError(() => console.warn('Couldn\'t GET /users/me: server didn\'t respond'));

provide('session', session);
</script>

<style scoped>
    .bg {
        min-height: 100%;
        background: #0a7bff;
        display: grid;
        place-items: stretch;
    }

    .dotted {
        height: 100%;
        --dot-color: #0756b2;
        background-image: radial-gradient(circle at top,    var(--dot-color) 2px, transparent 2px),
                          radial-gradient(circle at bottom, var(--dot-color) 2px, transparent 2px),
                          radial-gradient(circle at right,  var(--dot-color) 2px, transparent 2px),
                          radial-gradient(circle at left,   var(--dot-color) 2px, transparent 2px);

        background-size: 32px 32px;
        background-repeat: repeat;
        background-position-x: 50%;
        background-position-y: 50%;
    }

    .navigation-bar {
        align-self: start;
    }

    .front-and-center {
        display: flex;
        flex-flow: column nowrap;
        gap: 4px;
        place-self: center;
        place-content: center;
        place-items: center;
    }

    .swap-screens-enter-active,
    .swap-screens-leave-active {
        transition: transform 120ms ease, opacity 120ms ease;
    }

    .swap-screens-enter-from {
        transform: translateX(-5px);
        opacity: 0;
    }

    .swap-screens-leave-to {
        transform: translateX(5px);
        opacity: 0;
    }
</style>
