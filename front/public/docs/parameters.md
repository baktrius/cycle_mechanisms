[<< back to index](index.md)

# Solver Parameters

[[WORK IN PROGRESS - Below some AI generated descriptions that need to be verified and improved]]

The solver accepts various parameters to define the problem instance and the analysis task.

## Problem Instance

### Number of Vertices
*   **Description:** The size of the discrete cycle graph ($V$).
*   **Input:** Can be a single integer, a range (e.g., `4..10`), or a comma-separated list.
*   **Note:** The solver estimates the continuous cycle case as $V \to \infty$. Larger $V$ provides a better approximation of the continuous case but increases computation time.

### Number of Agents
*   **Description:** The number of agents ($N$) located on the cycle.
*   **Input:** Single integer, range, or list.
*   **Impact:** Complexity generally grows exponentially or polynomially with $N$ depending on the task.

### States of the World
Defines the set of possible agent profiles (configurations) to check.
*   **All:** Checks all possible combinations of agent locations on the discrete cycle.
*   **Not Reversed:** Restricts checking to "non-reversed" or "balanced" profiles (optimization).
*   **With Limited Distinct Votes:** Restricts profiles to those where agents report at most $k$ distinct locations. Useful for analyzing "boring" or low-entropy profiles.

## Tasks

### Check Strategyproofness (D)
*   **Goal:** Verify if the mechanism is Strategy-Proof (SP) in expectation.
*   **Output:** Returns `1` (True) if SP, `0` (False) otherwise.
*   **Method:** Checks if any agent can gain by misreporting their location.

### Calculate Approximation Ratio (A)
*   **Goal:** Find the worst-case approximation ratio of the mechanism.
*   **Formula:** $\max_{\text{profiles}} \frac{E[\text{Cost}(\text{Mechanism})]}{E[\text{Cost}(\text{OPT})]}$
*   **Output:** The maximum ratio found.

### Calculate RD Ratio (B)
*   **Goal:** Compare the mechanism against Random Dictator.
*   **Output:** A ratio indicating performance relative to RD.

### Calculate Complexity (C)
*   **Goal:** Estimate the size of the state space (number of sequences) to be processed.
*   **Output:** Number of profiles.

## Execution Settings

### Verbosity
*   **Inline Answer (V1):** Prints the final result (e.g., the ratio or boolean status).
*   **Summary (S):** Prints a summary including the worst-case profile found.
*   **All (V3):** Prints details for every processed profile (Warning: produces huge output).
*   **None (V0):** Silent.

### Calculations Limit
*   **Description:** Limits the number of profiles checked or operations performed.
*   **Usage:** Useful for quick checks or preventing browser freezes on large instances.

[<< back to index](index.md)