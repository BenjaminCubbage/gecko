<template>
    <div class="friends-list">
        <FriendsListStage class="list-stage" />

        <div
            class="
                list-border
                txtr-vert txtr-vert--orange
                shdw shdw--inst-orange shdw--elevated-l">
            <FriendsListDetails class="border-details" />
            <FriendsListView
                class="border-list-view"
                :friends="friends"
                v-model:selected-friend="selectedFriend" />
        </div>

        <FriendsListPageSelect class="list-page-select" />
    </div>
</template>

<script setup>
import { ref } from 'vue';

import FriendsListStage      from './FriendsListStage.vue';
import FriendsListDetails    from './FriendsListDetails.vue';
import FriendsListView       from './FriendsListView.vue';
import FriendsListPageSelect from './FriendsListPageSelect.vue';

import { Friend, FriendStatus } from '@/models/friend.js';

const friends = ref([
    Friend.fromJSON({
        'user': {
            'username': 'Bob',
            'user_id':  1
        },
        'accepted_on': '12/21/2002'
    }, FriendStatus.PendingIncoming),
    Friend.fromJSON({
        'user': {
            'username': 'Jim',
            'user_id':  1
        },
        'accepted_on': '12/21/2002'
    }, FriendStatus.PendingOutgoing),
    Friend.fromJSON({
        'user': {
            'username': 'Larry',
            'user_id':  1
        },
        'accepted_on': '12/21/2002'
    }, FriendStatus.Active),
    null,
    null
]);

const selectedFriend = ref(friends.value[0]);
</script>

<style scoped>
.friends-list {
    display:   flex;
    flex-flow: column;

    width: 520px;

    & > .list-stage       { z-index: 1; place-self: center  stretch; }
    & > .list-border      { z-index: 0; place-self: stretch stretch; }
    & > .list-page-select { z-index: 1; place-self: center; }
}

.list-stage {
    margin-bottom: -20px;
}

.list-border {
    display:   flex;
    flex-flow: column;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    padding: 30px 12px 20px;

    & > .border-details     { align-self: stretch; }
    & > .border-list-view   { align-self: stretch; margin-top: 18px; }
}

.list-page-select {
    margin-top: -16px;
}

.stage-view {
    margin: 15px;
}
</style>