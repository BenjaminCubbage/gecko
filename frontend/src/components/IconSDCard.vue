<template>
    <svg viewBox="10 4 132.5 137.1">
        <path class="stroke s0" d="M117.2 19.7v-5.4H30.7v5.4h-5.4v5.4h-5.4v97.3h5.4v5.4H101v-5.4h5.4v-21.6h5.4v-5.4h5.4V90h5.4
            V68.4h-16.2V52.2h5.4v-5.4h5.4v-5.4h5.4V19.8h-5.4z"/>
        <path class="s3" d="M117.2 19.7h-5.4v16.2h-5.4v5.4H101v5.4h-5.4v16.2h5.4v5.4h5.4V52.1h5.4v-5.4h5.4v-5.4h5.4
            V19.7h-5.4z" />
        <path class="s6" d="M111.8 25.1v-5.4h5.4v-5.4H30.7v5.4h-5.4v5.4h-5.4v97.3h5.4V90h5.4V30.5h5.4v-5.4h75.7z" />
        <path class="s3" d="M111.8 73.7v16.2h-5.4v5.4H101v21.6h-5.4v5.4h-5.4V117h-5.4v5.4H74V117h-5.4v5.4H57.8V117h-5.4
            v5.4H41.6V117h-5.4v5.4h-5.4V117h-5.4v10.8h75.7v-5.4h5.4v-21.6h5.4v-5.4h5.4V90h5.4V73.8c-2.8-.1-8.2-.1-10.9-.1z" />        
        <g class="pins">
            <path class="stroke" d="M95.6 105.8h-5.4v5.4V122v5.4h5.4V122h5.4v-10.8-5.4h-5.4zm-21.7 5.4v5.4 5.4 5.4h10.8V122v-10.8-5.4
                H73.9v5.4zm-16.2 0v5.4 5.4 5.4h10.8V122v-10.8-5.4H57.7v5.4zm-16.2 0v5.4 5.4 5.4h10.8V122v-10.8-5.4H41.5
                v5.4zm-16.2 0v5.4 5.4h5.4v5.4h5.4V122v-10.8-5.4H25.3v5.4z" />
            <path class="s1" d="M90.1 122h5.4v5.4h-5.4zm-16.2 0v5.4h10.8V122H73.9zm-16.2 0v5.4h10.8V122H57.7zm-16.2 0v5.4h10.8V122
                H41.5zm-10.8 0h5.4v5.4h-5.4z" />
            <g class="s2">
                <path d="M95.6 111.2V122h5.4v-10.8h-5.4z"/>
                <use href="#B"/>
                <use href="#B" x="-16.2"/>
                <use href="#B" x="-32.4"/>
                <use href="#B" x="-48.6"/>
            </g>
            <g class="s4">
                <path d="M90.1 111.2V122h5.4v-10.8h-5.4z"/>
                <use href="#B" x="-5.4"/>
                <use href="#B" x="-21.6"/>
                <use href="#B" x="-37.8"/>
                <use href="#B" x="-54"/>
            </g>
            <path class="s5" d="M95.6 105.8h-5.4v5.4H101v-5.4h-5.4zm-21.7 0v5.4h10.8v-5.4H73.9zm-16.2 0v5.4h10.8v-5.4H57.7zm-16.2 0v5.4
                h10.8v-5.4H41.5zm-16.2 0v5.4h10.8v-5.4H25.3z" />
        </g>
        <path class="s6" d="M117.2 68.3h-10.8v10.8h5.4v-5.4h10.8v-5.4h-5.4z" />
        <defs >
            <path id="B" d="M79.3 116.6v5.4h5.4v-10.8h-5.4v5.4z"/>
        </defs>
    </svg>
</template>

<style>
.stroke {
    stroke: black;
    stroke-width: 19px;
}

.s0 { fill: var(--col-lt-blue-6) }
.s1 { fill: var(--col-lt-gray-7) }
.s2 { fill: var(--col-lt-gray-5) }
.s3 { fill: var(--col-blue-4) }
.s4 { fill: var(--col-lt-gray-2) }
.s5 { fill: var(--col-lt-gray-1) }
.s6 { fill: var(--col-lt-blue-1) }

.pins {
    translate: -2px 0px;
}
</style>