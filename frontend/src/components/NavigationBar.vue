<template>
    <header class="navigation-bar">
        <div class="account">
            <AccountWidget />
        </div>

        <div class="navigation">
            <NavigationBarTabs v-model="selectedTab" class="navigation-bar-tabs" />
        </div>
    </header>
</template>

<script setup>
import AccountWidget     from './AccountWidget.vue';
import NavigationBarTabs from './NavigationBarTabs.vue';

const selectedTab = defineModel('selectedTab');
</script>

<style scoped>
.navigation-bar {
    padding:       0 3px;
    margin-bottom: 16px;

    grid-template:
        "account links ." 80px /
         1fr     auto  1fr;

    align-items: center;
    display:     grid;

    @media (560px <= width < 930px) {
        grid-template: 
            "account . links" 70px /
             auto    1fr auto;
        padding: var(--vp-margin);
    }

    @media (width < 560px) {
        grid-template: 
            "account" auto
            "links"   70px /
             1fr;
        gap: 6px;
        padding: var(--vp-margin);
    }
}

.account {
    grid-area: account;
}

.navigation {
    grid-area:    links;
    place-self: center;
}
</style>