<template>
    <nav class="navbar">
        <AccountWidget class="account" />
        <NavbarTabs v-model="tabSelection" class="pill" />
    </nav>
</template>

<script setup>
import { ref, watch } from 'vue';
import AccountWidget from '@/components/account_widget/AccountWidget.vue';
import NavbarTabs from './NavbarTabs.vue';

const emit = defineEmits([
    // See NavbarTabs.vue
    'selectedTabChanged'
]);

const tabSelection = ref('canvas');

watch(tabSelection, () => emit('selectedTabChanged', tabSelection.value), { immedate: true });
</script>

<style scoped>
    .navbar {
        display: grid;
        align-items: center;
        height: 96px;
    }

    .account, .pill { grid-area: 1/1; }

    .account {
        justify-self: left;
    }

    .pill {
        justify-self: center;
        align-self:center
    }
</style>