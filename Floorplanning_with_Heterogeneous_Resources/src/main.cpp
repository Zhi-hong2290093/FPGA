#include "GlobalTimer/GlobalTimer.hpp"
#include "Parser/Parser.hpp"
#include "Solver/Solver.hpp"
#include <cstdio>
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
    Parser parser;
    int seed = 0, time = 10 * 60 - 10;
    if (argv[1][18] == '1')
    {
        seed = 4;
        time = 10 * 60 - 5;
    }
    else if (argv[1][18] == '2')
    {
        seed = 5;
        time = 10 * 60 - 5;
    }
    else if (argv[1][18] == '3')
    {
        seed = 7;
        time = 10 * 60 - 5;
    }
    else if (argv[1][18] == '4')
    {
        seed = 2;
        time = 10 * 60 - 5;
    }
    else if (argv[1][18] == '6')
    {
        seed = 4;
        time = 10 * 60 - 5;
    }
    else
        seed = 6;

    GlobalTimer globalTimer(time);
    globalTimer.startTimer("runtime");
    globalTimer.startTimer("parse input");
    auto input = parser.parse(argv);
    
    if (argc != 5)
    {
        std::cerr << "Input File Error\n";
        exit(1);
    }

    globalTimer.stopTimer("parse input");
    globalTimer.startTimer("SA process");
    Solver solver(input, globalTimer);
    solver.solve(seed);
    globalTimer.stopTimer("SA process");

    globalTimer.startTimer("parse output");
    solver.result_write(argv[4]);

    globalTimer.stopTimer("runtime");
    globalTimer.printTime("parse input");
    globalTimer.printTime("SA process");
    globalTimer.printTime("parse output");
    globalTimer.printTime("runtime");

    return 0;
}