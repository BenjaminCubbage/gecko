<template>
    <a class="log-in-button" :href="href" draggable="false">
        <i class="google-icon hn hn-google"></i>

        <div class="button-text">
            Log in
        </div>
    </a>
</template>

<script setup>
defineProps({
    href: { type: String, required: true }
});
</script>

<style scoped>
.log-in-button {
    align-items:  center;
    display:      flex;
    gap:          12px;
    justify-self: start;
    padding:      5px 22px 5px 12px;

    color:           black;
    text-decoration: none;

    background:    white;
    border:        var(--border-s);
    border-radius: 999px;
    box-shadow:    var(--shadow-l);

    transition: transform 100ms ease;
}

.log-in-button:hover {
    transform: scale(1.02) rotate(1deg);
}

.log-in-button:active {
    box-shadow: var(--shadow-s);
    transform:  scale(0.97) rotate(0);
}

.google-icon {
    color:       transparent;
    font-size:   26px;
    line-height: 1;
    
    --google-blue:   #4285F4;
    --google-green:  #34A853;
    --google-red:    #EA4335;
    --google-yellow: #FBBC05;

    background-image: conic-gradient(at 50% 40%,
        var(--google-red)    25%,
        var(--google-blue)   25%,
        var(--google-blue)   50%,
        var(--google-green)  50%,
        var(--google-green)  65%,
        var(--google-yellow) 65%,
        var(--google-yellow) 90%,
        var(--google-red)    90%);
    background-clip: text;
}

.button-text {
    user-select:    none;
    font-family:    var(--font-heading);
    font-size:      2.4rem;
    letter-spacing: 0.04em;
}
</style>