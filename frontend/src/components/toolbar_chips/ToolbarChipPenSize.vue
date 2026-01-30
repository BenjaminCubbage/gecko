<template>
    <ToolbarChipLayout>
        <template #pad>
            <div class="paper-sheet"></div>
        </template>

        <template #icons>
            <!-- Small -->
            <button
                class="option"
                :class="{ 'selected': selectedPenSize == 'small' }"
                @click="selectedPenSize = 'small'">
                <div class="icon" style="--size: 10px">
                    <div class="ink"></div>
                    <div class="nib">
                        <StrokedText strokeColor="black" strokeThickness="5px" allowOverflow>
                            <i class="icon-text hn hn-pen-nib-solid"></i>
                        </StrokedText>
                    </div>
                </div>
            </button>

            <!-- Medium -->
            <button
                class="option"
                :class="{ 'selected': selectedPenSize == 'medium' }"
                @click="selectedPenSize = 'medium'">
                <div class="icon" style="--size: 12px">
                    <div class="ink"></div>
                    <div class="nib">
                        <StrokedText strokeColor="black" strokeThickness="5px" allowOverflow>
                            <i class="icon-text hn hn-pen-nib-solid"></i>
                        </StrokedText>
                    </div>
                </div>
            </button>

            <!-- Large -->
            <button
                class="option"
                :class="{ 'selected': selectedPenSize == 'large' }"
                @click="selectedPenSize = 'large'">
                <div class="icon" style="--size: 15px">
                    <div class="ink"></div>
                    <div class="nib">
                        <StrokedText strokeColor="black" strokeThickness="5px" allowOverflow>
                            <i class="icon-text hn hn-pen-nib-solid"></i>
                        </StrokedText>
                    </div>
                </div>
            </button>
        </template>
    </ToolbarChipLayout>
</template>

<script setup>
import ToolbarChipLayout from './ToolbarChipLayout.vue';
import StrokedText       from '@/components/stroked_text/StrokedText.vue';

const selectedPenSize = defineModel({ default: 'small' });
</script>

<style scoped>
.paper-sheet {
    height:   calc(100% + 3px);
    position: relative;
    top:      -1.5px;
    width:    140px;
    z-index:  0;

    border:        var(--border-s);
    border-radius: var(--radius-s);

    box-shadow:
        inset 0 2px 0 white,
        var(--shadow-s);

    background: rgb(242, 242, 245);

    transform: perspective(500px) rotateX(26deg);
}

.options {
    display:          flex;
    flex-flow:        row nowrap;
    justify-content:  space-evenly;
    z-index:          1;
}

.option {
    align-self: stretch;
    display:    flex;
    height:     100%;
}

.icon {
    display:     grid;
    font-size:   2.7rem;
    line-height: 0;
}

.nib,
.ink {
    grid-area: 1 / 1;
}

.nib {
    transform:  translate(12.5px, -3.5px);
    transition: transform 50ms ease;
}

.ink {
    height:      calc(var(--size) * 0.8);
    place-self:  center;
    width:       var(--size);

    background: black;

    border-radius: 50%;

    transform: translate(0, 6.5px);
}

.icon-text {
    background:
        linear-gradient(45deg,
            white             63%,
            black             63% 70%,
            var(--col-orange-4) 70%);

    color:           transparent;
    background-clip: text;
}

.option:not(.selected) .nib {
    opacity:   0.3;
    transform: translate(13px, -4px) rotate(5deg);
}

.option.selected > .icon {
    --s: 1px;
    filter: drop-shadow(calc(var(--s) *  1.0) calc(var(--s) *  0.0) 0px white)
            drop-shadow(calc(var(--s) *  1.0) calc(var(--s) *  1.0) 0px white)
            drop-shadow(calc(var(--s) *  0.0) calc(var(--s) *  1.0) 0px white)
            drop-shadow(calc(var(--s) * -0.8) calc(var(--s) *  1.0) 0px white)
            drop-shadow(calc(var(--s) * -0.8) calc(var(--s) *  0.0) 0px white)
            drop-shadow(calc(var(--s) * -0.8) calc(var(--s) * -0.8) 0px white)
            drop-shadow(calc(var(--s) *  0.0) calc(var(--s) * -0.8) 0px white)
            drop-shadow(calc(var(--s) *  1.0) calc(var(--s) * -0.8) 0px white);
}
</style>