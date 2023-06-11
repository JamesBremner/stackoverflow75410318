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
#include "cSequenceHunter.h"

cSequence::cSequence(
    cell::cAutomaton<mcell> *m,
    const std::vector<int> &vid)
    : matrix(m), myID(vid)
{
    for (int i : myID)
        myStepType.push_back(eStepType::step);
}
bool cSequence::firstStep(
    int &col,
    int &row,
    bool &vert) const
{
    if (myID.size() < 2)
        return false;
    matrix->coords(col, row, matrix->cell(myID[0]));
    int c2, r2;
    matrix->coords(c2, r2, matrix->cell(myID[1]));
    vert = (row != r2);
    return true;
}
bool cSequence::lastStep(
    int &col,
    int &row,
    bool &vert) const
{
    if (myID.size() < 2)
        return false;
    int c1, r1;
    matrix->coords(c1, r1, matrix->cell(myID[myID.size() - 2]));
    matrix->coords(col, row, matrix->cell(myID[myID.size() - 1]));
    vert = (row != r1);
    return true;
}

void cSequence::display() const
{
    for (int id : myID)
    {
        auto pmCell = matrix->cell(id);
        int c, r;
        matrix->coords(c, r, pmCell);
        std::cout << " col " << c << " row " << r
                  << " value " << pmCell->value
                  << " id " << id << "\n";
    }
    std::cout << "\n";
}

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
        std::cout << " col " << c << " row " << r
                  << " value " << pmCell->value
                  << " id " << id << "\n";
    }
    std::cout << "\n";
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
    cSequence SQ(matrix, foundSequence);
    int colstart, rowstart;
    bool vert;

    SQ.firstStep(colstart, rowstart, vert);

    // check for start in first row
    if (!rowstart)
        return ret;

    if (!vert)
    {
        // the first move in sequence is horizontal
        // so we can start by simply dropping down from the first row
        ret.push_back(matrix->index(colstart, 0));
    }
    else
    {
        // the first move in sequence is vertical
        // so we need to drop down an adjacent column
        // and then move horizontally to the starting column
        int wmCol = colstart - 1;
        if (colstart == 0)
            wmCol = 1;

        ret.push_back(matrix->index(wmCol, 0));
        ret.push_back(matrix->index(wmCol, rowstart));
    }
    return ret;
}


main()
{
    raven::set::cRunWatch::Start();

    cSequenceHunter theHunter;

    theHunter.read("../data/tid3.txt");

    std::cout << "Searching\n";
    theHunter.displayMatrix();

    std::vector<std::vector<int>> vSeq;
    for (
        int seqNo = 0;
        seqNo < theHunter.sequenceCount();
        seqNo++)
    {
        std::cout << "for sequence ";
        theHunter.displaySequence(seqNo);
        std::cout << "\n\n";

        auto fseq = theHunter.findSequence(seqNo);
        if (fseq.size())
        {
            vSeq.push_back(fseq);
            theHunter.displayFoundSequence(vSeq.back());
        }
    }

    std::cout << "\nFound sequences connected in order input\n";
    for (int k = 0; k < vSeq.size(); k++)
    {
        theHunter.displayFoundSequence(vSeq[k]);
        if (k < vSeq.size() - 1)
            theHunter.displayFoundSequence(
                theHunter.connect(vSeq[k], vSeq[k + 1]));
    }

    raven::set::cRunWatch::Report();
    return 0;
}
