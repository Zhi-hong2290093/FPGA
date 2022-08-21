#include "Data.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <omp.h>
#include <vector>
#define alpha 3
#define RATIO 10

void Module::initIRL(int const d, int const base)
{
    for (int i = 0; i < d + base; i++)
        for (int j = 0; j < 3; j++)
        {
            for (size_t k = 0; k < (pattern_list[j][i].rec).size(); k++)
            {
                int width = (pattern_list[j][i].rec)[k].first;
                int height = (pattern_list[j][i].rec)[k].second;
                (pattern_list[j][i].IRL).emplace_back(std::vector<int>({i, j, width, height, 0, 0}));
            }
            std::sort(pattern_list[j][i].IRL.begin(), pattern_list[j][i].IRL.end(),
                      [](std::vector<int> &a, std::vector<int> &b)
                      { return a.at(3) > b.at(3); });
        }
}

void Module::updateIRL(int const d, int const col, int const row, int const base)
{
    if (id == 0)
        return;

    int d_base = d + base;
    for (int x = 0; x < d_base; x++)
        for (int y = 0; y < 3; y++)
        {
            if (id == -2)
                get_realization_list_V(x, y, d, row, base);
            else if (id == -1)
                get_realization_list_H(x, y, col);
        }
}

void Module::get_realization_list_V(const int &x, const int &y, int const d, int const row, int const base)
{
    pattern_list.at(y).at(x).IRL.clear();
    int upper_height;
    std::vector<std::vector<int>> irlv = left_child->pattern_list.at(y).at(x).IRL;
    int len = irlv.size();
    for (int i = len - 1; i >= 0; --i)
    {
        int x_q;
        if (base == 0)
            x_q = (x + irlv.at(i).at(2)) % d;
        else
        {
            if ((x + irlv.at(i).at(2)) < base)
                x_q = (x + irlv.at(i).at(2));
            else
                x_q = (x + irlv.at(i).at(2) - base) % d + base;
        }

        std::vector<std::vector<int>> irlq = right_child->pattern_list.at(y).at(x_q).IRL;

        if (i == 0)
            upper_height = int(row * alpha) + 1;
        else
            upper_height = irlv.at(i - 1).at(3);

        int j = 0;
        while (j < int(irlq.size()) - 1 && irlq.at(j).at(3) > irlv.at(i).at(3))
            j++;

        while (j >= 0 && irlq.at(j).at(3) < upper_height)
        {
            int h = std::max(irlq.at(j).at(3), irlv.at(i).at(3));
            int w = irlq.at(j).at(2) + irlv.at(i).at(2);
            double r = double(std::max(h, w)) / double(std::min(h, w));
            if ((pattern_list.at(y).at(x).IRL.size() == 0) ||
                (pattern_list.at(y).at(x).IRL.size() != 0 && w < pattern_list.at(y).at(x).IRL.at(0).at(2) && r < RATIO))
                pattern_list.at(y).at(x).IRL.insert(pattern_list.at(y).at(x).IRL.begin(), std::vector<int>({0, 0, w, h, i, j}));
            j--;
        }
    }
}

void Module::get_realization_list_H(const int &x, const int &y, int const col)
{
    pattern_list.at(y).at(x).IRL.clear();
    int upper_width;
    std::vector<std::vector<int>> irlv = left_child->pattern_list.at(y).at(x).IRL;
    int len = irlv.size();
    for (int i = 0; i <= len - 1; i++)
    {
        int y_q = (y + irlv.at(i).at(3)) % 3;
        std::vector<std::vector<int>> irlq = right_child->pattern_list.at(y_q).at(x).IRL;
        if (i == int(irlv.size()) - 1)
            upper_width = int(col * alpha) + 1;
        else
            upper_width = irlv.at(i + 1).at(2);
        int j = irlq.size() - 1;
        while (j > 0 && irlq.at(j).at(2) > irlv.at(i).at(2))
            j--;

        while (j < int(irlq.size()) && irlq.at(j).at(2) < upper_width)
        {
            int w = std::max(irlq.at(j).at(2), irlv.at(i).at(2));
            int h = irlq.at(j).at(3) + irlv.at(i).at(3);
            double r = double(std::max(h, w)) / double(std::min(h, w));
            if ((pattern_list.at(y).at(x).IRL.size() == 0) ||
                (pattern_list.at(y).at(x).IRL.size() != 0 && h < pattern_list.at(y).at(x).IRL.back().at(3) && r < RATIO))
            {
                pattern_list.at(y).at(x).IRL.emplace_back(std::vector<int>({0, 0, w, h, i, j}));
            }
            j++;
        }
    }
}
