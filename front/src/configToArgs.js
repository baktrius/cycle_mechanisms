// Convert a mechanism definition object into a flat list of CLI tokens.
export function mechanismToArgs(mech) {
    const res = [];
    if (!mech || !mech.type) return res;

    if (mech.type === "mix") {
        // mix: right-mechanism, M<ratio>, left-mechanism
        res.push(...mechanismToArgs(mech.right));
        res.push(`M${mech.ratio}`);
        res.push(...mechanismToArgs(mech.left));
        return res;
    }

    res.push(mech.type);
    if (mech.type === "qcd") {
        res.push(mech.threshold ?? 0);
    } else if (mech.type === "dbl") {
        res.push(mech.exponent ?? 1);
    }
    return res;
}

// Convert the "states of the world" selection into CLI tokens.
export function statesTypeToArgs(config) {
    const { statesType, numDistinctVotes } = config;
    if (statesType === "I") {
        return ["I"];
    } else if (statesType === "J") {
        return [`J${numDistinctVotes}`];
    }
    return [];
}

// Main conversion function: from config object to args array.
// Expected config shape (subset):
// {
//   numAgents: number | string,
//   numVertices: number | string,
//   task: string,
//   verbosity: string,
//   statesType: "all" | "I" | "J",
//   numDistinctVotes: number,
//   calculationsLimit: number,
//   mechanism: { ... }
// }
export function configToArgs(config) {
    const {
        numAgents,
        numVertices,
        task,
        verbosity,
        calculationsLimit,
        mechanism,
    } = config;

    return [
        `N${numAgents}`,
        task,
        verbosity,
        ...statesTypeToArgs(config),
        numVertices,
        ...mechanismToArgs(mechanism),
        ...(Number(calculationsLimit) > 0 ? ['E2', `L${calculationsLimit}`] : []),
    ];
}
