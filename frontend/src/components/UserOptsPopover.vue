<template>
    <teleport to="body">
        <div
            v-bind="attrs"
            role="menu"
            class="user-opts-popover"
            :hidden="!isExpanded"
            v-clicked-outside="clickedOutside"
            v-clicked-outside-except="popoverTarget">
            <li>
                <UserOptsPopoverLogOut
                    :is-pressed="isLoggingOut"
                    @click="logOut" />
            </li>
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
});

function clickedOutside() {
    if (!isLoggingOut.value)
        isExpanded.value = false;
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

    transition:
        scale   80ms,
        display 80ms allow-discrete;

    transform-origin: 50% calc(-3 * var(--border-thickness-s));

    will-change: transform;

    &[hidden]       { scale: 0; }
    @starting-style { scale: 0; }

    & > li {
        list-style: none;
    }
}
</style>