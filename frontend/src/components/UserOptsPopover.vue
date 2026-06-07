<template>
    <teleport to="body">
        <div
            v-bind="attrs"
            role="menu"
            class="user-opts-popover"
            :hidden="!isExpanded"
            :aria-expanded="isExpanded"
            v-clicked-outside="clickedOutside">
            <UserOptsPopoverLogOut />
        </div>
    </teleport>
</template>

<script setup>
import { 
    ref,
    useAttrs,
    watch
} from 'vue';

import UserOptsPopoverLogOut from './UserOptsPopoverLogOut.vue';

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

const attrs = useAttrs();

const clientLeft = ref(0);
const clientTop  = ref(0);

watch([ isExpanded, () => props.popoverTarget ], () => {
    const clientRect = props.popoverTarget?.getBoundingClientRect();

    if (props.popoverTarget == null || !clientRect?.width) {
        /* No popover target, or it's hidden */
        isExpanded.value = false;
        return;
    }

    clientLeft.value = clientRect.width / 2  + clientRect.left;
    clientTop.value  = clientRect.height + clientRect.top;
});

function clickedOutside() {
    isExpanded.value = false;
}
</script>

<style scoped>
.user-opts-popover {
    position: fixed;
    z-index:  999;

    left: v-bind('`${clientLeft}px`');
    top:  v-bind('`${clientTop}px`');

    margin-top: 12px;

    translate: -50%;

    filter:
        drop-shadow(
            var(--shadow-dist-m)
            var(--shadow-dist-m)
            var(--col-shadow-alpha));

    transition:
        scale   80ms steps(4, end),
        display 80ms allow-discrete;

    transform-origin: 50% calc(-3 * var(--border-thickness-s));

    &[hidden]       { scale: 0; }
    @starting-style { scale: 0; }
}
</style>