<script setup>
import SolverPanel from "./SolverPanel.vue";
import OverviewPanel from "./OverviewPanel.vue";
import { computed } from "vue";
import { useRunsStore } from "../runsStore";

const store = useRunsStore();
const reversedRuns = computed(() => [...store.runsArray].reverse());

function exportJson() {
  const data = store.exportRuns();
  const json = JSON.stringify(data, null, 2);
  const blob = new Blob([json], { type: "application/json" });
  const url = URL.createObjectURL(blob);
  const a = document.createElement("a");
  a.href = url;
  a.download = `runs-${new Date().toISOString()}.json`;
  a.click();
  URL.revokeObjectURL(url);
}

function closeAll() {
  store.clearRuns();
}
</script>

<template>
  <div class="right-panel-content">
    <div class="header">
      <h3>Runs</h3>
      <div>
        <button @click="exportJson">Export JSON</button>
        <button @click="closeAll">Close all</button>
      </div>
    </div>
    <div class="content">
      <div v-for="{ id, type } in reversedRuns" :key="id">
        <div class="entry-wrapper">
          <span class="dot">●</span>
          <div class="entry">
            <SolverPanel :id="id" v-if="type === 'run'" />
            <OverviewPanel :id="id" v-else-if="type === 'overview'" />
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped>
h3 {
  margin: 5px 0;
}
.header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding-left: 1em;
  padding-right: 1em;
  border-bottom: 1px solid #ccc;
}

.right-panel-content {
  display: flex;
  flex-direction: column;
  height: 100%;
  box-sizing: border-box;
}
.content {
  padding-left: 1em;
  padding-right: 1em;
  flex-grow: 1;
  overflow: auto;
}
.entry-wrapper {
  display: flex;
}
.entry {
  flex-grow: 1;
}
.dot {
  padding: 0 2px;
}
</style>
