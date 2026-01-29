<template>
    <div class="pic-editor-pen-size">
        <div class="paper-sheet"></div>
        <div class="options">
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
                <div class="icon" style="--size: 13px">
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
                <div class="icon" style="--size: 18px">
                    <div class="ink"></div>
                    <div class="nib">
                        <StrokedText strokeColor="black" strokeThickness="5px" allowOverflow>
                            <i class="icon-text hn hn-pen-nib-solid"></i>
                        </StrokedText>
                    </div>
                </div>
            </button>
        </div>
    </div>
</template>

<script setup>
import StrokedText from '@/components/stroked_text/StrokedText.vue';

const selectedPenSize = defineModel({ default: 'small' });
</script>

<style scoped>
.pic-editor-pen-size {
    display: grid;
    z-index: 0;
}

.paper-sheet, .options {
    grid-area: 1 / 1;
}

.paper-sheet {
    align-self: end;

    width: 140px;
    height: 27px;
    border: var(--border-small);
    border-radius: var(--border-radius-small);
    box-shadow: inset 0 2px 0 white,
                var(--border-shadow-small);

    background: rgb(242, 242, 245);

    transform: perspective(500px) rotateX(30deg) translate(0, 0.3px);
    z-index: 0;
}

.options {
    display: flex;
    flex-flow: row nowrap;
    justify-content: space-evenly;

    z-index: 1;
}

.option {
    align-self: stretch;
    height: 100%;
    display: flex;
}

.icon {
    display: grid;

    font-size: 2.7rem;
    line-height: 0;
}

.nib, .ink {
    grid-area: 1 / 1;
}

.nib {
    transform: translate(11px, -3px);
}

.ink {
    background: black;
    width: var(--size);
    height: calc(var(--size) * 0.8);
    place-self: center;

    border-radius: 50%;
    transform: translate(0, 5px);
}

.icon-text {
    color: transparent;
    background: linear-gradient(45deg, white 63%, black 63% 70%, rgb(240, 131, 6) 70%);
    background-clip: text;
}

.option:not(.selected) .nib {
    opacity: 0;
}
</style>