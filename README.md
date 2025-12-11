# Cycle Mechanisms Solver

This repository contains a C++ solver and a web-based GUI for analyzing facility location mechanisms on a discrete cycle graph. It was developed to support the research presented in the paper [Strategy-Proof Facility Location on a Cycle](https://doi.org/10.24963/ijcai.2025/449) (IJCAI 2025).

## Web Demo

The **preferred and supported way** to use this repository is via the online graphical interface:
**[https://baktrius.github.io/cycle_mechanisms/](https://baktrius.github.io/cycle_mechanisms/)**

The web interface offers you to visually configure mechanisms, run simulations using WebAssembly, and analyze results directly in the browser.
Moreover it supports:
*   **Batch Processing:** Configure and execute runs for many different parameters at once.
*   **Result Aggregation:** View collective results and summaries for batch runs.
*   **Data Export:** Export run data to JSON for further custom analysis.

Comprehensive documentation for mechanisms and parameters is available directly within the application (Right Panel). You can also browse the source documentation files in [`front/public/docs/`](front/public/docs/index.md).

## Native C++ Solver

The core logic is implemented in `main.cpp` and `lib.h`. While the web interface is the primary tool, the native C++ solver is approximately **2x faster** and can be more convenient for scripting or integration into other pipelines.

### Prerequisites

*   **Compiler:** A modern C++ compiler supporting **C++23** (e.g., GCC 13+, Clang 16+).
*   **Build System:** GNU Make.
*   **Web Build (Optional):** Emscripten (for compiling to WebAssembly).

### Building

To build the native solver:

```bash
make
```

This will produce the `main` executable.

To build the WebAssembly modules (requires Emscripten):

```bash
make wasm
```

### Usage

The solver accepts a sequence of flags and positional arguments. **Note:** The argument parser is strict and expects arguments in a specific order.

We do not document the specific arguments here. Instead, **use the Web Demo to generate them**. Configure your mechanism and settings in the Left Panel of the web interface, and copy the generated string from the **"Resulting Args"** field. You can then pass this string directly to the `main` executable.

## Project Structure

*   `main.cpp`: Entry point for the CLI solver. Handles argument parsing and simulation setup.
*   `lib.h`: Core library containing mechanism definitions, graph logic, and solver algorithms.
*   `front/`: Vue.js frontend application.
*   `Makefile`: Build configuration for native and WASM targets.

## Citation

If you find this project useful in your research, please cite the following paper:

```bibtex
@inproceedings{rogowski2025,
  title = {Improved Approximation Ratio for Strategyproof Facility Location on a Cycle},
  author = {Rogowski, Krzysztof and Dziubiński, Marcin},
  booktitle = {Proceedings of the Thirty-Fourth International Joint Conference on
                 Artificial Intelligence, {IJCAI-25}},
  publisher = {International Joint Conferences on Artificial Intelligence Organization},
  editor = {Kwok, James},
  pages = {4032--4039},
  year = {2025},
  month = aug,
  note = {Main Track},
  doi = {10.24963/ijcai.2025/449},
  url = {https://doi.org/10.24963/ijcai.2025/449},
}
```