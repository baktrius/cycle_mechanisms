<script setup>
import Mechanism from "./Mechanism.vue";
import { ref, computed } from "vue";
import { useRunsStore } from "../runsStore.js";

const store = useRunsStore();

const mechanism = ref({});
const numOfVertices = ref(4);
const numOfVertices2 = ref(4);
const numOfAgents = ref(3);
const numOfAgents2 = ref(3);
const task = ref("D");
const statesType = ref("all");
const numDistinctVotes = ref(1);
const verbosity = ref("S");
const calculationsLimit = ref(0);
const multirunsEnabled = ref(false);

function mechanismToArgs(mech) {
  const res = [];
  if (mech.type === "mix") {
    res.push(...mechanismToArgs(mech.right));
    res.push(`M${mech.ratio}`);
    res.push(...mechanismToArgs(mech.left));
    return res;
  }
  res.push(mech.type);
  if (mech.type === "qcd") {
    res.push(mech.threshold ?? 0);
  } else if (mech.type === "dbl") {
    res.push(mech.exponent ?? 1);
  }
  return res;
}

function statesTypeToArgs() {
  if (statesType.value === "I") {
    return ["I"];
  } else if (statesType.value === "J") {
    return [`J${numDistinctVotes.value}`];
  }
  return [];
}

function buildArgs(nAgents, nVertices) {
  return [
    `N${nAgents}`,
    task.value,
    verbosity.value,
    ...statesTypeToArgs(),
    nVertices,
    ...mechanismToArgs(mechanism.value),
    ...(Number(calculationsLimit.value) > 0
      ? [`L${calculationsLimit.value}`]
      : []),
  ];
}

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

function rangeString(min, max) {
  return min === max ? `${min}` : `${min}..${max}`;
}

const argsPreview = computed(() => {
  const agentsTok = rangeString(...numOfAgentsPair.value);
  const verticesTok = rangeString(...numOfVerticesPair.value);
  return buildArgs(agentsTok, verticesTok);
});
function runSolver() {
  for (let a = numOfAgentsPair.value[0]; a <= numOfAgentsPair.value[1]; a++) {
    for (
      let v = numOfVerticesPair.value[0];
      v <= numOfVerticesPair.value[1];
      v++
    ) {
      store.addRun(buildArgs(a, v));
    }
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
          <option value="S">Summary</option>
          <option value="V3">All</option>
          <!-- <option value="V1">Inline Answer</option> -->
          <option value="V0">None</option>
        </select>
      </div>

      <div class="form-group">
        <div class="label-row">
          <label for="calculations-limit">Calculations limit</label>
          <div
            class="info"
            tabindex="0"
            role="button"
            aria-label="Calculations limit info"
          >
            i
            <div class="tooltip">
              Set to 0 for no limit. When &gt;0 the frontend sends an
              <code>L&lt;n&gt;</code> flag to the solver.
            </div>
          </div>
        </div>
        <input
          id="calculations-limit"
          type="number"
          v-model.number="calculationsLimit"
          min="0"
        />
      </div>

      <label for="args">Args</label>
      <input type="text" :value="argsPreview.join(' ')" readonly id="args" />
      <label for="warnings">Warnings</label>
      <textarea rows="10" readonly id="warnings"></textarea>
      <button @click="runSolver">Run Solver</button>
    </fieldset>
  </form>
</template>

<style scoped>
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
.info {
  position: relative;
  width: 18px;
  height: 18px;
  border-radius: 50%;
  background: #e0e0e0;
  color: #000;
  display: inline-flex;
  align-items: center;
  justify-content: center;
  font-weight: 600;
  cursor: default;
}
.info:focus {
  outline: 2px solid rgba(0, 0, 0, 0.2);
}
.info .tooltip {
  display: none;
  position: absolute;
  right: 0;
  top: calc(100% + 8px);
  background: rgba(0, 0, 0, 0.85);
  color: white;
  padding: 6px 8px;
  border-radius: 4px;
  font-size: 12px;
  z-index: 20;
  width: 100px;
}
.info:hover .tooltip,
.info:focus .tooltip {
  display: block;
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
