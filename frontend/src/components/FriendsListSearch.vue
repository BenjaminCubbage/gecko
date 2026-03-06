<template>
    <div v-if="session.state.value === 'ready'">
        test
    </div>
</template>

<script setup>
import {
    inject,
    ref,
    watch
} from 'vue';

import { Keys } from '@/core/di/keys.js';

const session = inject(Keys.SessionStore);
const friends = inject(Keys.FriendsStore);
</script>

<style scoped>

</style>