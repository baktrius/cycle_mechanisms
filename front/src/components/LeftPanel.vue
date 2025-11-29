<script setup>
import Mechanism from "./Mechanism.vue";
import MultiNumberInput from "./MultiNumberInput.vue";
import { ref, computed, toRaw } from "vue";
import { useRunsStore } from "../runsStore.js";
import { configToArgs } from "../configToArgs.js";

const store = useRunsStore();

const mechanism = ref({});
const verticesInput = ref({
  mode: "single",
  numbers: [4],
  text: "4",
});
const agentsInput = ref({
  mode: "single",
  numbers: [3],
  text: "3",
});
const task = ref("D");
const statesType = ref("all");
const numDistinctVotes = ref(1);
const verbosity = ref("V1");
const calculationsLimit = ref(1000);
const calculationsLimitEnabled = ref(false);

const numOfRuns = computed(
  () => verticesInput.value.numbers.length * agentsInput.value.numbers.length
);

const runButtonText = computed(() => {
  return numOfRuns.value > 1
    ? `Run Solver (${numOfRuns.value} runs)`
    : "Run Solver";
});

function rangeString(min, max) {
  return min === max ? `${min}` : `${min}..${max}`;
}

// return an array of integers between start and end (inclusive)
function range(start, end) {
  const s = Number(start);
  const e = Number(end);
  if (!Number.isFinite(s) || !Number.isFinite(e)) return [];
  const step = s <= e ? 1 : -1;
  const res = [];
  for (let i = s; step > 0 ? i <= e : i >= e; i += step) {
    res.push(i);
  }
  return res;
}

function buildConfig(numAgents, numVertices) {
  return {
    numAgents: numAgents,
    numVertices: numVertices,
    task: task.value,
    verbosity: verbosity.value,
    statesType: statesType.value,
    numDistinctVotes: numDistinctVotes.value,
    calculationsLimit: calculationsLimitEnabled.value
      ? calculationsLimit.value
      : -1,
    mechanism: structuredClone(toRaw(mechanism.value)),
  };
}

function buildArgs(numAgents, numVertices) {
  return configToArgs(buildConfig(numAgents, numVertices));
}

const argsPreview = computed(() => {
  return buildArgs(agentsInput.value.text, verticesInput.value.text);
});

function runSolver() {
  const ids = [];
  const agents = agentsInput.value.numbers;
  const vertices = verticesInput.value.numbers;

  for (const a of agents) {
    const row = [];
    for (const v of vertices) {
      row.push(store.addRun(buildArgs(a, v)));
    }
    ids.push(row);
  }
  if (numOfRuns.value > 1 && verbosity.value === "V1") {
    store.addOverview(argsPreview.value, agents, vertices, ids);
  }
}
</script>

<template>
  <form class="mechanism-form" @submit.prevent>
    <fieldset>
      <legend><h3>Mechanism Editor</h3></legend>

      <div class="form-group">
        <label>Mechanism</label>
        <Mechanism v-model="mechanism" />
      </div>

      <div class="form-group">
        <MultiNumberInput
          v-model="verticesInput"
          label="Number of Vertices"
          :min="1"
        />
      </div>

      <div class="form-group">
        <MultiNumberInput
          v-model="agentsInput"
          label="Number of Agents"
          :min="1"
        />
      </div>

      <div class="form-group">
        <label for="statesType">States of the world</label>
        <select id="statesType" v-model="statesType">
          <option value="all">All</option>
          <option value="I">Not reversed</option>
          <option value="J">With limited number of distinct votes</option>
        </select>
      </div>

      <div class="form-group" v-if="statesType === 'J'">
        <label for="num-distinct-votes">Number of distinct votes</label>
        <input
          id="num-distinct-votes"
          type="number"
          v-model="numDistinctVotes"
          min="1"
        />
      </div>

      <div class="form-group">
        <label for="task">Task</label>
        <select id="task" v-model="task">
          <option value="D">Check Strategyproofness</option>
          <option value="A">Calculate Approximation Ratio</option>
          <option value="B">Calculate RD ratio</option>
          <option value="C">Calculate complexity</option>
        </select>
      </div>
      <div class="form-group">
        <label for="verbosity">Verbosity</label>
        <select id="verbosity" v-model="verbosity">
          <option value="V1">Inline Answer</option>
          <option value="S">Summary</option>
          <option value="V3">All</option>
          <option value="V0">None</option>
        </select>
      </div>

      <div class="form-group">
        <div class="label-row">
          <label for="calculations-limit">Calculations limit</label>
          <div style="display: flex; align-items: center; gap: 8px">
            <label
              style="
                font-weight: normal;
                display: flex;
                align-items: center;
                gap: 6px;
              "
            >
              <input type="checkbox" v-model="calculationsLimitEnabled" />
              Enable
            </label>
          </div>
        </div>
        <input
          id="calculations-limit"
          type="number"
          v-model.number="calculationsLimit"
          min="1"
          v-if="calculationsLimitEnabled"
        />
      </div>

      <label for="args">Resulting Args</label>
      <input type="text" :value="argsPreview.join(' ')" readonly id="args" />
      <button @click="runSolver">{{ runButtonText }}</button>
    </fieldset>
  </form>
</template>

<style scoped>
h3 {
  margin: 5px 0;
}
label {
  display: block;
}

input,
textarea,
select,
button {
  padding: 2px;
  width: 100%;
  box-sizing: border-box;
}
form > fieldset > input,
form > fieldset > select,
form > fieldset > button {
  margin-bottom: 8px;
}

fieldset {
  border: none;
}
textarea {
  resize: vertical;
}

.label-row {
  display: flex;
  align-items: center;
  justify-content: space-between;
}

.range-row {
  display: flex;
  align-items: center;
  gap: 8px;
}
.range-row input[type="number"] {
  flex: 1 1 0;
}
.dots {
  padding: 0 4px;
}
</style>
