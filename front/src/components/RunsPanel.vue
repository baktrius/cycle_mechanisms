<script setup>
import SolverPanel from "./SolverPanel.vue";
import OverviewPanel from "./OverviewPanel.vue";
import { computed } from "vue";
import { useRunsStore } from "../runsStore";

const store = useRunsStore();
const reversedRuns = computed(() => [...store.runsArray].reverse());
</script>

<template>
  <div class="runs-list">
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
</template>

<style scoped>
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
