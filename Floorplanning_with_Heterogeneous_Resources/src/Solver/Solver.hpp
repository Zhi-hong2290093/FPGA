#pragma once
#include "../Data/Data.hpp"
#include "../GlobalTimer/GlobalTimer.hpp"
#include <string>

class Solver
{
    SAInput *input;
    GlobalTimer &globalTimer;
    std::vector<Module::ptr > ModuleNodes, CutNodes;

    void init_pattern();
    void init_NPE(std::vector<int> &npe);
    void remove_redundant_element(std::vector<std::pair<int, int>> &tmp);
    Module::ptr build_slicing_tree(std::vector<int> const &npe);
    void setPosition(Module::ptr m, size_t const &choice, int const &x, int const &y);
    void invertCut(std::vector<int> &npe, size_t pos);
    bool checkBallot(std::vector<int> const &npe, size_t const &pos) const;
    std::vector<int> perturbNPE(std::vector<int> npe, int const &type);
    double calCost(std::vector<int> const &npe, bool const &focusWirelength);
    bool checkSkewed(std::vector<int> const &npe, size_t const &pos) const;
    double calWL() const;
    std::vector<int> saProcess(double const &c, double const &r, int const &k, std::vector<int> const &NPE, bool const &focusWirelength);
    std::vector<std::pair<int, int>> gen_pattern(const Module::ptr m, const int boundary, const int start, const int x, const int sub, const int base);

public:
    void solve(int seed);
    void result_write(const std::string filename);
    Solver(SAInput *input, GlobalTimer &globalTimer)
        : input(input),globalTimer(globalTimer) {}
};
