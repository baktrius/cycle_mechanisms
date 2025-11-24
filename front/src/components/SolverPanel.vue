<script setup>
import { computed, ref } from "vue";
import { useRunsStore } from "../runsStore";

const props = defineProps({
  id: { type: [String, Number], required: true },
});

const store = useRunsStore();
// Ensure the run exists and is started by the store (addRun handles start)
// SolverPanel is a view: it reads run state and invokes store actions.

const run = computed(() => store.getRun(props.id));
const linesLimit = ref(20);
const output = computed(() => run.value?.output ?? []);
const trimmedOutput = computed(() =>
  output.value.slice(-Number(linesLimit.value || 20))
);
const outputText = computed(() => trimmedOutput.value.join("\n"));
const statusText = computed(() => run.value.statusText);
const statusColor = computed(() => run.value.statusColor);

function abort() {
  store.abortRun(props.id);
}

function close() {
  store.closeRun(props.id);
}
</script>
<template>
  <div class="solver-panel" :style="{ backgroundColor: statusColor }">
    <div class="header">
      <span class="args">{{ run?.args?.join(" ") }}</span>
      <span class="status">{{ statusText }}</span>
      <div class="actions">
        <button @click="close">Close</button>
        <button @click="abort" v-if="run?.status === 'running'">Abort</button>
      </div>
    </div>
    <div class="outputWrapper" v-if="output.length > 1">
      <textarea
        id="output"
        :rows="Math.min(trimmedOutput.length, 20)"
        style="resize: vertical"
        readonly
        >{{ outputText }}</textarea
      >
      Showing {{ trimmedOutput.length }}/{{ output.length ?? 0 }} lines (limit
      <input type="number" v-model.number="linesLimit" />).
    </div>
  </div>
</template>

<style scoped>
.args {
  min-width: 40%;
}
.status {
  min-width: 30%;
  flex-grow: 1;
}
.actions {
  min-width: 20%;
}

.actions {
  text-align: right;
}

.solver-panel {
  position: relative;
  padding: 2px;
}
.solver-panel:hover {
  /* darken the current background color no matter its value */
  filter: brightness(0.9);
}

.header {
  display: flex;
  gap: 10px;
}
#output {
  flex-grow: 1;
  width: 100%;
  margin: 0 auto;
  border: none;
  padding: 2px;
  display: block;
  background-color: black;
  color: white;
  font-family: "Lucida Console", Monaco, monospace;
  outline: none;
  box-sizing: border-box;
}
</style>
