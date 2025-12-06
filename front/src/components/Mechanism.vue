<script setup>
import { computed, effect } from "vue";
// Register the component for recursion
defineOptions({ name: "Mechanism" });

const mechanism = defineModel({ required: true });

const MECHANISM_TYPES = {
  opt: { label: "opt" },
  pcd: { label: "pcd" },
  rd: { label: "rd" },
  qcd: { label: "qcd", schema: { threshold: 0 } },
  sqcd: { label: "sqcd" },
  dbl: { label: "dbl", schema: { exponent: 1 } },
  r3pcd: { label: "r3pcd" },
  mix: { label: "mix", schema: { left: {}, right: {}, ratio: 0.5 } },
  rdsp: { label: "rdsp", schema: { left: {} } },
};

effect(() => {
  if (mechanism.value.type === undefined) {
    mechanism.value.type = "rd";
  }
  const config = MECHANISM_TYPES[mechanism.value.type]?.schema || {};
  // Only merge if config introduces any new top-level keys not present on mechanism.value
  const missing = Object.keys(config).some((key) => !(key in mechanism.value));
  if (missing) {
    for (const key of Object.keys(config)) {
      if (!(key in mechanism.value)) {
        mechanism.value[key] = config[key];
      }
    }
  }
});
</script>

<template>
  <div class="box">
    <span class="mech-type-select">
      <label for="mech-type">Type: </label>
      <select id="mech-type" v-model="mechanism.type">
        <option v-for="(meta, key) in MECHANISM_TYPES" :key="key" :value="key">
          {{ meta.label }}
        </option>
      </select>
    </span>
    <span v-if="mechanism.type === 'qcd'">
      <label for="qcd-threshold"> Threshold: </label>
      <input
        id="qcd-threshold"
        v-model="mechanism.threshold"
        type="number"
        step="0.05"
        size="5"
        min="0"
        max="1"
      />
    </span>
    <span v-if="mechanism.type === 'dbl'">
      <label for="dbl-exponent"> Exponent: </label>
      <input
        id="dbl-exponent"
        v-model="mechanism.exponent"
        type="number"
        step="0.1"
        size="5"
      />
    </span>
    <ul v-else-if="mechanism.type === 'mix'">
      <li>
        <input
          id="mix-ratio"
          v-model="mechanism.ratio"
          type="number"
          step="0.1"
          size="5"
          min="0"
          max="1"
        />
        *
        <Mechanism v-model="mechanism.left" />
      </li>
      <li>
        <input
          id="mix-ratio"
          :value="1 - mechanism.ratio"
          type="number"
          disabled
          size="5"
        />
        *
        <Mechanism v-model="mechanism.right" />
      </li>
    </ul>
    <ul v-if="mechanism.type === 'rdsp'">
      <li><Mechanism v-model="mechanism.left" /></li>
    </ul>
  </div>
</template>

<style scoped>
h3 {
  margin-bottom: 0.5em;
}

.box {
  border: 1px solid #ccc;
  padding: 4px;
  border-radius: 4px;
  background-color: #f9f9f9;
  display: inline-block;
}
ul {
  padding-left: 20px;
}
input[type="number"] {
  width: 50px;
}
</style>
