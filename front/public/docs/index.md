# Cycle Mechanisms Solver

This tool allows you to analyze various facility location mechanisms operating on discrete cycle graphs. You can:
1.  Define a mechanism (or a mixture of mechanisms).
2.  Set the environment (number of agents, number of vertices).
3.  Run tasks like checking strategyproofness or calculating approximation ratios.

Use the **Left Panel** to configure your experiment and the **Right Panel** to view results and this documentation.

## Documentation Overview

[[WORK IN PROGRESS - Especially Mechanisms descriptions need to be improved]]

- **[Mathematical Background](math_background.md):** An introduction to the theoretical concepts behind facility location on cycles, including definitions of strategyproofness and approximation ratios.
- **[Mechanisms](mechanisms.md):** Details on the available facility location mechanisms (RD, PCD, Mix, etc.) and their mathematical properties.
- **[Parameters](parameters.md):** Explanation of the solver inputs, including graph size, agent count, tasks, and execution settings.

## Development
This project is implemented in C++ for performance, with a web-based GUI built using Vue.js and WebAssembly for accessibility.
Its source code is available on [GitHub](https://github.com/baktrius/cycle_mechanisms) under the MIT License.

## Acknowledgments

This project was developed to support the research presented in the paper [Strategy-Proof Facility Location on a Cycle](https://doi.org/10.24963/ijcai.2025/449) (IJCAI 2025) by Krzysztof Rogowski and Marcin Dziubiński