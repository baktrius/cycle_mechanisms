export function main(args, print, abortSignal) {
    return new Promise((resolve, reject) => {
        const worker = new Worker("wasm/worker.js");

        // send args to main()
        worker.postMessage(args);
        worker.onmessage = (e) => {
            if (e.data.print !== undefined) {
                print(e.data.print);
            }
            if (e.data.finished !== undefined) {
                worker.terminate();
                resolve();
            }
        };

        worker.onerror = (err) => {
            worker.terminate();
            reject(err);
        };

        if (abortSignal) {
            abortSignal.addEventListener('abort', () => {
                worker.terminate();
                resolve();
            });
        }
    });
}
