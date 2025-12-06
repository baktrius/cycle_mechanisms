#include <iostream>
#include <iomanip>
#include <string>
#include <functional>
#include <memory>
#include "lib.h"

using std::cout;
using std::identity;
using std::make_unique, std::unique_ptr;
using std::max;
using std::setprecision;
using std::stoul, std::stod, std::stoi;
using std::string;
using std::move;

[[noreturn]] void fail(string reason) {
    cout << reason << "\n";
    exit(1);
}

int main(int argc, const char **argv) {
    auto consume = [&argv](const char *arg) {
        if (!*argv) fail(string{"expected parameter: "} + arg);
        return *argv++;
    };
    auto flag = [&argv](const char *flag, char symbol, const char *def = nullptr)
        EXPR((*argv && **argv == symbol) ? (*argv++)+1 : def);
    consume("program name");
    size_t agentsNum = stoul(flag("num of agents", 'N', "3"));
    bool rdFlag = flag("rd ratio", 'B');
    bool scFlag = flag("approximation ratio", 'A');
    bool complexityFlag = flag("complexity only", 'C');
    flag("check", 'D');
    bool avgFlag = flag("calculate average", 'E');
    bool pcdBoundFlag = flag("pcd bound", 'F');
    bool numOfPointsFlag = flag("num of points", 'P');
    Verbosity verbosity = flag("summary only", 'S') ? Verbosity::summary : Verbosity::all;
    if (const char *val = flag("verbosity", 'V')) verbosity = (Verbosity) stoul(val);
    bool reversedLot = flag("reverse lottery", 'R');
    bool reverseOptimization = flag("reverse optimization", 'I');
    size_t boringOptimization = stoul(flag("boring optimization", 'J', "0"));
    bool stdinGenerator = flag("stdin generator", 'G');
    size_t graphSize = stoul(consume("size of graph"));
    const Graph &graph = Circle(graphSize);
    auto parseLottery = [&]() -> lottery {
        string method = consume("method");
        if (method == "rd") return rdLottery;
        else if (method == "pcd") return distantBasedLottery(graphSize, uniformRank);
        else if (method == "pcd2") return oppositionBasedLottery<false>(graphSize, identity());
        else if (method == "pcd3") {
            l<real> weight(agentsNum, .0);
            weight[(agentsNum - 1) / 2] = 1;
            return gapBasedLottery(graphSize, weight);
        }
        else if (method == "r3pcd") {
            l<real> weight(agentsNum, .0);
            for(size_t i = 0; i < agentsNum; i++) weight[i] = (1+2*i)*agentsNum-2/3.-2*i*(i+1);
            return gapBasedLottery(graphSize, weight);
        }
        else if (method == "dbl") return distantBasedLottery(graphSize, powerRank(stod(consume("exponent"))));
        else if (method == "sqcd") return distantBasedLottery(graphSize, circleRank);
        else if (method == "qcd") {
            real bound = stod(consume("exponent"));
            return oppositionBasedLottery(graphSize, [bound](real r) EXPR(max(r * r, bound * bound)));
        }
        else if (method == "custom0") return customLottery(graphSize, consume("path"), 0);
        else if (method == "custom1") return customLottery(graphSize, consume("path"), 1);
        else if (method == "opt") return optLottery(graph);
        fail("unrecognised method: " + method);
    };
    lottery lot = parseLottery();

    while(const char *val = flag("mix lottery", 'M')) lot = mixedLottery(graphSize, stod(val), parseLottery(), lot);

    while(const char *val = flag("randomized lottery", 'R')) {
        size_t t = stoul(val);
        if (t == 0) lot = randomizedLottery(lot);
        else if (t == 1) lot = randomizedLottery2(lot);
        else fail("unrecognised randomization type: " + string(val));
    }

    if (reversedLot) lot = reversedLottery(graphSize, lot);

    unique_ptr<seqs> generator;
    if (stdinGenerator) generator = make_unique<stdin_seqs>(0, graphSize, agentsNum);
    else if (boringOptimization) generator = make_unique<increasing_boring_asymmetric_seqs<>>(0, graphSize, agentsNum, boringOptimization);
    else if (reverseOptimization) generator = make_unique<increasing_asymmetric_seqs>(0, graphSize, agentsNum);
    else generator = make_unique<increasing_seqs>(0, graphSize, agentsNum);

    while (const char *val = flag("filter", 'F'))
    {
        if (val[0] == '1') generator = make_unique<FilterUnbalanced>(std::move(generator), graph);
        else if (val[0] == '2') generator = make_unique<FilterDominant>(std::move(generator), graph);
        else fail("unrecognised filter: " + string(val));
    }

    if(const char *val = flag("strategyproofisation", 'P')) {
        size_t gen_type = val[0] != 0 ? stoul(val) : 0;
        unique_ptr<seqs> gen;
        if (gen_type == 1) gen = make_unique<increasing_boring_asymmetric_seqs<>>(0, graphSize, agentsNum, boringOptimization);
        else if (gen_type == 0) gen = make_unique<increasing_asymmetric_seqs>(0, graphSize, agentsNum);
        else gen = make_unique<increasing_seqs>(0, graphSize, agentsNum);
        lot = mixedLottery(graphSize, rdRatio(lot, *gen, Circle(graphSize), Verbosity::none), rdLottery, lot);
    }

    int exitCodeOnLimit = stoi(flag("exit code on limit", 'E', "0"));

    if (const char *val = flag("limit", 'L')) {
        double limit = stod(val);
        double estimatedSize = generator->approxSize();
        if (limit > 0 && estimatedSize > limit) {
            if (verbosity == Verbosity::answer) cout << "SEQS: " << setprecision(2) << scientific<< estimatedSize;
            else if (verbosity >= Verbosity::summary) {
                cout << "Estimated number of sequences: " << setprecision(2) << scientific << estimatedSize << '\n';
            }
            return exitCodeOnLimit;
        }
    }

    // check that there are no arguments left
    if (*argv) fail("unconsumed arguments left");

    if(rdFlag) rdRatio(lot, *generator, graph, verbosity);
    else if (pcdBoundFlag) score(SumQ(make_unique<PcdBound>(), make_unique<ApproxRatio>(lot)), *generator, graph, verbosity, avgFlag);
    else if (scFlag || avgFlag || numOfPointsFlag)
        score(ApproxRatio(lot), *generator, graph, verbosity, avgFlag, numOfPointsFlag);
    else if(complexityFlag && verbosity >= Verbosity::answer) {
        cout << setprecision(2) << real(generator->approxSize());
        if (verbosity >= Verbosity::summary) cout << '\n';
    }
    else check(lot, *generator, graph, verbosity);
    std::cout.flush();
    std::cerr.flush();
    return 0;
}
