import { defineStore } from 'pinia';
import { createRun } from './createRun';

// Pinia store managing a list of runs created by the per-run composable.
export const useRunsStore = defineStore('runs', {
    state: () => ({
        runs: new Map(), // Map<id, run>
        nextRunId: 0,
        queue: [], // FIFO queue of run ids waiting to start
        activeCount: 0,
        concurrencyLimit: 8,
    }),
    getters: {
        getRun: (state) => (id) => state.runs.get(id),
        runsArray: (state) => Array.from(state.runs.values()),
    },
    actions: {
        addRun(args) {
            const id = this.nextRunId++;
            const run = createRun(id, args);
            this.runs.set(id, run);
            // Enqueue and attempt to start if under the limit
            this.queue.push(id);
            this.maybeStartNext();
            return id;
        },
        maybeStartNext() {
            while (this.activeCount < this.concurrencyLimit && this.queue.length > 0) {
                const nextId = this.queue.shift();
                const run = this.getRun(nextId);
                if (!run) continue; // It might have been removed
                if (run.status !== 'queued') continue;
                this.activeCount++;
                // Start and when finished, decrement and continue
                run.start().finally(() => {
                    --this.activeCount;
                    this.maybeStartNext();
                });
            }
        },
        removeFromQueue(id) {
            if (!this.queue.length) return;
            this.queue = this.queue.filter((qid) => qid !== id);
        },
        abortRun(id) {
            const run = this.getRun(id);
            if (!run) return;
            run.abort();
        },
        closeRun(id) {
            const run = this.getRun(id);
            if (!run) return;
            if (run.status === 'queued') {
                this.removeFromQueue(id);
                run.dispose();
            }
            run.abort();
            run.dispose();

            this.runs.delete(id);
        },
        clearRuns() {
            for (const run of this.runs.values()) {
                run.abort();
                run.dispose();
            }
            this.queue = [];
            this.runs = new Map();
        },
    }
});
