<template>
    <dialog
        ref="dialogEl"
        class="modal-welcome"
        closedby="any"
        @close="isOpen = false"
        v-modal-auto-focus>
        <div class="
            modal-header
            shdw shdw--inst-lt-gray">
            <h1 class="header-title">Welcome</h1>

            <button 
                class="header-close"
                type="button"
                aria-label="Close dialog"
                @click="isOpen = false">
                <IconClose height="29px" inert />
            </button>
        </div>

        <div 
            class="
                modal-body
                shdw shdw--elevated-l"
            tabindex="-1"
            v-modal-auto-focus-target>
            <div class="body-preface">
                <span class="
                    preface-icon
                    efct-rays-before efct-rays-before--rotating">
                    <IconLightBulb height="50px" role="presentation" />
                </span>
                <p class="preface-text">
                    Send black-and-white images to my <span class="text-emphasis util-nowrap">E-Ink Board</span>!
                </p>
            </div>

            <div class="body-steps">
                <h2 class="steps-subtitle">If you know me:</h2>
                <ul class="steps-list">
                    <li>Log in w/ Google</li>
                    <li>Send a friend request <span class="util-nowrap">to <span class="text-emphasis">@ben</span></span></li>
                    <li>Once I accept, you'll see me as <span class="util-nowrap">a recipient ☺</span></li>
                </ul>
            </div>
        </div>

        <div class="modal-bracket modal-bracket--l" inert></div>
        <div class="modal-bracket modal-bracket--r" inert></div>

        <button
            v-if="variant === 'loggedout'"
            class="
                modal-log-in-button
                txtr-vert txtr-vert--green
                shdw shdw--inst-green shdw--elevated-l"
            :data-is-pressed="isLoggingIn"
            type="button"
            @click="emit('log-in')">
            Log In
        </button>

        <aside v-else
            class="
                modal-alt-aside
                shdw shdw--inst-lt-gray shdw--elevated-s">
            Already Logged In!
        </aside>

        <IconCloud class="icon-cloud" inert />
    </dialog>
</template>

<script setup>
import {
    useTemplateRef,
    watch
} from 'vue';

import IconLightBulb from './IconLightBulb.vue';
import IconClose     from './IconClose.vue';
import IconCloud     from './IconCloud.vue';

defineProps({
    variant: {
        type:     String,
        required: true,
        validator(value) {
            return [
                'loggedin',
                'loggedout'
            ].includes(value);
        }
    },

    isLoggingIn: {
        type:     Boolean,
        required: true
    }
});

const emit = defineEmits([
    'log-in'
]);

const isOpen = defineModel('isOpen', {
    type:     Boolean,
    required: true
});

const dialogEl = useTemplateRef('dialogEl');

watch([isOpen, dialogEl], () => {
    if (dialogEl.value != null) {
        if (isOpen.value)
            dialogEl.value.showModal();
        else
            dialogEl.value.close();
    } else
        isOpen.value = false;
}, {
    immediate: true
});
</script>

<style scoped>
.modal-welcome {
    anchor-scope: --brackets-anchor;
    container:    modal-welcome / inline-size;

    isolation: isolate;

    display:   flex;
    flex-flow: column;
    overflow:  visible;

    width: 430px;

    > .modal-log-in-button,
    > .modal-alt-aside { z-index: 4; }
    > .modal-header    { z-index: 2; }
    > .modal-body      { z-index: 1; }
    > .modal-bracket   { z-index: 3; }
    > .icon-cloud      { z-index: 0; }
}

.modal-header {
    isolation:     isolate;
    display:       grid;
    place-content: end stretch;

    position: relative;
    width:  100%;
    height: 43px;

    background: var(--col-gray-2);

    border:        var(--border-s);
    border-radius: var(--radius-s) var(--radius-s) 0 0;

    filter:
        drop-shadow(
            var(--shadow-dist-m)
            var(--shadow-dist-m)
            var(--col-shadow-alpha));

    > .header-title { grid-area: 1 / 1; place-self: center; }
    > .header-close { grid-area: 1 / 1; place-self: end; }
}

.header-title {
    --wd-cap: 24px;
    --sz-peg: 6px;

    position:      relative;
    margin-bottom: 8px;
    padding:       2px 18px 1px 22px;

    -webkit-text-stroke: var(--text-stroke-s);
    font-size:           3.5rem;
    letter-spacing:      0.05em;
    line-height:         1;
    text-transform:      uppercase;

    background:    var(--col-yellow-2);
    border:        var(--border-s);
    border-radius: var(--radius-s);

    box-shadow:
        inset      var(--shadow-dist-m)            var(--shadow-dist-m)       var(--col-yellow-0),
        inset calc(var(--shadow-dist-m) * -1) calc(var(--shadow-dist-m) * -1) var(--col-yellow-5),
        0 var(--shadow-dist-m);

    filter:
        drop-shadow(
            var(--shadow-dist-m)
            var(--shadow-dist-m)
            var(--col-shadow-alpha));

    &::before,
    &::after {
        content:  '';
        position: absolute;
        top:      0;
        bottom:   0;

        width:  var(--wd-cap);
        height: calc(100% - 5px);
        margin: auto 0;

        background:
            linear-gradient(
                var(--col-gray-4) 0 0)
                center / var(--sz-peg) var(--sz-peg)
                no-repeat
            var(--col-gray-2);

        border: var(--border-s);

        box-shadow:
            inset      var(--shadow-dist-m)            var(--shadow-dist-m)       var(--col-gray-1),
            inset calc(var(--shadow-dist-m) * -1) calc(var(--shadow-dist-m) * -1) var(--col-gray-3),
            0 var(--shadow-dist-s) black;
    }

    &::before { left:  calc(var(--wd-cap) * -1); border-radius: var(--radius-s) 0 0 var(--radius-s); }
    &::after  { right: calc(var(--wd-cap) * -1); border-radius: 0 var(--radius-s) var(--radius-s) 0; }
}

.header-close {
    margin: 0 -9px 18px 0;

    line-height: 0;
    opacity:     0.65;

    &:hover,
    &:active {
        @media (hover: hover) {
            scale: 1.05;
        }

        filter:  
            var(--filter-hl-1)
            drop-shadow(
                var(--shadow-dist-m)
                var(--shadow-dist-m)
                var(--col-shadow-alpha));
        opacity: 1;
    }
}

.modal-body {
    anchor-name: --brackets-anchor;

    position:   relative;
    margin:     0 6px;
    background: var(--col-lt-gray-4);

    border:        var(--border-s);
    border-top:    0;

    overflow: auto;

    --shdw-etc:
        inset calc(-1 * var(--shadow-dist-m)) calc(-1 * var(--shadow-dist-m)) var(--col-gray-3),
        inset           var(--shadow-dist-m)  0                               var(--col-lt-gray-0);

    font-family: var(--font-scnd);

    padding: 4px 24px 20px 24px;
    @container modal-welcome (width < 400px) {
        padding-left: 12px;
        padding-right: 12px;
    }
}

.body-preface {
    display: grid;
    grid-template:
        "icon text" auto /
         auto minmax(0, 1fr);

    gap:         14px;
    align-items: center;

    border-bottom:
        var(--border-thickness-s) dashed var(--col-gray-4);

    font-size:   1.8rem;
    line-height: 1.3;

    > .preface-icon { grid-area: icon; }
    > .preface-text { grid-area: text; }
    
    padding: 18px 28px 12px;
    @container modal-welcome (width < 400px) {
        padding-left:  10px;
        padding-right: 10px;
    }
}

.preface-icon {
    line-height: 0;
    position:    relative;

    /*
        Rays behind bulb
    */
    &::before {
        translate: 0 -4px;
    }
}

.preface-text {
    &, & span {
        cursor: text;
    }
}

.icon-light-bulb {
    filter:
        drop-shadow(
            var(--shadow-dist-m)
            var(--shadow-dist-m)
            var(--col-shadow-alpha));
}

.body-steps {
    line-height: 1.3;
    font-size:   1.8rem;
    
    padding: 12px 32px 16px 32px;
    @container modal-welcome (width < 400px) {
        padding-left:  12px;
        padding-right: 12px;
    }
}

.steps-subtitle {
    font-size:   1.05em;
    font-weight: bold;
}

.steps-list {
    display:     flex;
    flex-flow:   column;
    gap:         2px;
    padding-top: 3px;

    > li {
        --sz-bullet: 7px;

        background:
            url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 9 9'%3E%3Cpath fill='%231256dd' d='M0,0v9h9V0H0z M6,6H3V3h3V6z'/%3E%3C/svg%3E")
            left calc(1em * 0.65 - var(--sz-bullet) / 2) / var(--sz-bullet) no-repeat;

        padding-left: calc(var(--sz-bullet) * 2);
        padding-top:  2px;
    }

    &, > li, > li span {
        cursor: text;
    }
}

.text-emphasis {
    color:       var(--col-blue-4);
    font-weight: bold;
}

.modal-bracket {
    position: fixed;
    bottom:   calc(anchor(--brackets-anchor bottom) - 6px);
    width:    43px;
    height:   43px;

    background: var(--col-gray-2);

    box-shadow:
        inset  3px 3px  var(--col-gray-1),
        inset -3px -3px var(--col-gray-3),
        inset -2px 2px  var(--col-gray-1),
        0 var(--shadow-dist-l) black;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    &.modal-bracket--l {
        border-top-right-radius: 26px;
        corner-top-right-shape: bevel;
        left: calc(anchor(--brackets-anchor left) - 6px);
    }

    &.modal-bracket--r {
        border-top-left-radius: 26px;
        corner-top-left-shape: bevel;
        right: calc(anchor(--brackets-anchor right) - 6px);
    }

    @supports not (anchor-name: --a) {
        display: none;
    }
}

.modal-log-in-button,
.modal-alt-aside {
    --hl:   brightness(1);
    
    align-self:    center;
    margin-top:    -24px;

    position: relative;
    padding:  2px 24px 2px 26px;

    letter-spacing: 0.04em;

    -webkit-text-stroke: var(--text-stroke-s);

    text-transform: uppercase;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    filter:
        drop-shadow(
            var(--shadow-dist-m)
            var(--shadow-dist-m)
            var(--col-shadow-alpha))
        var(--hl);
        
    translate: 
        0 
        calc(var(--shadow-dist-l) - var(--shdw-dist-elevation));

}

.modal-log-in-button {
    font-size: 3.4rem;

    @media (hover: hover) {
        &:hover,
        &:active {
            --hl: var(--filter-hl-0);
        }
    }

    &:active {
        --shdw-dist-elevation: calc(var(--shadow-dist-l) * 0.4);
    }

    &[data-is-pressed=true] {
        --shdw-dist-elevation: 0px;
        --shdw-etc:            var(--shadow-aura);
        --hl: var(--filter-hl-0);
    }
}

.modal-alt-aside {
    font-size:  2.8rem;
    background: var(--col-lt-gray-1);
}

.icon-cloud {
    --wd-cloud: 600px;

    bottom:         -40px;
    left:           calc(50% - var(--wd-cloud) / 2);
    opacity:        90%;
    pointer-events: none;
    position:       absolute;
    width:          var(--wd-cloud);
}

/*
    Bolts
*/

.modal-header::before,
.modal-header::after,
.modal-bracket::after {
    --sz-bolt: 11px;

    content:  '';
    position: absolute;

    z-index: -1;

    width:  var(--sz-bolt);
    height: var(--sz-bolt);
    margin: auto 0;

    background:    var(--col-gray-3);
    border-radius: var(--radius-s);

    box-shadow:
        0 0 0 var(--border-thickness-s) var(--col-gray-4),
        inset var(--shadow-dist-m) var(--shadow-dist-m) var(--col-lt-gray-3);
}

.modal-header::before   { inset: 0 auto 0 14px; }
.modal-header::after    { inset: 0 14px 0 auto }
.modal-bracket--l::after { inset: auto auto 9px 9px; }
.modal-bracket--r::after { inset: auto 9px 9px auto; }
</style>