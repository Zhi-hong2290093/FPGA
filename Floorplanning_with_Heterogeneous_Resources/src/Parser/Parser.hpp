#pragma once
#include "../Data/Data.hpp"
#include <string>
#include <unordered_map>
#include <vector>

class Parser
{
    std::vector<Module::ptr > module_list;
    std::vector<Net *> net_list;
    std::unordered_map<int, Module::ptr > IDtoModule;
    int s, d, row, col;

    void readModule(std::string const &filename);
    void readNet(std::string const &filename);
    void readArch(std::string const &filename);

public:
    Parser() {}
    SAInput *parse(char *argv[]);
};