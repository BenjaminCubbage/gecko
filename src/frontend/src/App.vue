<template>
  <div class="dotted">
    <div class="canvas-container">
      <h1 class="title">
        Gecko!
      </h1>
      <CanvasEditor />
    </div>
  </div>
</template>

<script setup>
  import CanvasEditor from './components/app/CanvasEditor.vue';
</script>

<style scoped>
  .dotted {
    height: 100%;

    display: grid;
    place-items: center;

    --dot-color: var(--subtle);
    background-image: radial-gradient(circle at top,    var(--dot-color) 1.5px, transparent 1.5px),
                      radial-gradient(circle at bottom, var(--dot-color) 1.5px, transparent 1.5px),
                      radial-gradient(circle at right,  var(--dot-color) 1.5px, transparent 1.5px),
                      radial-gradient(circle at left,   var(--dot-color) 1.5px, transparent 1.5px);

    background-size: 36px 36px;
    background-repeat: repeat;
    background-position-x: 50%;
    background-position-y: 50%;
  }

  .title {
    font-family: var(--font-heading);
    font-size: 3rem;
    padding-bottom: 4px;
    margin: 0;
    letter-spacing: 0.04em;
    line-height: 1;
    text-align: center;
  }
</style>
