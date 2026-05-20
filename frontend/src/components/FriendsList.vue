<template>
    <div class="friends-list">
        <div
            class="
                list-stage
                shdw shdw--inst-gray shdw--elevated-m">
            <div class="stage-items">
                <FriendsListStageItem class="friends-list-stage-item" />
                <FriendsListStageItem class="friends-list-stage-item" />
                <FriendsListStageItem class="friends-list-stage-item" />
                <FriendsListStageItem class="friends-list-stage-item" />
                <FriendsListStageItem class="friends-list-stage-item" />
            </div>
        </div>

        <div
            class="
                list-border
                txtr-vert txtr-vert--orange
                shdw shdw--inst-orange shdw--elevated-l">
            <FriendsListDetails class="stage-details" />
            <FriendsListView class="friends-list-view" style="width: 480px;" />
        </div>
    </div>
</template>

<script setup>
import FriendsListStageItem from './FriendsListStageItem.vue';
import FriendsListDetails   from './FriendsListDetails.vue';
import FriendsListView      from './FriendsListView.vue';

import IconFriendRequestArrow from './IconFriendRequestArrow.vue';
</script>

<style scoped>
.friends-list {
    display:   flex;
    flex-flow: column;

    & > .list-stage  { z-index: 1; place-self: center  stretch; }
    & > .list-border { z-index: 0; place-self: stretch stretch; }
}

.list-stage {
    isolation: isolate;
    position:  relative;

    display:       grid;
    align-content: end;

    height: 46px;

    background: var(--col-lt-gray-3);

    border:        var(--border-s);
    border-radius: var(--radius-s);

    margin: 40px 0 -20px;
}

.list-border {
    display:   flex;
    flex-flow: column;
    gap:       18px;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    padding: 30px 12px 12px;
}

.stage-items {
    height: 80px;

    display: flex;
    padding: 16px 26px 8px;
    gap:     20px;

    justify-content: space-evenly;

    align-items: end;
    & > :nth-child(even of .friends-list-stage-item) {
        align-self: start;
    }
}

.stage-view {
    margin: 15px;
}
</style>