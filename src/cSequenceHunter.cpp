#include <vector>
#include <iostream>
#include "autocell.h"
#include "cSequenceHunter.h"

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
        if (colstart == colend) {
            if( (!vertstart) && (!vertend)) {
                throw 1;
            }
            int col = colstart-1;
            if( ! colstart )
                col = colstart + 1;
            ret.push_back(matrix->index(col, rowstart));
            ret.push_back(matrix->index(col, rowend));
            throw 1;
        }

        if (rowstart == rowend) {
            if( vertstart && vertend) {
                throw 1;
            }
        }

        if( vertstart ) {
            if( ! vertend ) {
            ret.push_back(matrix->index(colend, rowstart));
            ret.push_back(matrix->index(colend, rowend));
               throw 1;
            }
            int col = colstart-1;
            if( colstart == 0 )
                col = 1;
            ret.push_back(matrix->index(col, rowstart));
            ret.push_back(matrix->index(col, rowend));
            throw 1;
        }

        if( ! vertstart ) {
            if( vertend ) {
                
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
        std::cout << "connect: ";
        SQ1.display();
        std::cout << "with ";
        sdb.display();
        std::cout << "to ";
        SQ2.display();

        return ret;
    }
    throw std::runtime_error(
        "cSequenceHunter::connect failed");
}
