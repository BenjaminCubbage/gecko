<template>
    <div
        class="
            friends-list-stage
            shdw shdw--inst-gray shdw--elevated-m">
        <div class="stage-items">
            <FriendsListStageItem class="friends-list-stage-item" />
            <FriendsListStageItem class="friends-list-stage-item" />
            <FriendsListStageItem class="friends-list-stage-item" />
            <FriendsListStageItem class="friends-list-stage-item" />
            <FriendsListStageItem class="friends-list-stage-item" />
        </div>
    </div>
</template>

<script setup>
import FriendsListStageItem from './FriendsListStageItem.vue';
</script>

<style scoped>
.friends-list-stage {
    --ht-stage-body:    42px;
    --ht-stage-content: 80px;

    position: relative;

    display:       grid;
    align-content: end;

    height:     var(--ht-stage-body);
    margin-top: calc(var(--ht-stage-content) - var(--ht-stage-body));

    background: var(--col-lt-gray-3);

    border:        var(--border-s);
    border-radius: var(--radius-s);
}

.stage-items {
    height: var(--ht-stage-content);

    display: flex;
    padding: 16px 20px 8px;
    gap:     20px;

    justify-content: space-evenly;

    align-items: end;
    & > :nth-child(even) {
        align-self: start;
    }
}
</style>