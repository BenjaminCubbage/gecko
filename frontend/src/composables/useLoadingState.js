import {
    computed,
    onUnmounted,
    ref
} from 'vue';

/*
    Utility for displaying a loading state while the user waits
    for a task to complete.

    For instance, a spinner can be shown if a resource fetch is
    taking a while. It will then be displayed for a minimum amount
    of time, to prevent flickering.
*/
export function useLoadingState(spinDelay = 100, minimumSpinTime = 300) {
    const isActuallyLoading = ref(false);
    const isSpinning        = ref(false);

    const isLoading = computed(() => {
        return isActuallyLoading.value || isSpinning.value;
    });

    onUnmounted(() => {
        clearTimeout(spinStartTimeout);
        clearTimeout(spinStopTimeout);
    });

    let spinStartTimeout = null;
    let spinStopTimeout  = null;
    let spinStartTime = null;

    function startedLoading() {
        isActuallyLoading.value = true;

        clearTimeout(spinStartTimeout);
        clearTimeout(spinStopTimeout);

        spinStartTimeout = setTimeout(() => {
            isSpinning.value = true;
            spinStartTime    = performance.now();
        }, spinDelay);
    }

    function stoppedLoading() {
        isActuallyLoading.value = false;

        clearTimeout(spinStartTimeout);
        clearTimeout(spinStopTimeout);

        if (!isSpinning.value)
            return;

        const timeDifference = (spinStartTime + minimumSpinTime) - performance.now();

        if (timeDifference <= 0) {
            isSpinning.value = false;
        } else {
            spinStopTimeout = setTimeout(() => {
                isSpinning.value = false;
            }, timeDifference);
        }
    }

    return {
        startedLoading,
        stoppedLoading,
        isActuallyLoading,
        isLoading,
        isSpinning
    };
}