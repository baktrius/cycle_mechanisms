import { reactive, toRaw, computed } from 'vue';
import { main } from './solver.js';

// Factory for a single run: encapsulates lifecycle and output buffering.
// Buffer and timer live in closure (non-reactive) for performance.
export function createRun(id, args, { throttleMs = 100 } = {}) {
    const run = reactive({
        type: "run",
        id,
        args: [...args],
        _status: 'queued',
        reportedStatus: 'queued',
        output: [],
        abortController: new AbortController(),
        exitCode: null,
    });

    // Non-reactive internals
    let buffer = [];
    let throttleTimer = null;

    function flush() {
        if (buffer.length) {
            run.output.push(...buffer);
            buffer = [];
        }
        throttleTimer = null;
    }

    function print(...parts) {
        const text = parts.join(' ');
        buffer.push(text);
        if (!throttleTimer) {
            throttleTimer = setTimeout(flush, throttleMs);
        }
    }

    function setStatus(status) {
        run.reportedStatus = status.join(' ');
    }

    async function start() {
        if (run._status !== 'queued') return;
        run._status = 'running';
        try {
            run.exitCode = await main(toRaw(run.args), print, run.abortController.signal, setStatus);
            if (run._status === 'running') run._status = 'completed';
        } catch (err) {
            if (run._status !== 'aborted') {
                run._status = 'error';
                print('Error:', err?.message ?? String(err));
            }
        } finally {
            flush();
        }
    }

    function abort() {
        if (run._status === 'running') {
            run.abortController.abort();
            run._status = 'aborted';
        }
    }

    function dispose() {
        if (throttleTimer) {
            clearTimeout(throttleTimer);
            throttleTimer = null;
        }
        buffer = [];
    }

    function exportData() {
        return {
            id: run.id,
            args: toRaw(run.args),
            status: status.value,
            answer: answer.value,
            exitCode: run.exitCode,
            output: toRaw(run.output),
        };
    }

    const status = computed(() => {
        const res = run._status ?? "unknown";
        if (res !== "completed") {
            return res;
        }
        switch (run.exitCode) {
            case 0:
                return "completed";
            case 2:
                return "skipped";
            case -1:
                return "aborted";
            default:
                return "error";
        }
    });

    const statusText = computed(() => {
        if (status.value === "completed" && run.output.length === 1) {
            return `answer ${run.output[0]}`;
        }
        return status.value;
    });
    const ANSWER_MAP = {
        running: "...",
        aborted: "X",
        error: "!",
        queued: "",
        unknown: "?",
        skipped: "-",
    };
    const answer = computed(() => {
        if (status.value === "completed") {
            if (run.output.length === 1)
                return `${run.output[0]}`;
            else if (run.output.length > 1) return '#';
            else return '-';
        }
        return ANSWER_MAP[status.value] ?? "?";
    });
    const COLORS = {
        // more saturated / vivid colors
        running: "#1E90FF", // dodger blue
        completed: "#00C853", // vivid green
        aborted: "#FF6D00", // vivid orange
        error: "#D50000", // vivid red
        queued: "#FFD600", // vivid yellow
        unknown: "#9E9E9E", // neutral gray
        skipped: "#AA00FF", // vivid purple
    };
    const statusColor = computed(() => COLORS[status.value] || "gray");

    return Object.assign(run, {
        start,
        abort,
        dispose,
        exportData,
        status,
        statusText,
        statusColor,
        answer,
    });
}
