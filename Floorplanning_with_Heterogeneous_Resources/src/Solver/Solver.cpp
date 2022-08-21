#include "Solver.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>

void Solver::remove_redundant_element(std::vector<std::pair<int, int>> &tmp)
{
    int x = tmp.at(0).first, y = tmp.at(0).second;

    for (auto it = tmp.begin() + 1; it != tmp.end();)
    {
        if (x <= it->first && y <= it->second)
            it = tmp.erase(it);
        else
        {
            x = it->first;
            y = it->second;
            it++;
        }
    }
}

std::vector<std::pair<int, int>> Solver::gen_pattern(const Module::ptr m, const int boundary, const int start, const int x, const int sub, const int base)
{
    std::vector<std::pair<int, int>> tmp;

    for (int i = start; i <= boundary; i++)
    {
        int j = 0, clbs_num = 0, muls_num = 0;
        bool flag = false;

        while (!flag)
        {
            if (clbs_num >= m->clbs && muls_num >= m->muls)
            {
                flag = true;
                tmp.push_back(std::make_pair(j, i));
            }
            if (base == 0)
            {
                if ((x + j) % (input->d) == input->s)
                    muls_num += (i - sub) / 3;
                else
                    clbs_num += i;
            }
            else
            {
                if ((x + j - base) % (input->s - base + 1) == (input->d - 1))
                {
                    muls_num += (i - sub) / 3;
                }
                else
                    clbs_num += i;
            }
            j++;
        }
    }
    return tmp;
}

void Solver::init_pattern()
{
    int base;
    if (input->s < input->d)
        base = 0;
    else
        base = input->s - input->d + 1;

    for (auto m : input->module_list)
    {
        std::vector<Pattern> pat1, pat2, pat3;
        for (int x = 0; x <= input->d + base; x++)
        {
            std::vector<std::pair<int, int>> tmp;
            int boundary;
            Pattern pattern1;
            Pattern pattern2;
            Pattern pattern3;

            if (m->muls > 0)
            {
                boundary = m->muls * 3 + 2;
                tmp = gen_pattern(m, boundary, 3, x, 0, base);
            }
            else
                tmp = gen_pattern(m, m->clbs, 1, x, 0, base);

            remove_redundant_element(tmp);
            pattern1.rec = tmp;
            pat1.emplace_back(pattern1);

            if (m->muls > 0)
            {
                boundary = m->muls * 3 + 5;
                tmp = gen_pattern(m, boundary, 5, x, 2, base);
            }
            else
                tmp = gen_pattern(m, m->clbs, 1, x, 2, base);

            remove_redundant_element(tmp);
            pattern2.rec = tmp;
            pat2.emplace_back(pattern2);

            if (m->muls > 0)
            {
                boundary = m->muls * 3 + 5;
                tmp = gen_pattern(m, boundary, 4, x, 1, base);
            }
            else
                tmp = gen_pattern(m, m->clbs, 1, x, 1, base);

            remove_redundant_element(tmp);
            pattern3.rec = tmp;
            pat3.emplace_back(pattern3);
        }
        m->pattern_list.emplace_back(pat1);
        m->pattern_list.emplace_back(pat2);
        m->pattern_list.emplace_back(pat3);
    }
}

void Solver::init_NPE(std::vector<int> &npe)
{
    int row_width = 0, xCnt = 0, yCnt = 0, cCnt = 0, col_height = 0, base = 0, max_x = 0, x;
    if (input->s >= input->d)
        base = input->s - input->d + 1;
    for (auto m : input->module_list)
    {
        m->initIRL(input->d, base);

        if (base == 0)
            x = row_width % input->d;
        else
        {
            if (row_width < base + input->d)
                x = row_width;
            else
                x = (row_width - base) % input->d + base;
        }

        int y = col_height % 3;
        auto &p = m->pattern_list.at(y).at(x); /*get_pattern_list()*/
        int index = p.IRL.size() / 2;
        col_height += p.IRL.at(index).at(3);
        if (max_x < p.IRL.at(index).at(2))
            max_x = p.IRL.at(index).at(2);

        if (col_height >= input->row)
        {
            xCnt += 1;
            if (xCnt >= 2)
            {
                npe.emplace_back(-2); //insert vertical cut
                cCnt += 1;
                xCnt = 1;
                row_width += max_x;
                max_x = 0;
            }
            col_height = p.IRL.at(index).at(3);
            yCnt = 0;
        }

        npe.emplace_back(m->id);
        yCnt += 1;
        if (yCnt >= 2)
        {
            npe.emplace_back(-1); //insert hertical cut
            cCnt += 1;
            yCnt = 1;
        }
    }
    for (size_t i = cCnt; i < input->module_list.size() - 1; ++i)
        npe.emplace_back(-2); //insert vorizontal cut
}

Module::ptr Solver::build_slicing_tree(std::vector<int> const &npe)
{
    std::vector<Module::ptr> post_order;
    int base = 0;
    if (input->s >= input->d)
        base = input->s - input->d + 1;
    for (auto n : npe)
    {
        if (n >= 0)
        {
            post_order.emplace_back(input->IDtoModule[n]);
        }
        else
        {
            auto m = std::make_shared<Module>(n, 0, 0, 0, 0);
            std::vector<Pattern> pat1(input->d + base), pat2(input->d + base), pat3(input->d + base);
            m->pattern_list.push_back(pat1);
            m->pattern_list.push_back(pat2);
            m->pattern_list.push_back(pat3);
            m->right_child = post_order.back();
            post_order.pop_back();
            m->left_child = post_order.back();
            post_order.pop_back();
            post_order.emplace_back(m);
            m->updateIRL(input->d, input->col, input->row, base);
        }
    }
    return post_order.back();
}

void Solver::setPosition(Module::ptr m, size_t const &choice, int const &x, int const &y)
{
    int basic_x, base = 0, basic_y = y % 3;
    if (input->s >= input->d)
        base = input->s - input->d + 1;
    if (base == 0)
        basic_x = x % input->d;
    else
    {
        if (x < base)
            basic_x = x;
        else
            basic_x = (x - base) % input->d + base;
    }

    if (m->id > 0)
    {
        m->x_pos = x;
        m->y_pos = y;
        m->width = m->pattern_list.at(basic_y).at(basic_x).IRL.at(choice).at(2);
        m->height = m->pattern_list.at(basic_y).at(basic_x).IRL.at(choice).at(3);
    }
    else
    {
        setPosition(m->left_child, m->pattern_list.at(basic_y).at(basic_x).IRL.at(choice).at(4), x, y);
        int offset_x = 0, offset_y = 0;

        if (m->id == -2)
            offset_x = m->left_child->pattern_list.at(basic_y).at(basic_x).IRL.at(m->pattern_list.at(basic_y).at(basic_x).IRL.at(choice).at(4)).at(2);
        else
            offset_y = m->left_child->pattern_list.at(basic_y).at(basic_x).IRL.at(m->pattern_list.at(basic_y).at(basic_x).IRL.at(choice).at(4)).at(3);
        setPosition(m->right_child, m->pattern_list.at(basic_y).at(basic_x).IRL.at(choice).at(5), x + offset_x, y + offset_y);
    }
}

double Solver::calWL() const
{
    double WL = 0;
    for (auto n : input->net_list)
    {
        double pin_x_max = 0, pin_y_max = 0, pin_x_min = std::numeric_limits<int>::max(), pin_y_min = std::numeric_limits<int>::max();
        for (auto m : n->module_list)
        {
            if (m->x_pos + m->width / 2.0 <= pin_x_min)
                pin_x_min = m->x_pos + m->width / 2.0;
            if (m->y_pos + m->height / 2.0 <= pin_y_min)
                pin_y_min = m->y_pos + m->height / 2.0;
            if (m->x_pos + m->width / 2.0 > pin_x_max)
                pin_x_max = m->x_pos + m->width / 2.0;
            if (m->y_pos + m->height / 2.0 > pin_y_max)
                pin_y_max = m->y_pos + m->height / 2.0;
        }
        WL += (pin_x_max - pin_x_min + pin_y_max - pin_y_min);
    }
    return WL;
}

double Solver::calCost(std::vector<int> const &npe, bool const &focusWirelength)
{
    auto root = build_slicing_tree(npe);
    double minArea = std::numeric_limits<int>::max(), area = 0;
    size_t choice = 0;
    for (size_t i = 0; i < root->pattern_list.at(0).at(0).IRL.size(); i++)
    {
        auto record = root->pattern_list.at(0).at(0).IRL.at(i);
        if (record.at(2) > input->col && record.at(3) > input->row)
            area = record.at(2) * record.at(3) - input->col * input->row;
        else if (record.at(2) > input->col && record.at(3) <= input->row)
            area = (record.at(2) - input->col) * input->row;
        else if (record.at(2) <= input->col && record.at(3) > input->row)
            area = input->col * (record.at(3) - input->row);
        else
            area = 0;

        if (minArea > area)
        {
            minArea = area;
            choice = i;
        }
    }

    if (focusWirelength == false)
        return minArea * 10;

    setPosition(root, choice, 0, 0);
    return minArea * 10000 + calWL();
}

void Solver::invertCut(std::vector<int> &npe, size_t pos)
{
    while (pos < npe.size() && npe[pos] < 0)
    {
        if (npe[pos] == -2)
            npe[pos] = -1;
        else
            npe[pos] = -2;
        pos += 1;
    }
}

bool Solver::checkSkewed(std::vector<int> const &npe, size_t const &pos) const
{
    if (npe[pos + 1] < 0 && pos != 0)
    {
        if (npe[pos - 1] != npe[pos + 1])
            return true;
    }
    else if (npe[pos] < 0 && pos + 1 != npe.size() - 1)
    {
        if (npe[pos] != npe[pos + 2])
            return true;
    }
    return false;
}

bool Solver::checkBallot(std::vector<int> const &npe, size_t const &pos) const
{
    if (npe[pos + 1] < 0)
    {
        size_t op = 0;
        for (size_t i = 0; i <= pos + 1; ++i)
            if (npe[i] < 0)
                op += 1;

        if (2 * op >= pos + 1)
            return false;
    }
    return true;
}

std::vector<int> Solver::perturbNPE(std::vector<int> npe, int const &type)
{
    int pos[npe.size()], posCnt = 0, pos1 = 0, pos2 = 0, violationCnt = 0;
    switch (type)
    {
    case 0:
        for (size_t i = 0; i < npe.size(); ++i)
            if (npe[i] >= 0)
                pos[posCnt++] = i;

        pos1 = pos2 = rand() % posCnt;
        while (pos1 == pos2)
            pos2 = rand() % posCnt;
        pos1 = pos[pos1];
        pos2 = pos[pos2];
        std::swap(npe[pos1], npe[pos2]);
        break;

    case 1:
        for (size_t i = 1; i < npe.size(); ++i)
            if (npe[i - 1] >= 0 && npe[i] < 0)
                pos[posCnt++] = i;

        pos1 = rand() % posCnt;
        pos1 = pos[pos1];
        invertCut(npe, pos1);
        break;

    case 2:
        for (size_t i = 0; i < npe.size() - 1; ++i)
            if ((npe[i] >= 0 && npe[i + 1] < 0) ||
                (npe[i] < 0 && npe[i + 1] >= 0))
                pos[posCnt++] = i;

        do
        {
            pos1 = rand() % posCnt;
            violationCnt += 1;
        } while ((checkSkewed(npe, pos[pos1]) && checkBallot(npe, pos[pos1])) == false && violationCnt < posCnt);

        if (violationCnt < posCnt)
        {
            pos1 = pos[pos1];
            pos2 = pos1 + 1;
            std::swap(npe[pos1], npe[pos2]);
        }
        break;
    }
    return npe;
}

std::vector<int> Solver::saProcess(double const &c, double const &r, int const &k, std::vector<int> const &NPE, bool const &focusWirelength)
{
    int MT = 0, uphill = 0, reject = 0, N = k * input->module_list.size();
    std::vector<int> BE, E;
    BE = E = NPE;
    double bestCost = 0, cost = 0;
    bestCost = cost = calCost(E, focusWirelength);

    if (bestCost == 0)
        goto finishSA;

    do
    {
        double T0 = 1000;
        do
        {
            MT = uphill = reject = 0;
            do
            {
                if (globalTimer.overTime())
                    goto finishSA;

                int type = 0;
                if (focusWirelength == false)
                    type = rand() % 3;
                std::vector<int> NE = perturbNPE(E, type);
                MT += 1;
                double newCost = calCost(NE, focusWirelength);
                double deltaCost = newCost - cost;
                if (deltaCost < 0 || (double)rand() / RAND_MAX < exp(-1 * deltaCost / T0))
                {
                    if (deltaCost > 0)
                        uphill += 1;

                    E = NE;
                    cost = newCost;

                    if (cost < bestCost)
                    {
                        BE = E;
                        bestCost = cost;
                        if (bestCost == 0)
                            goto finishSA;
                    }
                }
                else
                {
                    reject += 1;
                }
            } while (uphill <= N && MT <= 2 * N);
            T0 = r * T0;
        } while (reject / MT <= 0.95 && T0 >= c);
    } while (focusWirelength == false);

finishSA:
    calCost(BE, true);
    return BE;
}

void Solver::solve(int seed)
{
    srand(seed);
    std::vector<int> npe;
    init_pattern();
    init_NPE(npe);
    npe = saProcess(0.1, 0.9, 10, npe, false);
    if (!globalTimer.overTime())
        std::cout<< "area finish" << std::endl;
    npe = saProcess(0.1, 0.865, 6, npe, true);
    std::cout<< seed << " WL:" << calWL() << std::endl;
}

void Solver::result_write(const std::string filename)
{
    std::ofstream fout(filename);
    for (auto i : input->module_list)
        fout << i->id << " " << i->x_pos << " " << i->y_pos << " " << i->width << " " << i->height << std::endl;
    fout << calWL() << std::endl;
}
