#include <vector>
#include <iostream>
#include <algorithm>
#include "autocell.h"
#include "cSequenceHunter.h"
#include "cRunWatch.h"

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
                    int id = matrix->cell(c, r2)->ID();
                    // check that cell has not been visited previously
                    if (std::find(
                            foundSequence.begin(), foundSequence.end(), id) == foundSequence.end())
                    {
                        // add cell to sequence
                        foundSequence.push_back(id);
                        found = true;
                        break;
                    }
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
                    int id = matrix->cell(c2, r)->ID();
                    // check that cell has not been visited previously
                    if (std::find(
                            foundSequence.begin(), foundSequence.end(), id) == foundSequence.end())
                    {
                        // add cell to sequence
                        foundSequence.push_back(id);
                        found = true;
                        break;
                    }
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

std::vector<int> cSequenceHunter::connect(
    std::vector<int> &seq1,
    std::vector<int> &seq2)
{
    std::vector<int> ret;

    cSequence SQ1(matrix, seq1);
    cSequence SQ2(matrix, seq2);
    int colCount, rowCount;
    matrix->size(colCount, rowCount);

    int colstart, rowstart, colend, rowend;
    bool vertstart, vertend;
    SQ1.lastStep(colstart, rowstart, vertstart);
    SQ2.firstStep(colend, rowend, vertend);

    try
    {

        if (colstart == colend &&
            rowstart == rowend)
        {
            throw 1;
        }
        if (colstart == colend)
        {
            if ((!vertstart) && (!vertend))
            {
                throw 1;
            }
            int col = colstart - 1;
            if (!colstart)
                col = colstart + 1;
            ret.push_back(matrix->index(col, rowstart));
            ret.push_back(matrix->index(col, rowend));
            throw 1;
        }

        if (rowstart == rowend)
        {
            if (vertstart && vertend)
            {
                throw 1;
            }
        }

        if (vertstart)
        {
            if (!vertend)
            {
                ret.push_back(matrix->index(colend, rowstart));
                ret.push_back(matrix->index(colend, rowend));
                throw 1;
            }
            int col = colstart - 1;
            if (colstart == 0)
                col = 1;
            if (colstart == colCount - 1)
                col = colCount - 2;
            ret.push_back(matrix->index(col, rowstart));
            ret.push_back(matrix->index(col, rowend));
            throw 1;
        }

        if (!vertstart)
        {
            if (vertend)
            {
                ret.push_back(matrix->index(colend, rowstart));
                ret.push_back(matrix->index(colend, rowend));
                throw 1;
            }
            else
            {
                // std::cout << "horiz end, horiz start\n";
                int row = rowstart - 1;
                if (rowstart == 0)
                    row = 1;
                if( rowstart == rowCount-1)
                    row = rowCount-2;
                ret.push_back(matrix->index(colstart, row));
                ret.push_back(matrix->index(colend, row));
                throw 1;
            }
        }
    }

    catch (int e)
    {

        std::vector<int> vdb;
        vdb.push_back(matrix->index(colstart, rowstart));
        vdb.insert(vdb.end(), ret.begin(), ret.end());
        vdb.push_back(matrix->index(colend, rowend));
        cSequence sdb(matrix, vdb);
        // std::cout << "connect: ";
        // SQ1.display();
        // std::cout << "with ";
        // sdb.display();
        // std::cout << "to ";
        // SQ2.display();

        return ret;
    }
    throw std::runtime_error(
        "cSequenceHunter::connect failed");
}
