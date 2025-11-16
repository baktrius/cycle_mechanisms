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
const trimmedOutput = computed(() => {
  const out = run.value?.output ?? [];
  return out.slice(-Number(linesLimit.value || 20));
});
const outputText = computed(() => trimmedOutput.value.join("\n"));

function abort() {
  store.abortRun(props.id);
}

function close() {
  store.closeRun(props.id);
}
</script>
<template>
  <div class="solver-panel">
    <div class="header">
      {{ run?.args?.join(" ") }}
      {{ run?.status ?? "unknown" }}
      <button @click="close">Close</button>
      <button @click="abort" v-bind:disabled="run?.status !== 'running'">
        Abort
      </button>
      Showing {{ trimmedOutput.length }}/{{ run?.output?.length ?? 0 }} lines
      (limit <input type="number" v-model.number="linesLimit" />).
    </div>
    <div class="output">
      <textarea
        id="output"
        :rows="Math.min(trimmedOutput.length, 20)"
        style="resize: vertical"
        readonly
        >{{ outputText }}</textarea
      >
    </div>
  </div>
</template>

<style scoped>
.solver-panel {
  border: 1px solid #ccc;
  padding: 5px;
  border-radius: 4px;
  margin-bottom: 1em;
}
#output {
  flex-grow: 1;
  width: 100%;
  margin: 0 auto;
  border-left: 0px;
  border-right: 0px;
  padding-left: 0px;
  padding-right: 0px;
  display: block;
  background-color: black;
  color: white;
  font-family: "Lucida Console", Monaco, monospace;
  outline: none;
}
</style>
