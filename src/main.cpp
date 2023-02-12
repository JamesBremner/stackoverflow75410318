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

class cSequenceHunter
{
public:
    void read(const std::string &fname);

    std::vector<int> findSequence(int seqNo);

    int sequenceCount() const;

    void displayMatrix() const;
    void displaySequence(int seqNo) const;
    void displayFoundSequence(
        const std::vector<int> &foundSequence) const;

private:
    cell::cAutomaton<mcell> *matrix;
    std::vector<std::vector<std::string>> vSequence;

    /// @brief Find sequence with given start point
    /// @matrixram[in] seqNo index of sequence sought
    /// @matrixram[in] foundSequence vector with index of starting cell
    /// @matrixram[out] foundSequence vector with indices of cells in sequence, or empty on failure
    /// @matrixram[in] vert true if previous move was vertical
    /// @return true if sequence found

    bool findSequenceFromStart(
        int seqNo,
        std::vector<int> &foundSequence,
        bool vert);

    /// @brief find 'wasted moves' required to reach sequence start from first row
    /// @param foundSequence 
    /// @return vector of cell indices needed as stepping stones to sequence start
    
    std::vector<int> wastedMoves(
        std::vector<int> &foundSequence);

    std::vector<std::string>
    tokenize(
        const std::string &line);
};

int cSequenceHunter::sequenceCount() const
{
    return vSequence.size();
}
void cSequenceHunter::displayMatrix() const
{
    std::cout << matrix->text() << "\n";
}
void cSequenceHunter::displaySequence(int seqNo) const
{
    for (auto &v : vSequence[seqNo])
        std::cout << v << " ";
}

std::vector<std::string>
cSequenceHunter::tokenize(
    const std::string &line)
{
    std::vector<std::string> ret;
    std::stringstream sst(line);
    std::string a;
    while (getline(sst, a, ' '))
        ret.push_back(a);
    return ret;
}

void cSequenceHunter::read(const std::string &fname)
{
    std::ifstream ifs(fname);
    if (!ifs.is_open())
        throw std::runtime_error("No input");

    std::vector<std::vector<std::string>> vv;

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
            vv.push_back(row);
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

    int h = vv.size();
    if (!h)
        throw std::runtime_error("bad input");
    int w = vv[0].size();
    matrix = new cell::cAutomaton<mcell>(w, h);
    for (int r = 0; r < h; r++)
    {
        if (vv[r].size() != w)
            throw std::runtime_error("bad input");
        for (int c = 0; c < w; c++)
        {
            matrix->cell(c, r)->value = vv[r][c];
        }
    }
}

void cSequenceHunter::displayFoundSequence(
    const std::vector<int> &foundSequence) const
{
    for (int id : foundSequence)
    {
        auto pmCell = matrix->cell(id);
        int c, r;
        matrix->coords(c, r, pmCell);
        std::cout << "row " << r << " col " << c << " ";
        std::cout << pmCell->value << "\n";
    }
    std::cout << "\n";
}

bool cSequenceHunter::findSequenceFromStart(
    int seqNo,
    std::vector<int> &foundSequence,
    bool vert)
{
    auto foundSequencebackup = foundSequence;

    int w, h;
    matrix->size(w, h);
    bool found = true;
    while (found)
    {
        // toggle search direction
        vert = (!vert);

        // start from last cell found
        auto pmCell = matrix->cell(foundSequence.back());
        int c, r;
        matrix->coords(c, r, pmCell);

        // look for next value in required sequence
        std::string nextValue = vSequence[seqNo][foundSequence.size()];
        found = false;

        if (vert)
        {
            // loop over cells in column
            for (int r2 = 1; r2 < w; r2++)
            {
                if (matrix->cell(c, r2)->value == nextValue)
                {
                    foundSequence.push_back(matrix->cell(c, r2)->ID());
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
                if (matrix->cell(c2, r)->value == nextValue)
                {
                    foundSequence.push_back(matrix->cell(c2, r)->ID());
                    found = true;
                    break;
                }
            }
        }
        if (!found)
        {
            foundSequence = foundSequencebackup;
            return false;
        }

        if (foundSequence.size() == vSequence[seqNo].size())
            return true;
    }
    throw std::runtime_error(
        "Should never come here");
}

std::vector<int> cSequenceHunter::findSequence(int seqNo)
{
    raven::set::cRunWatch aWatcher("findSequence");

    int w, h;
    matrix->size(w, h);
    std::vector<int> foundSequence;
    bool found = false;
    bool vert = false;

    // loop over cells in first row
    for (int c = 0; c < w; c++)
    {
        foundSequence.clear();
        found = false;

        if (matrix->cell(c, 0)->value == vSequence[seqNo][0])
        {
            foundSequence.push_back(matrix->cell(c, 0)->ID());

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

    found = false;
    for (int c = 0; c < w; c++)
    {
        for (int r = 1; r < h; r++)
        {
            if (matrix->cell(c, r)->value == vSequence[seqNo][0])
            {
                foundSequence.push_back(matrix->cell(c, r)->ID());
                vert = false;
                if (findSequenceFromStart(
                        seqNo,
                        foundSequence,
                        vert))
                {
                    found = true;
                    break;
                }
                vert = true;
                if (findSequenceFromStart(
                        seqNo,
                        foundSequence,
                        vert))
                {
                    found = true;
                    break;
                }
            }
        }
        if (found)
            break;
    }
    if (!found)
    {
        std::cout << "Cannot find sequence\n";
        foundSequence.clear();
        return foundSequence;
    }

    for (int id : wastedMoves(foundSequence))
    {
        int wc, wr;
        matrix->coords(wc, wr, matrix->cell(id));
        std::cout << "WM " << wc << " " << wr << "\n";
    }

    return foundSequence;
}

std::vector<int> cSequenceHunter::wastedMoves(std::vector<int> &foundSequence)
{
    std::vector<int> ret;
    int colstart, rowstart;
    matrix->coords(colstart, rowstart, matrix->cell(foundSequence[0]));

    // check for start in first row
    if (!rowstart)
        return ret;

    // find direction of first sequence move
    int col2, row2;
    matrix->coords(col2, row2, matrix->cell(foundSequence[1]));
    bool vert = (rowstart != row2);

    if (!vert)
    {
        ret.push_back(matrix->index(colstart, 0));
    }
    else
    {
        int wmCol;
        if (colstart > 0)
            wmCol = colstart - 1;
        else
            wmCol = 1;

        ret.push_back(matrix->index(wmCol, 0));
        ret.push_back(matrix->index(wmCol,rowstart));
    }
    return ret;
}

main()
{
    raven::set::cRunWatch::Start();

    cSequenceHunter theHunter;

    theHunter.read("../data/data3.txt");

    std::cout << "Searching\n";
    theHunter.displayMatrix();

    for (
        int seqNo = 0;
        seqNo < theHunter.sequenceCount();
        seqNo++)
    {
        std::cout << "for sequence ";
        theHunter.displaySequence(seqNo);
        std::cout << "\n\n";
        theHunter.displayFoundSequence(
            theHunter.findSequence(seqNo));
    }

    raven::set::cRunWatch::Report();
    return 0;
}
