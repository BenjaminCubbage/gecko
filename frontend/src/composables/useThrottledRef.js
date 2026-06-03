import {
    customRef,
    onScopeDispose,
    watch
} from 'vue';

/*
    Throttle ref updates with respect to a source ref.

    This is useful for loading states, pressed states, etc.
    where you want to prevent flickering.

    The returned customRef is mostly designed to be readonly.
    However it can be set() to immediately flush a value and
    reset the timeout.

    This is useful for states where one state change (such as
    pressing) should always be instantaneous, whereas the inverse
    (such as unpressing) should be throttled.

    Usually the set value should be the same as the source value,
    though it doesn't technically need to be.
*/
export function useThrottledRef(source, ms) {
    let timeOfLastFlush = -Infinity;

    let unwatch = null;
    let timeout = null;
    let value   = source.value;

    function cancelTimeout() {
        clearTimeout(timeout);
        timeout = null;
    }

    onScopeDispose(() => {
        unwatch();
        cancelTimeout();
    });

    return customRef((track, trigger) => {
        function flushNow(newValue) {
            const valueChanged = !Object.is(newValue, value);

            timeOfLastFlush = performance.now();
            value           = newValue;
            cancelTimeout();

            /* Resolve back to source value later. */
            if (newValue != source.value)
                flushWithTimeout();

            if (valueChanged)
                trigger();
        }

        function flushWithTimeout() {
            const now = performance.now();
            cancelTimeout();

            if (timeOfLastFlush + ms < now)
                flushNow(source.value);
            else {
                timeout = setTimeout(() => {
                    timeout = null;
                    flushNow(source.value);
                }, ms - (now - timeOfLastFlush));
            }
        }

        unwatch = watch(source, () => {
            if (timeout == null)
                flushWithTimeout();
        });

        return {
            get: () => {
                track();
                return value;
            },

            /*
                [!] This flushes the value immediately. Usually newValue
                should be === source.value.
            */
            set: (newValue) => {
                flushNow(newValue);
            }
        }
    });
}