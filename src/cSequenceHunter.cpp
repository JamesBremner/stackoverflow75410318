#include <vector>
#include <iostream>
#include <algorithm>
#include "autocell.h"
#include "cSequenceHunter.h"

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
            ret.push_back(matrix->index(col, rowstart));
            ret.push_back(matrix->index(col, rowend));
            throw 1;
        }

        if (!vertstart)
        {
            if (vertend)
            {
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
