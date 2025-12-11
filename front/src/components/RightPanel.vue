<script setup>
import { computed, watch } from "vue";
import { useRunsStore } from "../runsStore";
import { useRouter, useRoute } from "vue-router";

const store = useRunsStore();
const router = useRouter();
const route = useRoute();

const activeTab = computed(() => {
  if (route.name === "runs") return "runs";
  if (route.name === "docs") return "docs";
  return "docs";
});

watch(
  () => store.runsArray.length,
  (newLen, oldLen) => {
    if (newLen > oldLen) {
      router.push({ name: "runs" });
    }
  }
);

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
      <div class="tabs">
        <button
          :class="{ active: activeTab === 'runs' }"
          @click="router.push({ name: 'runs' })"
        >
          Runs
        </button>
        <button
          :class="{ active: activeTab === 'docs' }"
          @click="router.push({ name: 'docs', params: { docId: 'index.md' } })"
        >
          Documentation
        </button>
      </div>
      <div v-if="activeTab === 'runs'">
        <button @click="exportJson">Export JSON</button>
        <button @click="closeAll">Close all</button>
      </div>
    </div>
    <div class="content" :class="{ 'no-padding': activeTab === 'docs' }">
      <router-view></router-view>
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
  height: 40px;
  background-origin: padding-box;
  flex-shrink: 0;
}

.tabs button {
  background: none;
  border: none;
  padding: 8px 16px;
  cursor: pointer;
  font-weight: bold;
  color: #666;
}

.tabs button.active {
  color: #000;
  border-bottom: 2px solid #4caf50;
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
.content.no-padding {
  padding: 0;
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
