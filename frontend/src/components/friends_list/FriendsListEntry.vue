<template>
    <div
        class="friends-list-entry"
        :class="{
            'selectable': !isSearchResult && (isActive || isPendingOut),
            'selected':   !isSearchResult && (isActive || isPendingOut) && isFocused
        }"
        tabindex="-1"
        @focus="isFocused = true"
        @focusout="isFocused = false">
        <div class="sections">
            <div class="icon-section">
                <i class="hn hn-user-solid"></i>
            </div>

            <div class="name-section">
                @<slot name="username" :user="user" />
            </div>

            <div class="subheading-section">
                <span v-if="isActive"          class="subh-label subh-label--joined"     >Friends Since {{ acceptedOn }}</span>
                <span v-else-if="isPendingIn"  class="subh-label subh-label--pending-in" >Requests Friend</span>
                <span v-else-if="isPendingOut" class="subh-label subh-label--pending-out">Request Sent</span>
                <span v-else-if="isNotFriends" class="subh-label subh-label--not-friends">Not Friends</span>
                <span v-else-if="isMe"         class="subh-label subh-label--me"         >Me</span>
            </div>

            <div class="buttons-section">
                <button class="shiny-button shiny-button--send-req"
                    v-if="isNotFriends"
                    @focus="isFocused = true"
                    @blur="isFocused = false"
                    @click="emit('sendRequest', user)">
                    SEND REQUEST
                </button>

                <button class="shiny-button shiny-button--accept-req"
                    v-if="isPendingIn"
                    @click="emit('acceptRequest', user)">
                    ACCEPT
                </button>

                <button class="shiny-button shiny-button--remove"
                    v-if="isActive || isPendingOut || isPendingIn"
                    :style="{ 'visibility': isSearchResult ? 'hidden' : 'visible' }"
                    @focus="isFocused = true"
                    @blur="isFocused = false"
                    @click="emit('delete', user)">
                    <i class="hn hn-times-solid"></i>
                </button>
            </div>
        </div>
    </div>
</template>

<script setup>
import { ref, computed, } from 'vue';

const props = defineProps({
    // 'notfriends' | 'pendingin' | 'pendingout' | 'active' | 'me'
    entryType:      { type: String, default: 'active' },
    user:           { type: Object, required: true },
    isSearchResult: { type: Boolean, default: false },
    acceptedOn:     { type: String, required: false }
});

const emit = defineEmits([
    'sendRequest',
    'acceptRequest',
    'delete'
]);

const isFocused = ref(false);

const isNotFriends = computed(() => props.entryType == 'notfriends');
const isPendingIn  = computed(() => props.entryType == 'pendingin');
const isPendingOut = computed(() => props.entryType == 'pendingout');
const isActive     = computed(() => props.entryType == 'active');
const isMe         = computed(() => props.entryType == 'me');
</script>

<style scoped>
.friends-list-entry {
    height:  68px;
    padding: 12px 24px 12px 18px;

    color:          black;
    font-family:    var(--font-heading);
    font-size:      2.2rem;
    letter-spacing: 0.04em;
    line-height:    1;
}

.sections {
    grid-template-areas:
        "icon name               buttons"
        "icon subheading-section buttons";

    display:               grid;
    grid-template-columns: auto 1fr;
}

.icon-section {
    grid-area:    icon;
    line-height:  0.87;
    margin-right: 12px;
    place-self:   center;
    text-align:   center;

    font-size: 2.4rem;

    background:    rgb(255, 170, 105);
    border:        var(--border-s);
    border-radius: 5px;
    box-shadow:    var(--shadow-s), inset 0 3px 0 rgb(255, 202, 162);
    padding:       6px;
}

.name-section {
    grid-area: name;
    height:    100%; /*???*/
}

.subheading-section {
    grid-area: subheading-section;
}

.buttons-section {
    display:    flex;
    flex-flow:  row nowrap;
    gap:        8px;
    grid-area:  buttons;
    place-self: center;
}

.shiny-button {
    height: 32px;

    /* Set these */
    --shiny-button-background:  white;
    --shiny-button-top-shadow:  white;

    color:          black;
    font-size:      2rem;
    text-transform: uppercase;

    background:    var(--shiny-button-background);
    border:        var(--border-s);
    border-radius: var(--radius-s);
    box-shadow:    var(--shadow-s), inset 0 2px 0 var(--shiny-button-top-shadow);
    padding:       4px 12px;

    transition: transform 80ms ease;
}

.shiny-button--remove {
    /* Set these */
    --shiny-button-background:  #ff826c;
    --shiny-button-top-shadow:  #ffb6aa;

    opacity:        0;
    pointer-events: none;

    font-size: 1.8rem;

    /* No extra side padding */
    padding: 4px;
}

.shiny-button--send-req {
    /* Set these */
    --shiny-button-background:  #8dacff;
    --shiny-button-top-shadow:  #afc4ff;
}

.shiny-button--accept-req {
    /* Set these */
    --shiny-button-background:  #91df43;
    --shiny-button-top-shadow:  #adf467;
}

.subh-label {
    color: #1e6d33;

    font-size: 0.8em;
}

.subh-label--joined      { color: #1e6d33; }
.subh-label--me          { color: #175bba; }
.subh-label--not-friends { color: #ba173b; }
.subh-label--pending-in  { color: #1e6d33; }
.subh-label--pending-out { color: #3d2888; }

.friends-list-entry.selectable {
    cursor: pointer;
}

.friends-list-entry.selectable:hover {
    background: #e2ebff;
}

.friends-list-entry.selectable.selected {
    background: #cbdcff;
}

.friends-list-entry.selected .shiny-button--remove,
.friends-list-entry:not(.selectable) .shiny-button--remove {
    opacity:        1;
    pointer-events: all;
}

.shiny-button:hover {
    transform: scale(1.02);
}

.shiny-button:active {
    box-shadow: none;
    transform:  scale(0.98);
}

.shiny-button--send-req:hover {
    transform: scale(1.015);
}

.shiny-button--send-req:active {
    transform: scale(0.985);
}
</style>