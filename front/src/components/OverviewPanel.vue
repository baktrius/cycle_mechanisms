<script setup>
import { computed, ref } from "vue";
import { useRunsStore } from "../runsStore";

const props = defineProps({
  id: {
    type: Number,
    required: true,
  },
});

const store = useRunsStore();

const overview = computed(() => store.getRun(props.id));

const xs = computed(() => overview.value?.xs ?? []);
const ys = computed(() => overview.value?.ys ?? []);
const ids = computed(() => overview.value?.ids ?? []);

// Track the currently hovered cell so we can highlight
// its entire row and column.
const hoveredX = ref(null);
const hoveredY = ref(null);

function cellRun(xIndex, yIndex) {
  const row = ids.value[yIndex];
  if (!row) return null;
  const runId = row[xIndex];
  if (runId == null) return null;
  return store.getRun(runId);
}

const argsText = computed(() => overview.value?.args ?? "");
</script>

<template>
  <div class="overview-panel">
    <div class="overview-header">
      <div class="overview-args-value">{{ argsText.join(" ") }}</div>
    </div>

    <div class="table-wrapper" v-if="xs.length && ys.length">
      <table class="overview-table">
        <thead>
          <tr>
            <th>V\N</th>
            <th
              v-for="(x, xi) in xs"
              :key="`x-${xi}`"
              :class="{ 'is-col-hovered': hoveredX === xi }"
            >
              {{ x }}
            </th>
          </tr>
        </thead>
        <tbody>
          <tr v-for="(y, yi) in ys" :key="`y-${yi}`">
            <th :class="{ 'is-row-hovered': hoveredY === yi }">{{ y }}</th>
            <td
              v-for="(x, xi) in xs"
              :key="`cell-${xi}-${yi}`"
              class="status-cell"
              :class="{
                'is-row-hovered': hoveredY === yi,
                'is-col-hovered': hoveredX === xi,
              }"
              @mouseenter="
                () => {
                  hoveredX = xi;
                  hoveredY = yi;
                }
              "
              @mouseleave="
                () => {
                  hoveredX = null;
                  hoveredY = null;
                }
              "
              :style="{ backgroundColor: cellRun(yi, xi)?.statusColor }"
            >
              {{ cellRun(yi, xi)?.answer }}
            </td>
          </tr>
        </tbody>
      </table>
    </div>

    <div v-else class="empty-message">No overview data.</div>
  </div>
</template>

<style scoped>
.overview-header {
  display: grid;
  grid-template-columns: auto 1fr;
  gap: 8px;
  align-items: center;
}

.table-wrapper {
  overflow: auto;
}

.overview-table {
  border-collapse: collapse;
  min-width: 100%;
}

.overview-table th,
.overview-table td {
  border: 1px solid #ddd;
  padding: 4px 6px;
  text-align: center;
  font-size: 0.85rem;
}

th {
  background: #f0f0f0;
  font-weight: 600;
}

.status-cell {
  white-space: nowrap;
}

/* Slightly darker background for hovered row/column */
.is-row-hovered,
.is-col-hovered {
  filter: brightness(0.9);
}

.empty-message {
  color: #666;
  font-size: 0.9rem;
}
</style>
