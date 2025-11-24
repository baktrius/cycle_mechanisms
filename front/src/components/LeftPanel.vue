<script setup>
import Mechanism from "./Mechanism.vue";
import { ref, computed, toRaw } from "vue";
import { useRunsStore } from "../runsStore.js";
import { configToArgs } from "../configToArgs.js";

const store = useRunsStore();

const mechanism = ref({});
const numOfVertices = ref(4);
const numOfVertices2 = ref(4);
const numOfAgents = ref(3);
const numOfAgents2 = ref(3);
const task = ref("D");
const statesType = ref("all");
const numDistinctVotes = ref(1);
const verbosity = ref("V1");
const calculationsLimit = ref(1000);
const calculationsLimitEnabled = ref(false);
const multirunsEnabled = ref(false);

const numOfVerticesPair = computed(() => {
  let nv = numOfVertices.value || 1;
  let nv2 = multirunsEnabled.value ? numOfVertices2.value || 1 : nv;
  return [nv, nv2].sort((a, b) => a - b);
});
const numOfAgentsPair = computed(() => {
  let na = numOfAgents.value || 1;
  let na2 = multirunsEnabled.value ? numOfAgents2.value || 1 : na;
  return [na, na2].sort((a, b) => a - b);
});

const numOfRuns = computed(() => {
  return (
    (numOfAgentsPair.value[1] - numOfAgentsPair.value[0] + 1) *
    (numOfVerticesPair.value[1] - numOfVerticesPair.value[0] + 1)
  );
});

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
  const agentsTok = rangeString(...numOfAgentsPair.value);
  const verticesTok = rangeString(...numOfVerticesPair.value);
  return buildArgs(agentsTok, verticesTok);
});

function runSolver() {
  const ids = [];
  for (let a = numOfAgentsPair.value[0]; a <= numOfAgentsPair.value[1]; a++) {
    const row = [];
    for (
      let v = numOfVerticesPair.value[0];
      v <= numOfVerticesPair.value[1];
      v++
    ) {
      row.push(store.addRun(buildArgs(a, v)));
    }
    ids.push(row);
  }
  if (numOfRuns.value > 1 && verbosity.value === "V1") {
    store.addOverview(
      argsPreview.value,
      range(...numOfAgentsPair.value),
      range(...numOfVerticesPair.value),
      ids
    );
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

      <div class="form-group" v-if="!multirunsEnabled">
        <label for="vertices">Number of Vertices</label>
        <input id="vertices" type="number" v-model="numOfVertices" min="1" />
      </div>
      <div class="form-group" v-else>
        <label for="vertices-1">Number of Vertices</label>
        <div class="range-row">
          <input
            id="vertices-1"
            type="number"
            v-model.number="numOfVertices"
            min="1"
          />
          <span class="dots">..</span>
          <input
            id="vertices-2"
            type="number"
            v-model.number="numOfVertices2"
            min="1"
          />
        </div>
      </div>

      <div class="form-group" v-if="!multirunsEnabled">
        <label for="agents">Number of Agents</label>
        <input id="agents" type="number" v-model="numOfAgents" min="1" />
      </div>
      <div class="form-group" v-else>
        <label for="agents-1">Number of Agents</label>
        <div class="range-row">
          <input
            id="agents-1"
            type="number"
            v-model.number="numOfAgents"
            min="1"
          />
          <span class="dots">..</span>
          <input
            id="agents-2"
            type="number"
            v-model.number="numOfAgents2"
            min="1"
          />
        </div>
      </div>

      <div class="form-group">
        <div class="label-row">
          <label for="multiruns">Enable multiruns</label>
          <input
            id="multiruns"
            type="checkbox"
            v-model="multirunsEnabled"
            style="width: auto"
          />
        </div>
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
