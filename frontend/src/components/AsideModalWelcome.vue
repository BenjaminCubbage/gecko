<template>
    <AsideModal title="Welcome!">
        <template #icon>
            <IconLightBulb height="47px" />
        </template>

        <template #content>Hey. You!
<div style="height:8px;"></div>This site lets my friends send images to my E-ink board, and vice-versa.
<div style="height:8px;"></div>If you know me, log in and send a friend request to <span class="at-ben">@ben</span>. My device should appear as a recipient once I accept.</template>

        <template #buttons>
            <div class="buttons">
                <button class="
                    button button--log-in
                    shdw shdw--inst-green shdw--elevated-s
                    txtr-diag txtr-diag--green"
                    type="button"
                    role="link"
                    @click="emit('logIn')">
                    Log In
                </button>

                <span class="or" aria-hidden="true">or</span>

                <button class="
                    button button--dismiss
                    shdw shdw--inst-lt-gray shdw--elevated-s
                    txtr-vert txtr-vert--lt-gray"
                    type="button"
                    @click="emit('dismiss')">
                    Dismiss
                </button>
            </div>
        </template>
    </AsideModal>
</template>

<script setup>
import AsideModal    from './AsideModal.vue';
import IconLightBulb from './IconLightBulb.vue';
import IconFriendUser from './IconFriendUser.vue';

const emit = defineEmits([
    'logIn',
    'dismiss'
]);
</script>

<style scoped>
.buttons {
    display: grid;
    grid-template:
        "left middle right" auto /
         1fr  auto   1fr;

    align-items: center;
    gap:         14px;

    & > .button--log-in  { grid-area: left; }
    & > .or              { grid-area: middle; }
    & > .button--dismiss { grid-area: right;}
}

.button {
    --hovering-filter: brightness(1);

    padding:    5px 0 6px;
    margin-top: var(--shadow-dist-s);

    -webkit-text-stroke: var(--text-stroke-s);
    font-size:           2.4rem;
    letter-spacing:      0.05em;
    line-height:         0.8;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    filter:
        var(--hovering-filter)
        drop-shadow(
            var(--shadow-dist-m) 
            var(--shadow-dist-m) 
            var(--col-lt-gray-6));

    &:hover,
    &:active {
        --hovering-filter: var(--filter-hl-0);
    }

    &:active {
        --shdw-dist-elevation: 0;
        translate:             0 var(--shadow-dist-s);
    }
}

.or {
    margin-block-start: var(--shadow-dist-s);
    padding-block-end:  1.5px;

    font-size:   2rem;
    color:       var(--col-gray-4);
    line-height: 0.7;

    border:       var(--border-thickness-s) solid currentColor;
    border-left:  0;
    border-right: 0;
}

.at-ben {
    contain:       none;
    display:       inline-block;
    margin-inline: 1px;
    color:         #38640C;
}
</style>