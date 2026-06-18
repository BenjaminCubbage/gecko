<template>
    <div
        class="
            friends-list-view
            shdw shdw--otst-orange"
        v-roving-container
        v-roving-home="selectedIndex">
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
                v-for="(friend, index) in friends"
                :key="index"
                class="outer-item"
                :data-state="
                    friend != null ?
                        friend === selectedFriend
                            ? 'selected'
                            : 'unselected'
                        : 'empty'"
                @click=        "friend != null && (selectedFriend = friend)"
                :tabindex=     "friend != null || index === 0 ? 0 : -1"
                :aria-label=   "friend != null ? `@${friend.user.username}` : 'Empty friend slot'"
                :aria-selected="friend != null && friend === selectedFriend"
                v-interactive-attrs
                v-roving-item>
                <div class="outer-item-grid">
                    <span
                        class="
                            item-gutter item-gutter--l
                            txtr-vert txtr-vert--gray
                            shdw shdw--inst-gray">
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
                            shdw shdw--inst-orange
                            txtr-vert txtr-vert--orange">
                        <span v-if="friend != null" class="item-text">
                            @{{ friend.user.username }}
                        </span>
                    </span>

                    <span
                        class="
                            item-gutter item-gutter--r
                            txtr-vert txtr-vert--gray
                            shdw shdw--inst-gray">
                        <IconListArrow
                            v-if="friend != null"
                            height="19px"
                            class="item-arrow item-arrow--r"
                            direction="left"
                            :variant="friend === selectedFriend ? 'normal' : 'inactive'"
                            inert />
                    </span>
                </div>
            </button>
        </div>
    </div>
</template>

<script setup>
import {
    computed,
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
        return value == null || value instanceof Friend;
    }
});

const selectedIndex = computed(() => {
    const i = props.friends.indexOf(selectedFriend.value);
    return i === -1 ? 0 : i;
});

watch(() => props.friends, () => {
    if (selectedFriend.value != null &&
        !props.friends.includes(selectedFriend.value)) {
        selectedFriend.value = props.friends.find(f => f != null);
    }
}, { immediate: true });
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

    &::before     { z-index: 0; }
    &::after      { z-index: 0; }
    > .outer-item { z-index: 1; }

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

    background: var(--col-lt-gray-2);

    cursor: pointer;
}

.outer-item {
    position: relative;

    display: block;
    width:   100%;

    padding: 0;
    border:  0;

    color:      inherit;
    font:       inherit;
    background: transparent;

    &[data-state=selected] {
        translate: 0 calc(-1 * var(--ht-elevation));

        filter:
            var(--filter-hl-1)
            drop-shadow(
                0
                var(--shadow-dist-m)
                var(--col-shadow-alpha));
    }

    &[data-state=empty] {
        cursor: default;
    }

    &:has(~ .outer-item) {
        > .outer-item-grid > .item-gutter {
            border-bottom: 3px solid var(--col-gray-4);
        }

        > .outer-item-grid > .item-inner {
            border-bottom: 3px solid var(--col-gray-3);
        }
    }

    &:where(:not([data-state=empty])[data-hovered]) {
        > .outer-item-grid > .item-inner {
            background:
                linear-gradient(
                    rgb(0 0 0 / 0.01) 50%,
                    rgb(0 0 0 / 0.04) 50%);
        }
    }

    &[data-state=selected] {
        > .outer-item-grid > .item-gutter {
            background: revert-layer;
            box-shadow: revert-layer;

            --shdw-etc:
                0 0 0 var(--border-thickness-s) black;
        }

        > .outer-item-grid > .item-inner {
            background: revert-layer;
            box-shadow: revert-layer;

            --shdw-etc:
                0 0 0 var(--ht-elevation) black;
        }

        > .outer-item-grid > .item-gutter,
        > .outer-item-grid > .item-inner {
            border-bottom-color: black;
        }
    }

    &:nth-child(1 of .outer-item) {
        > .outer-item-grid > .item-gutter--l {
            border-radius: var(--radius-s) 0 0 0;
        }

        > .outer-item-grid > .item-gutter--r {
            border-radius: 0 var(--radius-s) 0 0;
        }
    }

    &:nth-last-child(1 of .outer-item) {
        > .outer-item-grid > .item-gutter--l {
            border-radius: 0 0 0 var(--radius-s);
        }

        > .outer-item-grid > .item-gutter--r {
            border-radius: 0 0 var(--radius-s) 0;
        }
    }
}

.outer-item-grid {
    display: grid;

    grid-template-areas: "gutter-l inner gutter-r";
    grid-template-rows: var(--ht-list-item);
    grid-template-columns:
        var(--wd-outer-gutter)
        minmax(0, 1fr)
        var(--wd-outer-gutter);

    height: var(--ht-list-item);
}

.outer-item-grid > .item-gutter--l { grid-area: gutter-l; }
.outer-item-grid > .item-inner     { grid-area: inner; }
.outer-item-grid > .item-gutter--r { grid-area: gutter-r; }

.item-gutter {
    display:     grid;
    place-items: center;

    background:
        linear-gradient(
            rgb(255 255 255 / 0.4) 50%,
            #0000                  50%)
        center / calc(100% - 2 * var(--shadow-dist-m)) 100% no-repeat;
    box-shadow: none;
}

.item-inner {
    height: var(--ht-list-item);

    padding: 0 12px;
    margin:  0 calc(var(--shadow-dist-m) * 0.95);

    font-family: var(--font-scnd);
    font-size:   1.7rem;
    font-weight: bold;

    line-height: var(--ht-list-item);

    text-align: left;
    white-space:   nowrap;
    overflow:      hidden;
    text-overflow: ellipsis;

    background:
        linear-gradient(
            rgb(255 255 255 / 0.4) 50%,
            #0000                  50%)
        center / calc(100% - 2 * var(--shadow-dist-m)) 100% no-repeat;
    box-shadow:
        calc(-1 * var(--shadow-dist-m)) 0 black,
                  var(--shadow-dist-m)  0 black;
}

.item-text {
    -webkit-text-stroke: var(--text-stroke-xs);
    translate:           0 1px;
}
</style>