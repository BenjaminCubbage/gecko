<template>
    <div
        popover
        id="account-widget-profile-button-popover"
        class="account-widget-profile-button-popover">
        <AccountWidgetLogOutButton @click="emit('logout')" />
    </div>

    <!-- + Sibling selector -->

    <div class="account-widget-profile-button">
        <button
            popovertarget="account-widget-profile-button-popover"
            class="inner-button txtr-diag txtr-diag--green">
            <i class="inner-button-icon hn hn-user-solid"></i>
        </button>
    </div>
</template>

<script setup>
import AccountWidgetLogOutButton from './AccountWidgetLogOutButton.vue';

const emit = defineEmits([
    'logout'
]);
</script>

<style scoped>
.account-widget-profile-button {
    anchor-name: --options-anchor;

    display: grid;
}

.account-widget-profile-button-popover {
    position-anchor: --options-anchor;
    position-area:   bottom span-right;

    padding: 0;
    left:    1px;
    top:     7px;
    overflow: visible;

    background: transparent;
    border:     0;

    transition:
        display   120ms allow-discrete,
        opacity   120ms ease,
        transform 120ms 20ms ease;
}

.account-widget-profile-button-popover:not(:popover-open) {
    transform: translateY(-2px);
    opacity: 0;
}

.inner-button {
    display:       grid;
    place-content: center;
    width:         48px;

    --inner-button-aura:   0 0;
    --inner-button-offset: 0px;

    box-shadow:
        var(--inner-button-aura),
        calc(var(--shadow-dist-s) - var(--inner-button-offset))
        calc(var(--shadow-dist-s) - var(--inner-button-offset))
        0 black,
        inset 0  3px var(--col-green-0),
        inset 0 -3px var(--col-green-5);

    border-radius: var(--radius-s);
    border: var(--border-s);

    transform:
        translate(
            var(--inner-button-offset),
            var(--inner-button-offset));

    transition:
        box-shadow 50ms  ease,
        transform  100ms ease;
}

.inner-button:hover {
    --inner-button-offset: calc(var(--shadow-dist-s) / 2);
}

.account-widget-profile-button-popover:popover-open + .account-widget-profile-button > .inner-button,
.inner-button:active {
    --inner-button-aura:   var(--shadow-aura);
    --inner-button-offset: var(--shadow-dist-s);
}

.inner-button-icon {
    margin-left: 2.5px;
    color:       black;
    font-size:   2.4rem;
    line-height: 1em;

    -webkit-text-stroke: white 4px;
    paint-order: stroke;
}

@starting-style {
    .account-widget-profile-button-popover {
        opacity:   0%;
    }
}
</style>