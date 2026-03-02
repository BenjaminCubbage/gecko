import {
    onUnmounted,
    readonly,
    ref
} from 'vue';

export function useTimer({ duration, initiallyCompleted = false }) {
    let timeout       = null;
    const isCompleted = ref(initiallyCompleted);

    function reset(completed = false) {
        cancel();
        isCompleted.value = completed;
    }

    function restart(overrideDuration = null) {
        cancel();

        timeout = setTimeout(() => {
            isCompleted.value = true;
        }, overrideDuration ?? duration);
    }

    function cancel() {
        clearTimeout(timeout);
    }

    onUnmounted(cancel);

    return {
        isCompleted: readonly(isCompleted),
        reset,
        restart,
        cancel
    };
}