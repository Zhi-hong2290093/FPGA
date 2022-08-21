#include "Parser.hpp"
#include <fstream>
#include <iostream>
#include <memory>

void Parser::readModule(std::string const &filename)
{
    std::ifstream fin(filename);
    std::string buffer;
    int id, clbs, muls;

    if (!fin.is_open())
    {
        std::cerr << "Open Module File Error";
        exit(1);
    }

    while (fin >> buffer)
    {
        id = std::stoi(buffer);
        fin >> buffer;
        clbs = std::stoi(buffer);
        fin >> buffer;
        muls = std::stoi(buffer);
        auto tmp = std::make_shared<Module>(id, clbs, muls, 0, 0);
        IDtoModule[id] = tmp;
        if(muls>0)
            module_list.emplace_back(tmp);
        else
            module_list.insert(module_list.begin(),tmp);
    }
}

void Parser::readNet(std::string const &filename)
{
    std::ifstream fin(filename);
    std::string buffer;
    int id;

    if (!fin.is_open())
    {
        std::cerr << "Open Net File Error";
        exit(1);
    }

    while (fin >> buffer)
    {
        int module_number = 0;
        std::vector<Module::ptr > tmp_vector;

        id = std::stoi(buffer);
        fin >> buffer >> buffer;
        while (buffer != "}")
        {
            int module_id = std::stoi(buffer);
            tmp_vector.emplace_back(IDtoModule[module_id]);
            fin >> buffer;
            module_number++;
        }
        Net *tmp = new Net(id, module_number, tmp_vector);
        net_list.emplace_back(tmp);
    }
}

void Parser::readArch(std::string const &filename)
{
    std::ifstream fin(filename);
    std::string buffer;

    if (!fin.is_open())
    {
        std::cerr << "Open Arch File Error";
        exit(1);
    }

    fin >> buffer;
    row = std::stoi(buffer);
    fin >> buffer;
    col = std::stoi(buffer);
    fin >> buffer;
    s = std::stoi(buffer);
    fin >> buffer;
    d = std::stoi(buffer);
}

SAInput *Parser::parse(char *argv[])
{
    readModule(argv[2]);
    readNet(argv[3]);
    readArch(argv[1]);
    return new SAInput(module_list, net_list,IDtoModule, s, d, row, col);
}