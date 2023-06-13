#include <string>
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


main(int argc, char* argv[] )
{
    if( argc != 2 ) {
        std::cout << "Usage: seqHunter <path to data file>\n";
        exit(1);
    }
    raven::set::cRunWatch::Start();

    cSequenceHunter theHunter;

    theHunter.read(argv[1]);

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
