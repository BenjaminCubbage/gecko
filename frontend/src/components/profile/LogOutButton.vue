<template>
    <button 
        class="logout-button"
        @click="logout">
        Log Out
    </button>
</template>

<script setup>
    import { inject } from 'vue';
    import { Dispatch } from '@/core/dispatch/Dispatch.js';

    const session = inject('session');

    function logout() {
        Dispatch.Post_LogOut(session.value)
            .onSuccess(() => session.value.setActiveUser(null))
            .onHttpError((body, status) => `Couldn't POST_LogOut. code: ${status} body: ${body}`);
    }
</script>

<style>
    .logout-button {
        font-family: var(--font-heading);
        font-size: 2.2rem;
        padding: 0 12px;
        margin: 4px 0;
        background: hsl(0, 100%, 71%);
        border: var(--border-small);
        border: 2.5px solid black;
        box-shadow: 2px 2px 0 0 black;
        border-radius: 999px;
        display: flex;
        justify-content: center;
        align-items: center;
        
        transition: transform 200ms ease;
        transform: scale(1) rotate(0deg);
    }

    .logout-button:hover {
        transform: scale(1.02) rotate(-1deg);
        box-shadow: 2px 2px 0 0 black;
    }

    .logout-button:active {
        background: hsl(0, 86%, 67%);
        transform: scale(0.98) rotate(0deg);
        box-shadow: 1px 1px 0 0 black;
    }
</style>