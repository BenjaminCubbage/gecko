<template>
    <svg
        class="icon-friend-user"
        role="presentation"
        :data-variant="variant"
        viewBox="3 2 30.3 35.2">
        <path class="st0 stroke" d="M25.3 27.5v-2.4H24v-1.2h-1.2v-1.2h-1.2v-1.2h-9.7v1.2h-1.2v1.2H9.4v1.2H8.2v2.4H7v3.7h1.2v1.2h17v-1.2h1.2v-3.7zM10.7
                16.6h1.2v1.2h9.7v-1.2h1.2v-1.2H24V8.1h-1.2V6.8h-1.2V5.6h-9.7v1.2h-1.2V8H9.4v7.3h1.2v1.3z" />
        <path class="st1" d="M25.3 27.5v-2.4H24v-1.2h-1.2v-1.2h-1.2v2.4h1.2v2.4H24v2.4H9.4v1.2H8.2v1.2h17v-1.2h1.2v-3.7h-1.1zM14.3
                17.8h7.3v-1.2h1.2v-1.2H24V8.1h-1.2V6.8h-2.4V8h1.2v6.1h-1.2v1.2h-7.3v1.2h-1.2v1.2c.6.1 1.8.1 2.4.1" />
        <path class="st2" d="M20.4 5.6h-8.5v1.2h-1.2V8H9.4v7.3h1.2v1.2H13v-1.2h-1.2V9.2H13V8.1h7.3V6.8h1.2V5.6zM19.2
                21.4h-7.3v1.2h-1.2v1.2H9.4V25H8.2v2.4H7v3.7h2.4v-3.7h1.2V25h1.2v-1.2h9.7v-2.4z"/>
        <path class="st3 stroke" d="M28.6 9.9V8.7h-3.7v1.2h-1.2v3.7H25v1.2h3.7v-1.2h1.2V9.9zM17.3 8.7h-3.7v1.2h-1.2v3.7h1.2v1.2h3.7v-1.2h1.2V9.9h-1.2z"/>
        <!-- <path class="st3 stroke"  d="M13.7 11.1h3.7v1.2h-3.7zM25 11.1h3.7v1.2H25z"/> -->
    </svg>
</template>

<script setup>
defineProps({
    variant: {
        type:    String,
        default: 'vacant',
        validator(value) {
            return [
                'normal',
                'vacant'
            ].includes(value);
        }
    }
});
</script>

<style scoped>
.icon-friend-user {
    & > .st3 {
        transform-origin: center;
        fill:#FFFFFF;
    }
    
    & > .stroke {
        stroke:       black;
        stroke-width: 4px;
    }
}

.icon-friend-user[data-variant=normal] {
    & > .st0 { fill: var(--col-orange-4); }
    & > .st1 { fill: var(--col-orange-8); }
    & > .st2 { fill: var(--col-orange-0); }
}

.icon-friend-user[data-variant=vacant] {
    & > .st0 { fill: var(--col-lt-gray-0); }
    & > .st1 { fill: var(--col-lt-gray-0); }
    & > .st2 { fill: var(--col-lt-gray-0); }

    & > .stroke {
        stroke: var(--col-lt-gray-6);
    }
}
</style>