# Cycle Mechanism UI (Vue 3 + Vite)

This directory contains the frontend website used to configure and run an experimental numerical solver for the
strategy-proof (SP) anonymous facility location problem on the cycle.

The frontend is a small Vue 3 + Vite application that provides:

- a form to configure problem instances (mechanisms, parameters, sizes),
- a computed command-line argument preview that matches the solver's CLI,
- a run-manager UI that starts the solver, streams its output, and allows abort/close of individual runs or all runs.

This project is intended for researchers and developers experimenting with numerical solvers for facility
location on cyclic graphs. The frontend talks to a compiled solver (C++/WASM) and is mainly a convenience
UI for running and observing experiments.

For development (typical):

1. Install dependencies (if present in this workspace):

	npm install

2. Start the dev server (Vite):

	npm run dev

Adjust the commands if your workspace uses another package manager or custom scripts.

See repository root docs for more context about the solver and data used in experiments.
