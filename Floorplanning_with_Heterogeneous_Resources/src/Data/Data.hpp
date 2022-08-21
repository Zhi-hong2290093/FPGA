#pragma once
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <memory>

struct Pattern
{
    std::vector<std::pair<int, int>> rec;
    std::vector<std::vector<int>> IRL; // x, y, width, height, left choice, right choice
};

struct Module
{
    using ptr = std::shared_ptr<Module>;

    int id, clbs, muls, x_pos, y_pos,
        basic_width, basic_height, width, height;
    std::vector<std::vector<Pattern>> pattern_list;
    Module::ptr right_child, left_child;

    Module() {}
    Module(int const &id, int const &clbs, int const &muls, int const &x_pos, int const &y_pos)
        : id(id), clbs(clbs), muls(muls), x_pos(x_pos), y_pos(y_pos),
          basic_width(-1), basic_height(3), width(0), height(0),
          right_child(nullptr), left_child(nullptr) {}
    void updateIRL(int const d, int const col, int const row, int const base);
    void initIRL(int const d, int const base);
    void get_realization_list_V(const int &x, const int &y, int const d, int const row, int const base);
    void get_realization_list_H(const int &x, const int &y, int const col);
};

struct Net
{
    int id, module_number;
    std::vector<Module::ptr > module_list;

    Net(int const &id, int const &module_number, std::vector<Module::ptr > const &module_list)
        : id(id), module_number(module_number), module_list(module_list) {}
};

struct SAInput
{
    std::vector<Module::ptr > module_list;
    std::vector<Net *> net_list;
    std::unordered_map<int, Module::ptr > IDtoModule;
    int s, d, row, col;

    SAInput(std::vector<Module::ptr> const &module_list, std::vector<Net *> const &net_list, std::unordered_map<int, Module::ptr > const &IDtoModule, int const &s, int const &d, int const &row, int const &col)
        : module_list(module_list), net_list(net_list), IDtoModule(IDtoModule), s(s), d(d), row(row), col(col) {}
};
