#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include "cStarterGUI.h"
#include "autocell.h"
#include "cRunWatch.h"

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
    // std::cout << pA->text() << "\n";
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
/// @brief Find sequence with given start point
/// @param[in] seqNo index of sequence sought
/// @param[in] foundSequence vector with index of starting cell
/// @param[out] foundSequence vector with indices of cells in sequence, or empty on failure
/// @param[in] vert true if previous move was vertical
/// @return true if sequence found

bool findSequenceFromStart(
    int seqNo,
    std::vector<int> &foundSequence,
    bool vert)
{
    auto foundSequencebackup = foundSequence;

    int w, h;
    pA->size(w, h);
    bool found = true;
    while (found)
    {
        // toggle search direction
        vert = (!vert);

        // start from last cell found
        auto pmCell = pA->cell(foundSequence.back());
        int c, r;
        pA->coords(c, r, pmCell);

        // look for next value in required sequence
        std::string nextValue = vSequence[seqNo][foundSequence.size()];
        found = false;

        if (vert)
        {
            // loop over cells in column
            for (int r2 = 1; r2 < w; r2++)
            {
                if (pA->cell(c, r2)->value == nextValue)
                {
                    foundSequence.push_back(pA->cell(c, r2)->ID());
                    found = true;
                    break;
                }
            }
        }
        else
        {
            // loop over cells in row
            for (int c2 = 0; c2 < h; c2++)
            {
                if (pA->cell(c2, r)->value == nextValue)
                {
                    foundSequence.push_back(pA->cell(c2, r)->ID());
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            foundSequence = foundSequencebackup;
            return false;
        }

        if (foundSequence.size() == vSequence[seqNo].size())
            return true;
    }
    throw std::runtime_error(
        "Should never come here");
}

std::vector<int> findSequenceStart(int seqNo)
{
    raven::set::cRunWatch aWatcher("findSequence");

    int w, h;
    pA->size(w, h);
    std::vector<int> foundSequence;
    bool found = false;
    bool vert = false;

    // loop over cells in first row
    for (int c = 0; c < w; c++)
    {
        foundSequence.clear();
        found = false;

        if (pA->cell(c, 0)->value == vSequence[seqNo][0])
        {
            foundSequence.push_back(pA->cell(c, 0)->ID());

            if (findSequenceFromStart(
                    seqNo,
                    foundSequence,
                    vert))
            {
                return foundSequence;
            }
        }
    }

    std::cout << "Cannot find sequence starting in 1st row, using wasted moves\n";

    for (int c = 0; c < w; c++)
    {
        for (int r = 1; r < h; r++)
        {
            if (pA->cell(c, r)->value == vSequence[seqNo][0])
            {
                foundSequence.push_back(pA->cell(c, r)->ID());
                vert = false;
                if (findSequenceFromStart(
                        seqNo,
                        foundSequence,
                        vert))
                {
                    return foundSequence;
                }
                vert = true;
                if (findSequenceFromStart(
                        seqNo,
                        foundSequence,
                        vert))
                {
                    return foundSequence;
                }
            }
        }
    }

    std::cout << "Cannot find sequence\n";
    foundSequence.clear();
    return foundSequence;
}

main()
{
    raven::set::cRunWatch::Start();

    read("../data/data3.txt");

    std::cout << "Searching\n"
              << pA->text() << "\n";

    for (int seqNo = 0; seqNo < vSequence.size(); seqNo++)
    {
        std::cout << "for sequence ";
        for (auto &v : vSequence[seqNo])
            std::cout << v << " ";
        std::cout << "\n\n";
        displayFoundSequence(
            findSequenceStart(seqNo));
    }

    raven::set::cRunWatch::Report();
    return 0;
}
