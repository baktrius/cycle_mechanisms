<script setup>
import SolverPanel from "./SolverPanel.vue";
import { computed } from "vue";
import { useRunsStore } from "../runsStore";

const store = useRunsStore();
const reversedRuns = computed(() => [...store.runsArray].reverse());

function closeAll() {
  store.clearRuns();
}
</script>

<template>
  <div class="right-panel-content">
    <div
      style="display: flex; align-items: center; justify-content: space-between"
    >
      <h3>Runs</h3>
      <div>
        <button @click="closeAll">Close all</button>
      </div>
    </div>
    <div v-for="{ id } in reversedRuns" :key="id">
      <SolverPanel :id="id" />
    </div>
  </div>
</template>

<style scoped>
.right-panel-content {
  padding: 1em;
  padding-top: 0;
}
</style>
