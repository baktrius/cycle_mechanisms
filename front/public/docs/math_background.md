[<< back to index](index.md)

# Mathematical Background

A facility location problem consists of a set of agents $N = \{0, 1, \ldots, n-1\}$ and a domain
(a metric space). Typically, the domain is considered to be a graph
equipped with a metric. We restrict attention to a
weighted discrete cycle graph of length $1$.

Let $G_V$ be a discrete cycle graph with vertex set $V = \{0, 1, \ldots, V-1\}$
and edges connecting each vertex $i$ to $(i+1) \mod V$ and $(i-1) \mod V$.
The distance between any two vertices $v_1, v_2 \in V$ on the graph is given by
 $d(v_1, v_2) = \min(|v_2 - v_1|, V - |v_2 - v_1|) / V$.

**Profile** $b=(b_i)_{i \in N}$ is a $n$ tuple of vertices, which represents respective agents' ideal points on the graph i.e., $b_i \in V$ is the ideal point of agent $i$.
it determines the agent $i$ costs over the points of the graph. Agent's $i$ **cost** of choosing point $v \in V$ is defined as the distance between their ideal point $b_i$ and $v$.

For any set $A$, a **lottery** over $A$ is defined as a discrete
probability distribution on $A$. The set of all lotteries over $A$ is
denoted by $\Delta(A)$.
Agents' costs are extended to lotteries over the points of the
graph by using the expected values. The **cost** for an agent with ideal
point $v$ from lottery $l \in \Delta(G)$ is
 $$c_v(l) = \mathbb{E}_{X \sim l}[d(v, X)].$$

In this work, the quality of any lottery $l \in \Delta(G)$ is evaluated
using the **social cost**, defined as the sum of costs of all agents.
Formally, for any profile $b$, let
 $$\text{sc}_b(l) = \sum_{i \in N} c_{b_i}(l).$$

For simplicity, we sometimes abuse the notation and compute the social
cost for a single point $v \in G$, treating it as a degenerate lottery
where the given point is always selected. The minimal social cost is
referred to as the **optimal cost** and denoted by $opt_b$. Formally,
 $$\text{opt}_b = \inf_{v \in G} \text{sc}_b(v).$$ It is straightforward
to see that, due to the definition of agent costs via expected values,
the optimal cost is always achieved by some point.

Let $B$ be an arbitrary set of profiles. A function $M: B \to \Delta(G)$, which maps profiles to
lotteries over the points of the graph, is called a **mechanism**.

Given a mechanism, agents report their ideal points, not necessarily
truthfully. Based on these reports, the mechanism determines a lottery,
which is then used to select a point of the graph.

## Common Mechanism Properties

Let $b$ denote an arbitrary profile. Mechanism $M$ is said to be:

-   **Strategyproof (SP)** if, no agent $i \in N$, can benefit from misreporting their ideal location.  Formally:
    $$c_{b_i}(M(b)) \leq c_{b_i}(M(b[i \to v])),$$ where $b[i \to v]$
    denotes the profile in which agent $i$ reports point $v$ instead of
    their true ideal location $b_i$.

-   **Anonymous**, if it does not distinguish between agents, that is for
    any permutation of agents $\pi: N \to N$, the outcome of the
    mechanism $M$ remains unchanged. Formally, $M(b) = M(b \circ \pi)$.

-   **Neutral**, if it does not distinguish between similar graph's
    locations. Formally, for any automorphism $f$ of the graph $G$, the
    outcome of the mechanism $M$ is transformed according to $f$ i.e.,
     $M(f \circ b)\circ f = M(b)$.

-   **Peaks-only**, if the support of the resulting lottery $M(b)$ is
    contained within the points reported by the agents, i.e., in the
    image of $b$.

-   **$\alpha$ approximation** if the social cost of the mechanism's
    outcome is no greater than $\alpha$ times the optimal cost.
    Formally, $\text{sc}_b(M(b)) \leq \alpha \cdot opt_b$.

The **approximation ratio** of a mechanism $M$ for a set of profiles $B$
is defined as the smallest $\alpha$ such that $M$ is an
 $\alpha$ approximation for all profiles from $B$. We denote this value
by $\text{apx}_M(B)$.

[<< back to index](index.md)
