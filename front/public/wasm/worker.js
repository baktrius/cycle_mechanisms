importScripts("./main.js");

function print(...args) {
    postMessage({ print: args });
}

var Module = {
    print,
    printErr: print,
    setStatus: function (...msg) {
        print(...msg);
    },
    totalDependencies: 0,
    monitorRunDependencies(left) {
        this.totalDependencies = Math.max(this.totalDependencies, left);
        Module.setStatus(left ? 'Preparing... (' + (this.totalDependencies - left) + '/' + this.totalDependencies + ')' : 'All downloads complete.');
    },
    locateFile: (path) => `/wasm/${path}`, // Ensures .wasm resolves
};
self.onerror = (event) => {
    // TODO: do not warn on ok events like simulating an infinite loop or exitStatus
    Module.setStatus('Exception thrown, see JavaScript console');
};

onmessage = (e) => {
    const args = e.data.map(a => a.toString()) || [];

    console.log("Worker received args:", args);
    createModule(Object.assign({ arguments: args }, Module)).then(instance => {
        postMessage({ finished: true });
        self.close(); // terminate worker
    });
};
