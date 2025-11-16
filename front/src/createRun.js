import { reactive, toRaw } from 'vue';
import { main } from './solver.js';

// Factory for a single run: encapsulates lifecycle and output buffering.
// Buffer and timer live in closure (non-reactive) for performance.
export function createRun(id, args, { throttleMs = 100 } = {}) {
    const run = reactive({
        id,
        args: [...args],
        status: 'queued',
        output: [],
        abortController: new AbortController(),
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

    async function start() {
        if (run.status !== 'queued') return;
        run.status = 'running';
        try {
            await main(toRaw(run.args), print, run.abortController.signal);
            if (run.status === 'running') run.status = 'completed';
        } catch (err) {
            if (run.status !== 'aborted') {
                run.status = 'error';
                print('Error:', err?.message ?? String(err));
            }
        } finally {
            flush();
        }
    }

    function abort() {
        if (run.status === 'running') {
            run.abortController.abort();
            run.status = 'aborted';
        }
    }

    function dispose() {
        if (throttleTimer) {
            clearTimeout(throttleTimer);
            throttleTimer = null;
        }
        buffer = [];
    }

    return Object.assign(run, {
        start,
        abort,
        dispose,
    });
}
