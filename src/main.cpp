#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include "cStarterGUI.h"
#include "autocell.h"

class cGUI : public cStarterGUI
{
public:
    cGUI()
        : cStarterGUI(
              "Starter",
              {50, 50, 1000, 500}),
          lb(wex::maker::make<wex::label>(fm))
    {
        lb.move(50, 50, 100, 30);
        lb.text("Hello World");

        show();
        run();
    }

private:
    wex::label &lb;
};

class mcell : public cell::cCell
{
public:
    std::string value;

    mcell()
        : cell::cCell()
    {
        value = "?";
    }

    virtual std::string text()
    {
        return value + " ";
    }
};

cell::cAutomaton<mcell> *pA;
std::vector<std::vector<std::string>> vSequence;

std::vector<std::string> tokenize(const std::string &line)
{
    std::vector<std::string> ret;
    std::stringstream sst(line);
    std::string a;
    while (getline(sst, a, ' '))
        ret.push_back(a);
    return ret;
}

void read(const std::string &fname)
{
    std::ifstream ifs(fname);
    if (!ifs.is_open())
        throw std::runtime_error("No input");

    std::vector<std::vector<std::string>> matrix;

    std::string line;
    while (getline(ifs, line))
    {
        auto vtoken = tokenize(line);
        if (!vtoken.size())
            continue;
        if (vtoken[0] == "m")
        {
            std::vector<std::string> row;
            for (int c = 0; c < vtoken.size() - 1; c++)
                row.push_back(vtoken[c + 1]);
            matrix.push_back(row);
        }
        else if (vtoken[0] == "s")
        {
            std::vector<std::string> seq;
            for (int c = 0; c < vtoken.size() - 1; c++)
                seq.push_back(vtoken[c + 1]);
            vSequence.push_back(seq);
        }
        else
            throw std::runtime_error("bad input");
    }

    // populate the grid

    int h = matrix.size();
    if (!h)
        throw std::runtime_error("bad input");
    int w = matrix[0].size();
    pA = new cell::cAutomaton<mcell>(w, h);
    for (int r = 0; r < h; r++)
    {
        if (matrix[r].size() != w)
            throw std::runtime_error("bad input");
        for (int c = 0; c < w; c++)
        {
            pA->cell(c, r)->value = matrix[r][c];
        }
    }

    // display matrix
    std::cout << pA->text() << "\n";
}

void displayFoundSequence(const std::vector<int> &foundSequence)
{
    for (int id : foundSequence)
    {
        auto pmCell = pA->cell(id);
        int c, r;
        pA->coords(c, r, pmCell);
        std::cout << "row " << r << " col " << c << " ";
        std::cout << pmCell->value << "\n";
    }
    std::cout << "\n";
}

void findSequence()
{
    int w, h;
    pA->size(w, h);
    std::vector<int> foundSequence;
    bool found = false;
    // loop over cells in first row
    for (int c = 0; c < w; c++)
    {
        foundSequence.clear();
        found = false;

        if (pA->cell(c, 0)->value == vSequence[0][0])
        {
            foundSequence.push_back(pA->cell(c, 0)->ID());
            found = true;
        }

        if (found)
        {
            // found a possible starting cell in the first row
            // look for the next sequence value in the same column

            auto pmCell = pA->cell(foundSequence[0]);
            int c, r;
            pA->coords(c, r, pmCell);

            std::string nextValue = vSequence[0][1];
            found = false;

            // loop over cells in colum
            for (int r2 = 1; r2 < w; r2++)
            {
                if (pA->cell(c, r2)->value == nextValue)
                {
                    foundSequence.push_back(pA->cell(c, r2)->ID());
                    displayFoundSequence(foundSequence);
                    found = true;
                    break;
                }
            }
            if (!found)
                break;
        }
    }
}

main()
{
    read("../data/data1.txt");
    findSequence();
    return 0;
}
