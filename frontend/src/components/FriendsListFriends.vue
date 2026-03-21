<template>
    <div class="friends-list-friends">
        <ul v-roving-container>
            <li
                v-for="friend in friends.pendingIncoming"
                :key="friend.user.userID"
                v-roving-item>
                <FriendsListFriendsCard
                    :friend="friend"
                    friend-type="incoming" />
            </li>
        </ul>

        <div class="
            list-border 
            txtr-diag txtr-diag--orange 
            shdw shdw--inst-orange shdw--elevated-l">
            <ul class="
                list 
                shdw shdw--recessed shdw--otst-orange shdw--inst-white"
                v-roving-container>
                <li
                    v-for="friend in friends.pendingOutgoing"
                    :key="friend.user.userID">
                    <FriendsListFriendsCard
                        :friend="friend"
                        friend-type="outgoing"
                        v-roving-item />
                </li>

                <li
                    v-for="friend in friends.activeFriends"
                    :key="friend.user.userID"
                    class="list-item">
                    <FriendsListFriendsCard
                        :friend="friend"
                        friend-type="active"
                        v-roving-item />
                </li>
            </ul>
        </div>
    </div>
</template>

<script setup>
import { inject }             from 'vue';
import FriendsListFriendsCard from './FriendsListFriendsCard.vue';
import { Keys }               from '@/core/di/keys.js';
const session = inject(Keys.SessionStore);
const friends = inject(Keys.FriendsStore);
</script>

<style scoped>
.friends-list-friends {
    display:        flex;
    flex-direction: column;
}

.list-border {
    padding: 9px;

    border:        var(--border-s);
    border-radius: var(--radius-s);
}

.list {
    display:        flex;
    flex-direction: column;
    overflow:       auto;
    padding:        6px;
    gap:            3px;

    background:    white;
    border-radius: var(--radius-s);
    border:        var(--border-s);
}
</style>