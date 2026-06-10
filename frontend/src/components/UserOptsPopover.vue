<template>
    <teleport to="body">
        <transition name="popover-appear">
            <menu
                v-if="isExpanded"
                v-bind="attrs"
                class="user-opts-popover"
                :aria-expanded="isExpanded"
                v-clicked-outside="tryCollapse"
                v-clicked-outside-except="popoverTarget"
                v-roving-container
                @keydown.tab.exact="tabNext"
                @keydown.tab.shift="tabPrev">
                <li>
                    <UserOptsPopoverLogOut
                        :is-pressed="isLoggingOut"
                        @click="logOut"
                        v-roving-item
                        v-auto-focus />
                </li>
            </menu>
        </transition>
    </teleport>
</template>

<script setup>
import {
    ref,
    useAttrs,
    watch
} from 'vue';

import UserOptsPopoverLogOut       from './UserOptsPopoverLogOut.vue';
import { getNextFocusableElement } from '@/core/dom/tabOrder.js';

defineOptions({
    inheritAttrs: false
});

const props = defineProps({
    popoverTarget: {
        required: true,
        validator(value) {
            return value == null || value instanceof HTMLElement;
        }
    }
});

const isExpanded = defineModel('is-expanded', {
    type:    Boolean,
    default: true
});

const emit = defineEmits([
    'logOut'
]);

const attrs = useAttrs();

/*
    Popover position (bottom-center of target) in px
*/
const clientLeft = ref(0);
const clientTop  = ref(0);

const isLoggingOut = ref(false);

watch([ isExpanded, () => props.popoverTarget ], () => {
    const clientRect = props.popoverTarget?.getBoundingClientRect();

    if (props.popoverTarget == null || !clientRect?.width) {
        /* No popover target, or it's hidden */
        isExpanded.value = false;
        return;
    }

    clientLeft.value = clientRect.width / 2  + clientRect.left;
    clientTop.value  = clientRect.height     + clientRect.top;
}, { flush: 'post' });

function tryCollapse() {
    if (!isLoggingOut.value)
        isExpanded.value = false;
}

function tabNext(e) {
    const a = getNextFocusableElement(props.popoverTarget);
    if (a != null) {
        a.focus();
        e.preventDefault();
        tryCollapse();
    }
}

function tabPrev(e) {
    props.popoverTarget.focus();
    e.preventDefault();
    tryCollapse();
}

function logOut() {
    if (isLoggingOut.value)
        return;

    isLoggingOut.value = true;
    emit('logOut', () =>
        isLoggingOut.value = false);
}
</script>

<style scoped>
.user-opts-popover {
    position: fixed;
    z-index:  999;

    left: v-bind('`${clientLeft}px`');
    top:  v-bind('`${clientTop}px`');

    margin-top: 12px;

    translate:   -50%;
    will-change: transform;

    filter:
        drop-shadow(
            var(--shadow-dist-m)
            var(--shadow-dist-m)
            var(--col-shadow-alpha))
        var(--filter-hl-1);

    transform-origin: 50% calc(-3 * var(--border-thickness-s));

    > li {
        list-style: none;
    }
}

.popover-appear-enter-active,
.popover-appear-leave-active {
    transition: scale 80ms;
}

.popover-appear-enter-from,
.popover-appear-leave-to {
    scale: 0;
}
</style>