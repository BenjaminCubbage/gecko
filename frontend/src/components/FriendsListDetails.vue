<template>
    <div class="
        friends-list-details
        shdw shdw--otst-orange shdw--inst-lt-gray shdw--recessed">
        <h1
            class="
                details-username-tab
                shdw shdw--inst-lt-gray shdw--elevated-xs
                shdw-before shdw-before--inst-lt-gray
                shdw-after  shdw-after--inst-lt-gray">
            @Gary
        </h1>

        <p class="details-status">
            <template v-if="status === 'incoming'">
                <IconFriendQuestion class="status-icon" /> Wants to be friends
            </template>
            <template v-else-if="status === 'outgoing'">
                <IconFriendChevron class="status-icon" /> Friend request sent
            </template>
            <template v-else-if="status === 'active'">
                <IconFriendHeart class="status-icon" /> <span>Friends since <time class="status-date" datetime="02/26/2002">02/26/2002</time></span>
            </template>
        </p>

        <div
            class="
                details-buttons-tab
                shdw shdw--inst-lt-gray shdw--elevated-s"
            v-roving-container>
            <button
                v-if="status === 'incoming'" 
                class="
                    tab-btn
                    txtr-diag txtr-diag--green
                    shdw shdw--inst-green shdw--elevated-xs"
                v-roving-item>
                Accept
            </button>
            <button
                class="
                    tab-btn
                    txtr-diag txtr-diag--dk-red
                    shdw shdw--inst-dk-red shdw--elevated-xs"
                v-roving-item>
                <template      v-if="status === 'incoming'">Reject</template>
                <template v-else-if="status === 'outgoing'">Unsend</template>
                <template v-else-if="status === 'active'"  >Unfriend</template>
            </button>
        </div>
    </div>
</template>

<script setup>
import { ref }            from 'vue';
import IconFriendHeart    from './IconFriendHeart.vue';
import IconFriendChevron  from './IconFriendChevron.vue';
import IconFriendQuestion from './IconFriendQuestion.vue';

const status = ref('incoming');
</script>

<style scoped>
.friends-list-details {
    --ht-details:   110px;
    --ht-tab:       36px;
    --ht-tab-btn:   36px;
    --rd-tab-bevel: 9px;

    display:     grid;
    place-items: center;

    height: var(--ht-details);

    padding: 
        calc(var(--ht-tab) + 6px) 
        0 
        calc(var(--ht-tab-btn) + 4px);

    background: var(--col-lt-gray-1);

    border:        var(--border-s);
    border-radius: var(--radius-s);

    --shdw-etc:
        inset  17px  9px var(--col-lt-gray-4),
        inset  17px -9px var(--col-lt-gray-4),
        inset -17px  9px var(--col-lt-gray-4),
        inset -17px -9px var(--col-lt-gray-4);
}

.details-username-tab {
    position: absolute;
    top:      calc(-3 * var(--border-thickness-s));

    display:     flex;
    align-items: center;
    gap:         24px;

    height: var(--ht-tab);

    overflow: clip;

    border: var(--border-s);
    border-radius:
        var(--radius-s)
        var(--radius-s)
        var(--rd-tab-bevel)
        var(--rd-tab-bevel);
    corner-shape: notch notch bevel bevel;

    background: var(--col-lt-gray-3);

    line-height:         1.05;
    -webkit-text-stroke: var(--text-stroke-s);
    font-size:           2.8rem;
    letter-spacing:      0.03em;

    &::before,
    &::after {
        content: '';
        width:   36px;

        background:
            linear-gradient(var(--col-gray-4) 0 0) center / 3px 9px no-repeat,
            linear-gradient(var(--col-gray-4) 0 0) center / 9px 3px no-repeat
            var(--col-gray-2);
    }

    &::before { order: 0; border-right: var(--border-s); align-self: stretch; }
    &::after  { order: 1; border-left:  var(--border-s); align-self: stretch; }
}

.details-status {
    display:     flex;
    align-items: center;
    gap:         8px;

    font-family: var(--font-scnd);
    font-weight: bold;

    -webkit-text-stroke: var(--text-stroke-s);
    font-size:           1.65rem;
}

.status-icon {
    height:    20px;
    translate: 0 -1px;
}

.status-date {
    color: var(--col-green-9);
}

.details-buttons-tab {
    position: absolute;
    display:  flex;

    justify-content: end;
    place-items:     end center;

    bottom: calc(-4 * var(--border-thickness-s));
    height: var(--ht-tab);

    padding: 0 16px 4px;
    gap:     8px;

    background: var(--col-gray-2);

    border: var(--border-s);
    border-radius:
        var(--rd-tab-bevel)
        var(--rd-tab-bevel)
        var(--radius-s)
        var(--radius-s);
    corner-shape: bevel bevel notch notch;
}

.tab-btn {
    height: var(--ht-tab-btn);

    -webkit-text-stroke: var(--text-stroke-xs);
    font-size:           2.55rem;
    letter-spacing:      0.03em;

    padding: 0 24px;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    filter:
        drop-shadow(
            var(--shadow-dist-m)
            var(--shadow-dist-m)
            var(--col-shadow-alpha));
}
</style>