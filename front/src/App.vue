<script setup>
import LeftPanel from "./components/LeftPanel.vue";
import RightPanel from "./components/RightPanel.vue";
import { ref, onMounted, onBeforeUnmount } from "vue";

const leftPanelWidth = ref(400);
let dragging = false;
let startX = 0;
let startWidth = 0;

function startDrag(e) {
  dragging = true;
  startX = e.clientX;
  startWidth = leftPanelWidth.value;
  document.body.style.cursor = "col-resize";
}

function onDrag(e) {
  if (!dragging) return;
  const dx = e.clientX - startX;
  leftPanelWidth.value = Math.max(200, startWidth + dx);
}

function stopDrag() {
  dragging = false;
  document.body.style.cursor = "";
}

onMounted(() => {
  window.addEventListener("mousemove", onDrag);
  window.addEventListener("mouseup", stopDrag);
});
onBeforeUnmount(() => {
  window.removeEventListener("mousemove", onDrag);
  window.removeEventListener("mouseup", stopDrag);
});
</script>

<template>
  <div class="split-layout">
    <div
      class="split-panel left-panel"
      :style="{ width: leftPanelWidth + 'px' }"
    >
      <LeftPanel />
    </div>
    <div class="split-divider" @mousedown="startDrag"></div>
    <div class="split-panel right-panel">
      <RightPanel />
    </div>
  </div>
</template>

<style scoped>
.split-layout {
  display: flex;
  height: 100vh;
  width: 100vw;
  overflow: hidden;
}
.split-panel {
  height: 100%;
  overflow: auto;
}
.left-panel {
  background: #fafbfc;
  min-width: 200px;
  max-width: 80vw;
  transition: width 0.1s;
}
.right-panel {
  flex: 1;
  background: #fff;
}
.split-divider {
  width: 6px;
  background: #e0e0e0;
  cursor: col-resize;
  z-index: 10;
  height: 100%;
  position: relative;
}
</style>
