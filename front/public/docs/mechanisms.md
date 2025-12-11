[<< back to index](index.md)

# Mechanisms

[[WORK IN PROGRESS - Below some AI generated descriptions that need to be verified and improved]]

Internally solver implements several abstract classes of mechanisms operating on discrete cycle graphs.
Then mechanisms available to the user are implemented as parametrized instances of these classes.
We start by fixing notation and briefly
describing those abstract classes, and then we list available mechanisms:

## Notation
Let $G_V$ be a discrete cycle graph with vertex set $V = \{0, 1, \ldots, V-1\}$.
Let $N = \{0, 1, \ldots, n - 1\}$ be the set of agents.
As all mechanisms we consider are anonymous (independedent of agent identities), we assume agents are ordered in increasing order of their reported ideal points on the graph.
Therefore a profile is ordered tuple $b=(b_i)_{i \in N}$, where $b_i \in V$ is the ideal point of agent $i$.

We are interested in arcs' lengths between votes of consecutive agents.
Let $a_i$ be the length of the arc from $b_i$ to $b_{(i+1) \mod n}$ that is for $i \in N$:

 $$a_i = \frac{1}{V}\begin{cases} (b_{(i+1)} - b_i) & \text{if } i < n - 1 \\ (b_0 + V - b_{n-1}) & \text{if } i = n - 1 \end{cases}$$

For convenience for any index $i\in \mathbb{Z}$, we define $a_i = a_{i \mod n}$.

## Abstract Mechanism Classes

### Distance Based Mechanisms
- **Params:** 

### Gap Based Mechanisms

### Opposition Based Mechanisms

## Available Mechanisms

This solver implements several facility location mechanisms on a cycle:

### Random Dictator (RD)
*   **Type Code:** `rd`
*   **Description:** Selects one of the agents uniformly at random and places the facility at their reported location.
*   **Properties:** Strategyproof (SP), and its approximation ratio is $2 - \frac{2}{N}$ for any graph with at least 2 distinct locations.

### Proportional Circle Distance (PCD)
*   **Type Code:** `pcd`
*   **Description:** A mechanism proposed by Meir (2019). It assigns probabilities to locations based on their distance properties relative to the agents.
*   **Details:** It is designed to be strategyproof and offer better approximation guarantees than RD in certain contexts.

### Optimal (OPT)
*   **Type Code:** `opt`
*   **Description:** Calculates the ex-post optimal facility location that minimizes the sum of costs (distances) for the given agent profile.
*   **Note:** This is generally **not** strategyproof. It serves as a baseline for calculating the approximation ratio (Cost(Mechanism) / Cost(OPT)).

### Distance Based Lottery (DBL)
*   **Type Code:** `dbl`
*   **Parameters:** `exponent` (default: 1)
*   **Description:** Assigns probabilities based on a power function of the distance.
*   **Formula:** The rank/weight of a location $x$ is related to $(x^e + (1-x)^e)^{1/e}$.

### Squared Circle Distance (SQCD)
*   **Type Code:** `sqcd`
*   **Description:** A variation of distance-based mechanisms using a specific circle rank function.

### Quadratic Circle Distance (QCD)
*   **Type Code:** `qcd`
*   **Parameters:** `threshold` (or bound)
*   **Description:** Uses a quadratic function for weights, with a minimum threshold.
*   **Formula:** Weights are calculated as $\max(r^2, \text{bound}^2)$.

### Mixed Mechanism (Mix)
*   **Type Code:** `mix`
*   **Parameters:** `ratio` (0 to 1), `left` (Mechanism), `right` (Mechanism)
*   **Description:** A convex combination of two other mechanisms.
*   **Formula:** $P(x) = \text{ratio} \cdot P_{\text{left}}(x) + (1 - \text{ratio}) \cdot P_{\text{right}}(x)$.
*   **Usage:** Useful for finding trade-offs between different properties (e.g., mixing RD and PCD to improve approximation ratio while maintaining strategyproofness).

### Randomized Lottery
*   **Type Code:** `R` (flag) or `randomized` wrapper
*   **Description:** A meta-mechanism that averages results over permutations or specific randomizations of the input profile.

[<< back to index](index.md)