<template>
    <svg viewBox="0 0 36.3 36.2">
        <g class="door">
            <path class="s0 stroke" d="M32 10.7V8h-1.4V6.6h-1.4V5.2h-2.7V3.9h-6.8v1.4H17v1.4h-1.4v1.4h-1.4v2.7h-1.4v20.3h1.4v1.4h17.6V31h1.4V10.7H32z"/>
            <path class="s1" d="M26.6 6.6h-1.4v23h2.7v-23h-1.3zm-8.1 0v23h2.7v-23h-2.7z"/>
            <path class="s2" d="M32 10.7V8h-1.4V6.6h-2.7V8h1.4v2.7h1.4v19H15.8V31h-1.4v1.4H32V31h1.4V10.7H32z" />
            <path class="s3" d="M28 5.2h-1.4V3.9h-6.8v1.4h-2.7v1.4h-1.4v1.4h-1.4v2.7h-1.4v20.3h2.7V9.4H17V7.9h1.4V6.6h10.8V5.2H28z" />
            <path d="M19.8 18.1v-1.4h-2.7v1.4h-1.4v2.7h1.4v1.4h2.7v-1.4h1.4v-2.7h-1.4z"/>
        </g>

        <g class="arrow">
            <path class="s4 stroke" d="M14.5 6.5V5.2h-2.6v1.3H9.3V3.9H6.7v1.3H5.5v1.3H4.2v1.3H2.9v3.8h1.3v1.3h1.3v1.3h1.3v1.3h2.6v-2.6H12v1.3h2.6v-1.3h1.3V6.5h-1.4z" />
            <path class="s5" d="M14.5 6.5v1.3h-1.3v3.8H8.1v1.3H6.8v-1.3H4.2v1.3h1.3v1.3h1.3v1.3h2.6v-2.6H12v1.3h2.6v-1.3h1.3V6.5h-1.4z"/>
            <path class="s6" d="M13.2 5.2h-1.3v1.3H9.3V3.9H6.7v1.3H5.5v1.3H4.2v1.3H2.9v3.8h2.6V7.8h1.3V6.5h1.3v1.3h6.4V5.2h-1.3z"/>
        </g>
    </svg>
</template>

<style scoped>
.stroke {
    stroke-width: 5px;
}

.s0 { fill: var(--col-orange-4); }
.s1 { fill: var(--col-orange-3); }
.s2 { fill: var(--col-orange-8); }
.s3 { fill: var(--col-orange-0); }

.s4 { fill: var(--col-red-3); }
.s5 { fill: var(--col-red-8); }
.s6 { fill: var(--col-red-0); }
</style>