<template>
    <div 
        class="
            friends-list-view
            shdw shdw--otst-orange">
        <div 
            class="
                view-outer
                shdw-after  shdw-after--inst-gray
                shdw-before shdw-before--inst-gray">
            <span 
                class="
                    outer-bg
                    shdw shdw--inst-lt-gray">
            </span>

            <button
                v-for="friend in friends"
                class="outer-item"
                :data-state="
                    friend ?
                        friend === selectedFriend 
                            ? 'selected' 
                            : 'unselected' 
                        : 'empty'"
                @click="friend != null && (selectedFriend = friend)">
                <span 
                    class="
                        item-gutter item-gutter--l
                        shdw shdw--inst-lt-gray">
                    <IconListArrow
                        v-if="friend != null"
                        height="19px"
                        class="item-arrow item-arrow--l"
                        direction="right"
                        :variant="friend === selectedFriend ? 'normal' : 'inactive'"
                        inert />
                </span>

                <span 
                    class="
                        item-inner
                        shdw shdw--inst-orange">
                    <span v-if="friend != null" class="item-text">
                        @{{ friend.user.username }}
                    </span>
                </span>

                <span 
                    class="
                        item-gutter item-gutter--r
                        shdw shdw--inst-lt-gray">
                    <IconListArrow
                        v-if="friend != null"
                        height="19px"
                        class="item-arrow item-arrow--r"
                        direction="left"
                        :variant="friend === selectedFriend ? 'normal' : 'inactive'"
                        inert />
                </span>
            </button>
        </div>
    </div>
</template>

<script setup>
import { 
    ref,
    watch 
} from 'vue';

import IconListArrow from './IconFriendListArrow.vue';
import { Friend }    from '@/models/friend.js'

const props = defineProps({
    friends: {
        type: Array,
        required: true,
        validator(value) {
            return value.every(v => v == null || v instanceof Friend);
        }
    }
});

const selectedFriend = defineModel('selectedFriend', {
    required: true,
    validator(value, props) {
        return value == null || props.friends.includes(value);
    }
});

watch(() => props.friends, () => {
    if (selectedFriend.value != null &&
        !props.friends.includes(selectedFriend.value)) {
        selectedFriend.value = props.friends.find(f => f != null);
    }
}, {
    immediate: true
});

const names = [
    'Ben',
    'Joey',
    'Gary',
    'Bartholomew',
    'WWWWWWWWWWWWWWWWWW'
];

const selectedIndex = ref(2);
</script>

<style scoped>
.friends-list-view {
    --wd-outer-gutter: 38px;
    --ht-list-item:    34px;
    --ht-elevation:    var(--border-thickness-s);

    border:        var(--border-s);
    border-radius: var(--radius-s);
}

.view-outer {
    isolation: isolate;
    position:  relative;

    display:   flex;
    flex-flow: column;

    background:    var(--col-gray-2);
    border-radius: var(--radius-s);

    &::before       { z-index: 0; }
    &::after        { z-index: 0; }
    & > .outer-item { z-index: 1; }

    &::before,
    &::after {
        content:  '';
        position: absolute;

        width:  var(--wd-outer-gutter);
        height: 100%;
    }

    &::before { left:  0; border-radius: var(--radius-s) 0 0 var(--radius-s); }
    &::after  { right: 0; border-radius: 0 var(--radius-s) var(--radius-s) 0; }
}

.outer-bg {
    position: absolute;
    
    inset:  0;
    margin: 0 auto;

    height: 100%;
    width:  
        calc(100% - 2 * (var(--wd-outer-gutter) + var(--border-thickness-s)) + 0.5px);

    background: var(--col-lt-gray-1);

    cursor: pointer;
}

.outer-item {
    position: relative;

    display: grid;
    grid-template:
        "gutter-l               inner gutter-r" var(--ht-list-item) /
         var(--wd-outer-gutter) 1fr   var(--wd-outer-gutter);

    padding: 0;
    border:  0;

    color:      inherit;
    font:       inherit;
    background: transparent;

    & > .item-gutter--l { grid-area: gutter-l; }
    & > .item-inner     { grid-area: inner; }
    & > .item-gutter--r { grid-area: gutter-r; }

    &:has(~ .outer-item) {
        & > .item-gutter { border-bottom: 3px solid var(--col-gray-4); }
        & > .item-inner  { border-bottom: 3px solid var(--col-gray-3); }
    }

    &:where(:not([data-state='empty']):hover) > .item-inner {
        background: rgb(0 0 0 / 0.05); 
    }

    &[data-state='selected'] {
        translate: 0 calc(-1 * var(--ht-elevation));

        filter:
            drop-shadow(
                0
                var(--shadow-dist-m)
                var(--col-shadow-alpha));

        & > .item-gutter {
            background: var(--col-lt-gray-4);
            box-shadow: revert-layer;

            --shdw-etc:
                0 0 0 var(--border-thickness-s) black;
        }

        & > .item-inner {
            background: var(--col-orange-2);
            box-shadow: revert-layer;

            --shdw-etc:
                0 0 0 var(--ht-elevation) black;
        }
    }

    &[data-state='selected'] {
        & > .item-gutter,
        & > .item-inner {
            border-bottom-color: black;
        }
    }

    &[data-state='empty'] {
        cursor: default;
    }

    &:nth-child(1 of .outer-item) {
        & > .item-gutter--l { border-radius: var(--radius-s) 0 0 0; }
        & > .item-gutter--r { border-radius: 0 var(--radius-s) 0 0; }
    }

    &:nth-last-child(1 of .outer-item) {
        & > .item-gutter--l { border-radius: 0 0 0 var(--radius-s); }
        & > .item-gutter--r { border-radius: 0 0 var(--radius-s) 0; }
    }
}

.item-gutter {
    display: grid;
    place-items: center;

    background: transparent;
    box-shadow: none;
}

.item-inner {
    height: var(--ht-list-item);

    font-family: var(--font-scnd);
    font-size:   1.7rem;
    font-weight: bold;

    display:     grid;
    place-items: center left;

    padding: 0 12px;
    margin:  0 calc(var(--shadow-dist-m) * 0.95);

    box-shadow:
        calc(-1 * var(--shadow-dist-m)) 0 black,
                  var(--shadow-dist-m)  0 black;
}

.item-text {
    -webkit-text-stroke: var(--text-stroke-xs);
    translate:           0 1px;
}
</style>