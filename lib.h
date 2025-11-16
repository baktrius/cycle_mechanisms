#include <iostream>
#include <iomanip>
#include <functional>
#include <vector>
#include <cmath>
#include <numeric>
#include <list>
#include <ranges>
#include <limits>
#include <algorithm>
#include <iterator>
#include <string>
#include <memory>
// #include <generator>
#include "npy.hpp"
#include "generator.hpp"

#define EXPR(b) { return b; }

using std::setprecision, std::scientific;
using std::size_t;
using std::vector;
using std::min, std::max;
using std::multiplies;
using std::cout, std::cerr;
using std::function;
using std::plus;
using std::pow, std::round;
using std::list;
namespace rn = std::ranges;
using std::views::transform, std::views::drop, std::views::join, std::views::repeat;
using std::views::take, std::views::zip;
using std::views::reverse;
using std::numeric_limits;
using std::get;
using std::back_inserter;
using std::prev;
using std::string;
using std::unique_ptr;
using std::move;

using real=double;
template<typename T>
using l=vector<T>;
using lottery=function<l<real>(const l<size_t>&)>;
constexpr real EPS = 1e-6;

bool nzero(real x) EXPR(x > EPS || x < -EPS)

real circleDistance(real a, real b) {
    return min(b - a, 1 + a - b);
}

real circleRank(real x) {
    return circleDistance(0, x);
}

auto powerRank(real e) {
    return [e](real x) EXPR(pow((pow(x, e) + pow(1 - x, e)) / 2, 1/e));
}

real circleDistance2(real a, real b) {
    return a < b ? circleDistance(a, b) : circleDistance(b, a);
}

real r(real x, size_t p = 4) {
    real multiplier = pow(10, p);
    real t =  std::round(x * multiplier) / multiplier;
    return t == 0 ? 0 : t;
}

size_t binomialCoefficient(size_t n, size_t k) {
    size_t res = 1;
    for (size_t l = 0; l < min(k, n - k); ++l) res = (res * (n-l)) / (l+1);
    return res;
}

size_t numOfIncreasingSeqs(size_t len, size_t bound) {
    return binomialCoefficient(len + bound - 1, len);
}

template<rn::input_range R>
constexpr auto sum( R&& r) EXPR(rn::fold_left(r, 0, plus<std::ranges::range_value_t<R>>()));

template<rn::input_range R>
constexpr auto minimum(R&& r) {
    using T = rn::range_value_t<R>;
    auto it = rn::begin(r);
    auto end = rn::end(r);
    if (it == end) return numeric_limits<T>::infinity();
    T min_val = *it;
    ++it;
    for (; it != end; ++it) {
        if (*it < min_val) min_val = *it;
    }
    return min_val;
}

template<rn::input_range R>
constexpr auto maximum(R&& r) {
    using T = rn::range_value_t<R>;
    auto it = rn::begin(r);
    auto end = rn::end(r);
    if (it == end) return -numeric_limits<T>::infinity();
    T max_val = *it;
    ++it;
    for (; it != end; ++it) {
        if (*it > max_val) max_val = *it;
    }
    return max_val;
}

template<rn::input_range R>
void printR(R &&r, const char *sep="\t") {
    for (auto a : r) cout << a << sep;
}

template<typename... args>
void printM(const char *sep = "\t", args... Args) {
    ((cout << Args << sep), ...);
}

template<typename F>
lottery distantBasedLottery(size_t size, const F &ranks) {
    vector<real> weights = vector<real>(size+1);

    // precalculating weight
    real prefixSum = 0;
    for (size_t i = 0; i < size; ++i) {
        weights[i+1] = prefixSum += ranks((i + 0.5f) / size);
    }
    auto rankOfRange = [weights](size_t b, size_t a = 0) EXPR(weights[b] - weights[a]);
    return [rankOfRange, size, prefixSum](const l<size_t> &as) {
        const size_t agentsNum = as.size();
        const size_t dis = agentsNum / 2;
        l<real> res{};
        res.reserve(agentsNum);
        for (size_t i = 0; i < agentsNum; ++i) {
            real probability = 0;
            const size_t scoredRangeStart = (i + dis) % agentsNum;
            const size_t scoredRangeEnd = (scoredRangeStart + 1) % agentsNum;
            for (size_t j = 0; j < agentsNum; ++j) {
                if (scoredRangeStart < j)
                    probability += rankOfRange(as[j] - as[scoredRangeStart], as[j] - as[scoredRangeEnd]);
                else if (scoredRangeEnd <= j)
                    probability += rankOfRange(size + as[j] - as[scoredRangeStart], as[j] - as[scoredRangeEnd]);
                else
                    probability += rankOfRange(as[scoredRangeEnd] - as[j], as[scoredRangeStart] - as[j]);
            }
            res.push_back(probability / prefixSum / agentsNum);
        }
        return res;
    };
}

template<rn::input_range R>
vector<rn::range_value_t<R>> toVec(R &&r) {
    vector<rn::range_value_t<R>> res;
    if constexpr(rn::sized_range<decltype(r)>) res.reserve(rn::size(r));
    rn::copy(r, back_inserter(res));
    return res;
}

template<bool normalize = true>
lottery gapBasedLottery(size_t size, l<real> weights) {
    return [size, weights](const l<size_t> &as) {
        const size_t agentsNum = as.size();
        l<real> agent_pos(agentsNum * 2);
        for (size_t i = 0; i < agentsNum; ++i) {
            agent_pos[i] = real(as[i]) / size;
            agent_pos[i + agentsNum] = real(as[i]) / size + 1;
        }
        l<real> res(agentsNum);
        for (size_t i = 0; i < agentsNum; ++i) {
            real s = 0;
            for (size_t j = 0; j < weights.size(); ++j) {
            real sum = 0;
            for (size_t k = i; k < i + agentsNum; ++k) {
                size_t idx1 = k % agentsNum;
                size_t idx2 = (k + 1) % agentsNum;
                sum += weights[j] * (agent_pos[idx2] - agent_pos[idx1]);
            }
            s += sum;
            }
            res[i] = s;
        }
        if (normalize) {
            real s = sum(res);
            for (real &el : res) el /= s;
        }
        return res;
    };
}

template<bool normalize = true>
lottery oppositionBasedLottery(size_t size, function<real(real)> weights) {
    return [size, weights](const l<size_t> &as) {
        const size_t agentsNum = as.size();
        const size_t dis = agentsNum / 2;
        l<real> res(agentsNum);
        for (size_t i = 0; i < agentsNum; ++i) {
            size_t idx1 = (i + dis) % agentsNum;
            size_t idx2 = (i + dis + 1) % agentsNum;
            real diff = (real(as[idx2]) - as[idx1]) / size;
            if (i + dis == agentsNum - 1)
            diff += 1;
            res[i] = weights(diff);
        }
        if (normalize) {
            real s = sum(res);
            for (real &el : res) el /= s;
        }
        return res;
    };
}

auto customLottery(size_t size, const string &path, size_t opt) {
    std::vector<unsigned long> shape {};
    std::vector<real> data;

    npy::LoadArrayFromNumpy(path, shape, data);
    for (auto dim : shape | drop(1) | reverse | drop(1)) {
        if (dim != size) exit(2);
    }
    return [size, shape, data, opt](const l<size_t> &as) {
        const size_t agentsNum = as.size();
        if (agentsNum + 1 != shape.size()) exit(3);
        size_t start = 0;
        if (opt >= 1) {
            for (auto el : as) start = start * size + el - as[0];
        } else {
            for (auto el : as) start = start * size + el;
        }
        start *= agentsNum;
        return toVec(data | drop(start) | take(agentsNum));
    };
}

auto rdLottery(l<size_t> x) EXPR(l<real>(x.size(), 1. / x.size()))

lottery reversedLottery(size_t size, const lottery &lot) {
    return [size, lot](const l<size_t> &as) {
        return toVec(lot(toVec(as | reverse | transform([size](size_t x)EXPR(size - x)))) | reverse);
    };
}

lottery mixedLottery(size_t size, real a, const lottery &lot1, const lottery &lot2) {
    return [=](const l<size_t> &as) {
        auto res = lot1(as);
        auto tmp = lot2(as);
        for (size_t i = 0; i < res.size(); ++i) {
            res[i] = res[i] * a + tmp[i] * (1 - a);
        }
        return res;
    };
}

const real multipliers[] = {6, 3, 1};
lottery randomizedLottery(const lottery &lot) {
    return [=](const l<size_t> &as) {
        l<size_t> as2;
        l<real> res(as.size(), 0);
        size_t agents_num = as.size();
        size_t div = agents_num * agents_num * agents_num;
        for (size_t i0 = 0; i0 < agents_num; ++i0)
        {
            as2.push_back(as[i0]);
            for (size_t i1 = i0; i1 < agents_num; ++i1)
            {
                as2.push_back(as[i1]);
                for (size_t i2 = i1; i2 < agents_num; ++i2)
                {
                    as2.push_back(as[i2]);
                    size_t eq_num = (i0 == i1) + (i1 == i2);
                    real mul = multipliers[eq_num];
                    l<real> innerRes = lot(as2);
                    res[i0] += mul * innerRes[0];
                    res[i1] += mul * innerRes[1];
                    res[i2] += mul * innerRes[2];
                    as2.pop_back();
                }
                as2.pop_back();
            }
            as2.pop_back();
        }
        for (auto &el : res) el /= div;
        return res;
    };
}

lottery randomizedLottery2(const lottery &lot) {
    return [=](const l<size_t> &as) {
        l<size_t> as2;
        l<real> res(as.size(), 0);
        size_t agents_num = as.size();
        size_t div = agents_num * (agents_num - 1) * (agents_num - 2) / 6;
        for (size_t i0 = 0; i0 < agents_num; ++i0)
        {
            as2.push_back(as[i0]);
            for (size_t i1 = i0 + 1; i1 < agents_num; ++i1)
            {
                as2.push_back(as[i1]);
                for (size_t i2 = i1 + 1; i2 < agents_num; ++i2)
                {
                    as2.push_back(as[i2]);
                    l<real> innerRes = lot(as2);
                    res[i0] += innerRes[0];
                    res[i1] += innerRes[1];
                    res[i2] += innerRes[2];
                    as2.pop_back();
                }
                as2.pop_back();
            }
            as2.pop_back();
        }
        for (auto &el : res) el /= div;
        return res;
    };
}

class Graph {
    public:
    Graph(size_t size) : size(size) {}
    const size_t size;
    virtual real distance(size_t a, size_t b) const = 0;
};

class SplitCircle : public Graph {
    public:
    const size_t splitVertex;
    SplitCircle(size_t size, size_t splitVertex) : Graph(size), splitVertex(splitVertex) {}
    real distance(size_t a, size_t b) const override {
        return abs(int((b + size -  splitVertex) % size) - int((a + size - splitVertex) % size)) / real(size);
    }
};

class Circle : public Graph {
    public:
    Circle(size_t size) : Graph(size) {}
    real distance(size_t a, size_t b) const override {
        size_t diff = a < b ? b - a : a - b;
        return min(diff, size - diff) / real(size);
    }
    SplitCircle split(size_t splitVertex) const {
        return SplitCircle(size, splitVertex);
    }
};

class CustomCircle : public Graph {
    private:
    l<real> vertices;
    public:
    CustomCircle(l<real> vertices) : Graph(vertices.size()), vertices(vertices) {}
    real distance(size_t a, size_t b) const override {
        real diff = vertices[a] - vertices[b];
        real diff2 = diff - floor(diff);
        return min(diff2, 1 - diff2);
    }
};

template<typename T>
class gen {
public:
    using value_type = T;
private:
    T currentEl;
public:
    gen(T &&emptyEl) : currentEl(emptyEl) {}
    virtual const T &get() const EXPR(currentEl)
    virtual T &get() EXPR(currentEl)
    virtual bool next() = 0;
    cppcoro::generator<const T&> toGen() {
        while (next()) co_yield get();
        co_return;
    }

};

using std::reference_wrapper;
using conf=std::pair<reference_wrapper<const Graph>, reference_wrapper<const l<size_t>>>;

class seqs : public gen<l<size_t>> {
protected:
    size_t start;
    size_t end;
    size_t size;
public:
    seqs(size_t start, size_t end, size_t size)
    : gen<l<size_t>>(l<size_t>{start-1ul})
    , start(start), end(end), size(size) {}
    virtual size_t approxSize() const = 0;
};

template<typename T, class... Args>
cppcoro::generator<const typename T::value_type&> makeGen(Args... args) {
    T g(std::forward<Args>(args)...);
    while (g.next()) co_yield g.get();
    co_return;
}

template<typename T, class... Args>
cppcoro::generator<conf> genConfs(const Graph &graph, Args... args) {
    T g(0, graph.size, std::forward<Args>(args)...);
    while (g.next()) co_yield std::make_pair(std::ref(graph), std::ref(g.get()));
    co_return;
}

class stdin_seqs : public seqs
{
public:
    stdin_seqs(size_t start, size_t end, size_t size) : seqs(start, end, size) {}
    bool next()
    {
        std::string line;
        if (std::getline(std::cin, line))
        {
            if (line.starts_with("print "))
            {
                std::cout << line.substr(6);
                return next();
            }
            else if (line.starts_with("println "))
            {
                std::cout << line.substr(8) << '\n';
                return next();
            }
            get().clear();
            size_t pos = 0, nextDelim;
            do
            {
                nextDelim = line.find(' ', pos);
                if (nextDelim > pos)
                {
                    try
                    {
                        get().push_back(std::stoi(line.substr(pos, nextDelim)));
                    }
                    catch (const std::invalid_argument &e)
                    {
                        std::cout << "Error: " << e.what() << " on arg: " << line.substr(pos, nextDelim) << '\n';
                        return next();
                    }
                }
                pos = nextDelim + 1;
            } while (nextDelim != std::string::npos);
            std::sort(get().begin(), get().end());
            size_t min_val = get().front();
            for (size_t &el : get())
                el -= min_val;
            return true;
        }
        return false;
    }
    size_t approxSize() const EXPR(0);
};

class increasing_seqs : public seqs
{
public:
    increasing_seqs(size_t start, size_t end, size_t size) : seqs(start, end, size) {}
    bool next() {
        while (get().back() == end - 1) {
            get().pop_back();
            if (get().size() == 1) return false;
        }
        ++get().back();
        while(get().size() < size) get().push_back(get().back());
        return true;
    }
    size_t approxSize() const EXPR(numOfIncreasingSeqs(size, end - start));
};

class increasing_asymmetric_seqs : public seqs {
public:
    increasing_asymmetric_seqs(size_t start, size_t end, size_t size) : seqs(start, end, size) {}
    bool next() {
        for(;;) {
            while (get().back() == end - 1) {
                get().pop_back();
                if (get().size() == 1) return false;
            }
            ++get().back();
            while(get().size() < size) get().push_back(get().back());
            size_t numOfZeros = 0;
            for (auto i = get().begin(); *i == 0; ++i) ++numOfZeros;
            auto invertedSeq = repeat(get() | reverse | transform([&](size_t x)EXPR((end - x) % end))) | join | drop(size - numOfZeros);
            if (!rn::lexicographical_compare(invertedSeq, get())) return true;
            else get().pop_back();
        }
    }
    size_t approxSize() const EXPR(numOfIncreasingSeqs(size, end - start) / 2);
};

template<bool asymmetric = true>
class increasing_boring_asymmetric_seqs : public seqs {
private:
    size_t bound;
    size_t numOfDiffValues = 1;
    size_t pop() {
        size_t lastEl = get().back();
        if (lastEl != *prev(get().end(), 2)) --numOfDiffValues;
        get().pop_back();
        return lastEl;
    }
    bool push(size_t el) {
        if (get().back() != el) {
            if (numOfDiffValues >= bound) return false;
            ++numOfDiffValues;
        }
        get().push_back(el);
        return true;
    }
public:
    increasing_boring_asymmetric_seqs(size_t start, size_t end, size_t size, size_t bound) : seqs(start, end, size), bound(bound) {}
    bool next() {
        for(;;) {
            while (get().back() == end - 1) {
                pop();
                if (get().size() == 1) return false;
            }
            if(!push(pop()+1)) continue;
            while(get().size() < size) get().push_back(get().back());
            if (!asymmetric) return true;
            size_t numOfZeros = 0;
            for (auto i = get().begin(); *i == 0; ++i) ++numOfZeros;
            auto invertedSeq = repeat(get() | reverse | transform([&](size_t x)EXPR((end - x) % end))) | join | drop(size - numOfZeros);
            if (!rn::lexicographical_compare(invertedSeq, get())) return true;
            else pop();
        }
    }
    size_t approxSize() const {
        size_t res = 0;
        for (size_t i = 1; i <= bound; ++i) {
            res += binomialCoefficient(end - start, i) * numOfIncreasingSeqs(size - i, i) / 2;
        }
        return res; // slightly overestimated
    }
};

l<real> oppositeDistances(const Graph &g, const l<size_t> &seq) {
    l<real> res;
    size_t n = seq.size();
    res.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        size_t j = (i + n / 2) % n;
        res.push_back(g.distance(seq[i], seq[j]));
    }
    return res;
}

real pcdBoundValue(const Graph &g, const l<size_t> &seq) {
    auto dis_opp = oppositeDistances(g, seq);
    real optimalCost = sum(dis_opp);
    real realCost = 2* sum(dis_opp | transform([&](real x) EXPR(x * (1 - x))));
    return nzero(optimalCost) ? realCost / optimalCost : (nzero(realCost) ? numeric_limits<real>::infinity() : 1);
}

real getVertexCost(const Graph &g, const l<size_t> &seq, size_t vertex) {
    return sum(seq | transform([&](size_t x) EXPR(g.distance(vertex, x))));
}

real getLotteryCost(const lottery &lot, const Graph &g, const l<size_t> &seq) {
    real realCost = 0;
    for (const auto &[a, p] : zip(seq, lot(seq))) {
        real c = getVertexCost(g, seq, a);
        realCost += p * c;
    }
    return realCost;
}

real getOptCost(const Graph &g, const l<size_t> &seq) {
    return minimum(seq | transform([&](size_t x) EXPR(getVertexCost(g, seq, x))));
}

template<bool normalize = true>
auto optLottery(const Graph &g) {
    return [&](const l<size_t> &as) {
        real minCost = getOptCost(g, as);
        l<real> res = toVec(as | transform([&](size_t x)EXPR(getVertexCost(g, as, x) == minCost ? 1.0 : 0.0)));
        if (normalize) {
            real s = sum(res);
            for (real &el : res) el /= s;
        }
        return res;
    };
}

real approximationRatio(const lottery &lot, const Graph &g, const l<size_t> &seq) {
    real optimalCost = std::numeric_limits<real>::infinity();
    real realCost = 0;
    for (const auto &[a, p] : zip(seq, lot(seq))) {
        real c = getVertexCost(g, seq, a);
        realCost += p * c;
        optimalCost = min(optimalCost, c);
    }
    return nzero(optimalCost) ? realCost / optimalCost : (nzero(realCost) ? numeric_limits<real>::infinity() : 1);
}

class Quantity {
    public:
    virtual real score(const l<size_t> &seq, const Graph &g) const = 0;
    real operator()(const l<size_t> &seq, const Graph &g) const EXPR(score(seq, g));
};

class ApproxRatio : public Quantity {
    private:
    const lottery &lot;
    public:
    ApproxRatio(const lottery &lot) : lot(lot) {}
    real score(const l<size_t> &seq, const Graph &g) const override {
        return approximationRatio(lot, g, seq);
    }
};

class PcdBound : public Quantity {
    public:
    real score(const l<size_t> &seq, const Graph &g) const override {
        return pcdBoundValue(g, seq);
    }
};

class SumQ : public Quantity {
    private:
    unique_ptr<Quantity> q1;
    unique_ptr<Quantity> q2;
    public:
    SumQ(unique_ptr<Quantity> q1, unique_ptr<Quantity> q2) : q1(std::move(q1)), q2(std::move(q2)) {}
    real score(const l<size_t> &seq, const Graph &g) const override {
        return (*q1)(seq, g) + (*q2)(seq, g);
    }
};

class Filter : public seqs {
    unique_ptr<seqs> innerGen;
protected:
    const Graph &graph;
public:
    Filter(unique_ptr<seqs> gen, const Graph &graph) : seqs(0, 0, 0), innerGen(std::move(gen)), graph(graph) {}
    virtual bool ifSkip(const l<size_t> &seq) const = 0;
    bool next() override {
        do {
            if (!innerGen->next()) return false;
        } while (ifSkip(get()));
        return true;
    }
    const l<size_t> &get() const override EXPR(innerGen->get())
    l<size_t> &get() override EXPR(innerGen->get())
    size_t approxSize() const override EXPR(innerGen->approxSize());
};

bool is_balanced(conf c) {
    auto [graph_ref, seq_ref] = c;
    const Graph &graph = graph_ref.get();
    const l<size_t> &seq = seq_ref.get();
    const size_t bound = graph.size / 2;
    for (size_t i = 0; i + 1 < seq.size(); ++i) {
        if (seq[i + 1] - seq[i] > bound) return false;
    }
    return graph.size - seq.front() - seq.back() <= bound;
}

class FilterUnbalanced : public Filter {
    public:
    FilterUnbalanced(unique_ptr<seqs> gen, const Graph &graph) : Filter(std::move(gen), graph) {}
    bool ifSkip(const l<size_t> &seq) const override {
        return !is_balanced(std::make_pair(std::ref(graph), std::ref(seq)));
    }
};

bool is_nondominant(conf c) {
    auto [graph_ref, seq_ref] = c;
    const l<size_t> &seq = seq_ref.get();
    size_t consecutiveOccurrences = 0;
    size_t el = 0;
    for (size_t x : seq) {
        if (x == el) {
            if (++consecutiveOccurrences > seq.size() / 2) return false;
        } else {
            el = x;
            consecutiveOccurrences = 1;
        }
    }
    return true;
}

class FilterDominant : public Filter {
    public:
    FilterDominant(unique_ptr<seqs> gen, const Graph &graph) : Filter(std::move(gen), graph) {}
    bool ifSkip(const l<size_t> &seq) const override {
        return is_nondominant(std::make_pair(std::ref(graph), std::ref(seq)));
    }
};

size_t get_opt_agent(conf c)
{
    auto [graph_ref, seq_ref] = c;
    const l<size_t> &seq = seq_ref.get();
    const Graph &graph = graph_ref.get();
    size_t opt = 0;
    real min_cost = numeric_limits<real>::infinity();
    size_t right_agent = 0;
    for (size_t curr_agent = 0; curr_agent < seq.size(); ++curr_agent)
    {
        while (seq[right_agent % seq.size()] + graph.size * (right_agent / seq.size()) - seq[curr_agent] < graph.size / 2)
        {
            ++right_agent;
        }
        if ((right_agent - curr_agent) * 2 - 1 == seq.size())
        {
            real curr_cost = getVertexCost(graph, seq, seq[curr_agent]);
            if (curr_cost < min_cost)
            {
                min_cost = curr_cost;
                opt = curr_agent;
            }
        }
    }
    return opt;
}

bool is_agent_middle(size_t curr_agent, conf c)
{
    auto [graph_ref, seq_ref] = c;
    const l<size_t> &seq = seq_ref.get();
    const Graph &graph = graph_ref.get();
    size_t right_agent = curr_agent;
    while (seq[right_agent % seq.size()] + graph.size * (right_agent / seq.size()) - seq[curr_agent] < graph.size / 2.0) ++right_agent;
    return (right_agent - curr_agent) * 2 - 1 == seq.size();
}

bool is_0_opt(conf c)
{
    /* There may be more then one optimal point.
    // We use the fact that in such case 0 would be selected by get_opt_agent
    // as the first optimal point.
    */
    return get_opt_agent(c) == 0;
}

real get_centrality(conf c, size_t span)
{
    auto [graph_ref, seq_ref] = c;
    const Graph &graph = graph_ref.get();
    const l<size_t> &seq = seq_ref.get();
    size_t center = get_opt_agent(c);
    real sum_of_central_distances = 0;
    for (size_t i = center + seq.size() - span; i <= center + seq.size() + span; ++i)
    {
        sum_of_central_distances += graph.distance(seq[i % seq.size()], seq[center]);
    }
    real sum_of_other_distances = 0;
    for (size_t i = center + span + 1; i < center + seq.size() - span; ++i)
    {
        sum_of_other_distances += graph.distance(seq[i % seq.size()], seq[center]);
    }
    real centrality = sum_of_other_distances / sum_of_central_distances;
    return centrality;
}

std::function<bool(conf)> get_centrality_filter(real threshold, size_t span)
{
    return [=](conf c) EXPR(get_centrality(c, span) >= threshold);
}

bool is_sparse(real threshold, conf c)
{
    auto [graph_ref, seq_ref] = c;
    const Graph &graph = graph_ref.get();
    const l<size_t> &seq = seq_ref.get();
    auto dis_opp = oppositeDistances(graph, seq);
    real min_distance = minimum(dis_opp);
    return min_distance >= threshold;
}

std::function<bool(conf)> get_sparse_filter(real threshold)
{
    return bind_front(is_sparse, threshold);
}

std::function<bool(conf)> negate(std::function<bool(conf)> f)
{
    return [=](conf c) EXPR(!f(c));
}

std::function<bool(conf)> skip(bool cond, std::function<bool(conf)> f)
{
    return cond ? [](conf) EXPR(true) : f;
}

cppcoro::generator<const l<size_t>&> agent1_changes(l<size_t> seq, size_t end)
{
    auto elIter = seq.begin();
    while(++*elIter <= end) {
        while(next(elIter) != seq.end() && *elIter > *next(elIter)) {
            std::iter_swap(elIter, next(elIter));
            ++elIter;
        }
        co_yield seq;
    }
}

enum class Verbosity { none, answer, summary, all };

template<rn::input_range R>
real cost(size_t a, R && bs, const l<real> &ps, const Graph &g) {
    return inner_product(bs.begin(), bs.end(), ps.begin(), 0., plus<>(),
        [&a, &g](size_t b, real p) EXPR(g.distance(a, b) * p));
}

real lotteryCost(size_t a, const l<size_t> &bs, const Graph &g, const lottery &lot) {
    return cost(a, bs, lot(bs), g);
}

bool check(lottery lot, seqs &gen, const Graph &g, Verbosity verbosity) {
    auto printLine = [](const auto &seq, real base_cost, const auto &penalties){
        printR(seq | drop(1));
        cout << "|\t" << r(base_cost) << '\t';
        printR(penalties | transform([](real p)EXPR(r(p))));
        cout << '\n';
    };
    real minimalPenalty = numeric_limits<real>::infinity();
    l<size_t> worstSeq;
    real associatedBaseCost = 0;
    l<real> associatedPenalties;

    if (verbosity >= Verbosity::summary) cerr  << setprecision(2) << scientific << "estimated num of sequences: " << double(gen.approxSize()) << '\n';
    for (const l<size_t> &seq : gen.toGen()) {
        real base_cost = lotteryCost(0, seq, g, lot);
        vector<real> penalties;
        penalties.reserve(g.size - 1);
        for (const auto &seq2 : agent1_changes(seq, g.size - 1)) {
            penalties.push_back(lotteryCost(0, seq2, g, lot) - base_cost);
        }
        real tmp = minimum(penalties);
        if(tmp < minimalPenalty) {
            minimalPenalty = tmp;
            worstSeq = seq;
            associatedBaseCost = base_cost;
            associatedPenalties = penalties;
        }
        if (verbosity == Verbosity::all) printLine(seq, base_cost, penalties);
    }
    bool strategyproof = minimalPenalty >= -EPS;
    if (verbosity == Verbosity::summary) {
        cout << "strategyproof: " << (strategyproof ? "yes" : "no") << '\n';
        printLine(worstSeq, associatedBaseCost, associatedPenalties);
    } else if (verbosity == Verbosity::answer) cout << strategyproof;
    return strategyproof;
}

real rdRatio(lottery lot, seqs &gen, const Graph &g, Verbosity verbosity) {
    real rdVal = 0;

    for (const l<size_t> &seq : gen.toGen()) {
        real baseCost = lotteryCost(0, seq, g, lot);
        real baseRdCost = lotteryCost(0, seq, g, rdLottery);
        for (const auto &seq : agent1_changes(seq, g.size - 1)) {
            real penalty = lotteryCost(0, seq, g, lot) - baseCost;
            if (penalty < -EPS) rdVal = max(rdVal, penalty / (baseRdCost - lotteryCost(0, seq, g, rdLottery)));
        }
    }
    real res = rdVal / (1 + rdVal);
    if (verbosity >= Verbosity::summary) {
        cout << "rd ratio: " << r(res) << '\n';
    } else if (verbosity == Verbosity::answer) cout << r(res);
    return res;
}

real score(const Quantity &scorer, seqs &gen, const Graph &g, Verbosity verbosity, bool avg = false, bool distinctNum = false)
{
    auto printLine = [](const auto &seq, real approx) {
        printR(seq | drop(1));
        cout << "|\t" << r(approx) << '\n';
    };
    size_t sequencesNum = 0;
    real globalApproximationRatio = 0;
    real approximationRatioSum = 0;
    l<size_t> worstSeq;
    if (verbosity >= Verbosity::summary) cerr  << setprecision(2) << scientific << "estimated num of sequences: " << double(gen.approxSize()) << '\n';
    for (const l<size_t> &seq : gen.toGen()) {
        ++sequencesNum;
        const real approx = scorer(seq, g);
        if (approx > globalApproximationRatio)
        {
            globalApproximationRatio = approx;
            worstSeq = seq;
        }
        approximationRatioSum += approx;
        if (verbosity == Verbosity::all) printLine(seq, approx);
    }
    real averageApproximationRatio = approximationRatioSum / sequencesNum;
    real result = avg ? averageApproximationRatio : globalApproximationRatio;
    if (distinctNum) {
        // assign number of disctinct values in the worst sequence to result
        result = 1;
        for (size_t i = 1; i < worstSeq.size(); ++i) {
            if (worstSeq[i] != worstSeq[i - 1]) ++result;
        }
    }
    if (verbosity >= Verbosity::summary) {
        cout << "----------------------------------------" << '\n';
        cout << "number of processed sequences: " << sequencesNum << '\n';
        cout << "approximation ratio: " << r(result) << '\n';
        printLine(worstSeq, globalApproximationRatio);
    }
    else if (verbosity == Verbosity::answer)
        cout << r(result);
    return result;
}

real uniformRank(real) EXPR(1)
