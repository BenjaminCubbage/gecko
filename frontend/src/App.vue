<template>
    <div class="bg">
        <div class="dotted">
            <div class="front-and-center">
                <RecipientSelect />
                <PicEditor />
            </div>

            <div class="header-strip-container">
                <AccountWidget />
            </div>
        </div>
    </div>
</template>

<script setup>
import { ref, provide } from 'vue';
import PicEditor from './components/pic_editor/PicEditor.vue';
import RecipientSelect from './components/recipient_select/RecipientSelect.vue';
import AccountWidget from './components/account_widget/AccountWidget.vue';
import { ActiveUser } from '@/core/session/ActiveUser.js';
import { Dispatch } from '@/core/dispatch/Dispatch.js';
import { Cookies } from '@/core/storage/Cookies.js';
import { Session } from '@/core/session/Session.js';

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
    
setTimeout(() => {
    Dispatch.Get_UserByUsername("johntron")
        .onSuccess(body => console.log(body))
        .onHttpError(body => console.log(body));
}, 1000);

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
        display: grid;

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

    .header-strip-container, .front-and-center { grid-area: 1/1; }

    .header-strip-container {
        place-self: start;
        pointer-events: none;
    }

    :where(.header-strip-container > *) {
        pointer-events: all;
    }

    .front-and-center {
        display: flex;
        flex-flow: column nowrap;
        gap: 4px;
        place-self: center;
    }
</style>
