<script setup>
import { computed, reactive, watch } from "vue";

const props = defineProps({
  modelValue: {
    type: Object,
    default: () => ({
      mode: "single",
      numbers: [1],
      text: "1",
    }),
  },
  label: {
    type: String,
    default: "",
  },
  min: {
    type: Number,
    default: 1,
  },
});

const emit = defineEmits(["update:modelValue"]);

const state = reactive({
  mode: props.modelValue.mode ?? "single",
  single: props.modelValue.numbers?.[0] ?? props.min,
  rangeStart: props.modelValue.numbers?.[0] ?? props.min,
  rangeEnd:
    props.modelValue.numbers?.[props.modelValue.numbers.length - 1] ??
    props.min,
  rangeStep: 1,
  customText: props.modelValue.text ?? "",
});

function toNumberSafe(v) {
  const n = Number(v);
  return Number.isFinite(n) ? n : null;
}

function buildNumbers() {
  const nums = [];

  if (state.mode === "single") {
    const n = toNumberSafe(state.single);
    return n == null ? [] : [n];
  }

  if (state.mode === "range" || state.mode === "range-step") {
    const start = toNumberSafe(state.rangeStart);
    const end = toNumberSafe(state.rangeEnd);
    const step =
      state.mode === "range-step" ? toNumberSafe(state.rangeStep) : 1;
    if (start == null || end == null || !step || step === 0) return [];
    const dir = start <= end ? 1 : -1;
    const s = dir > 0 ? Math.abs(step) : -Math.abs(step);
    for (let v = start; dir > 0 ? v <= end : v >= end; v += s) {
      nums.push(v);
    }
    return nums;
  }

  if (state.mode === "custom") {
    if (!state.customText.trim()) return [];
    const parts = state.customText.split(/[\s,;]+/).filter(Boolean);
    for (const p of parts) {
      const n = toNumberSafe(p);
      if (n != null) nums.push(n);
    }
    return nums;
  }

  return [];
}

const text = computed(() => {
  const nums = buildNumbers();
  if (!nums.length) return "";

  if (state.mode === "single") {
    return String(nums[0]);
  }
  if (state.mode === "range") {
    return `${nums[0]}..${nums[nums.length - 1]}`;
  }
  if (state.mode === "range-step") {
    const start = nums[0];
    const end = nums[nums.length - 1];
    const step = state.rangeStep || 1;
    return `${start}..${end}:${step}`;
  }
  if (state.mode === "custom") {
    return nums.join(",");
  }
  return "";
});

watch(
  () => ({ ...state }),
  () => {
    const numbers = buildNumbers();
    emit("update:modelValue", {
      mode: state.mode,
      numbers,
      text: text.value,
    });
  },
  { deep: true }
);
</script>

<template>
  <div class="multi-number-input">
    <label v-if="label" class="mni-label">{{ label }}</label>
    <select v-model="state.mode" class="mni-mode" aria-label="mode">
      <option value="single">Single</option>
      <option value="range">Range</option>
      <option value="range-step">Range with step</option>
      <option value="custom">Custom list</option>
    </select>
    <div class="mni-row">
      <template v-if="state.mode === 'single'">
        <input
          type="number"
          v-model.number="state.single"
          class="mni-input"
          :min="min"
        />
      </template>

      <template v-else-if="state.mode === 'range'">
        <input
          type="number"
          v-model.number="state.rangeStart"
          class="mni-input"
          :min="min"
        />
        <span class="dots">..</span>
        <input
          type="number"
          v-model.number="state.rangeEnd"
          class="mni-input"
          :min="min"
        />
      </template>

      <template v-else-if="state.mode === 'range-step'">
        <input
          type="number"
          v-model.number="state.rangeStart"
          class="mni-input"
          :min="min"
        />
        <span class="dots">..</span>
        <input
          type="number"
          v-model.number="state.rangeEnd"
          class="mni-input"
          :min="min"
        />
        <span class="dots">:</span>
        <input
          type="number"
          v-model.number="state.rangeStep"
          class="mni-input mni-step"
          min="1"
        />
      </template>

      <template v-else-if="state.mode === 'custom'">
        <input
          type="text"
          v-model="state.customText"
          class="mni-input mni-custom"
          placeholder="e.g. 1,3,5,10"
        />
      </template>
    </div>
  </div>
</template>

<style scoped>
.multi-number-input {
  display: flex;
  flex-direction: column;
  gap: 4px;
}

.mni-row {
  display: flex;
  gap: 8px;
  align-items: center;
  width: 100%;
  flex-wrap: wrap;
}

.mni-mode {
  padding: 2px 2px;
}

.mni-input {
  width: 60px;
  flex: 1 1 auto;
}

.mni-step {
  width: 40px;
}

.mni-custom {
  width: 60px;
  flex: 1 1 auto;
}

.dots {
  padding: 0 4px;
}
</style>
