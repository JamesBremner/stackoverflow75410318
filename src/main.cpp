#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include "autocell.h"
#include "cRunWatch.h"
#include "cSequenceHunter.h"


main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: seqHunter <path to data file>\n";
        exit(1);
    }
    raven::set::cRunWatch::Start();

    cSequenceHunter theHunter;

    theHunter.read(argv[1]);

    std::cout << "Searching " << argv[1] << "\n";
    theHunter.displayMatrix();

    for (
        int seqNo = 0;
        seqNo < theHunter.sequenceCount();
        seqNo++)
    {
        std::cout << "for sequence ";
        theHunter.displaySequence(seqNo);
        std::cout << "\n\n";

        auto fseq = theHunter.findSequence(
            seqNo);
        if (fseq.size())
        {
            theHunter.displayFoundSequence(fseq);
        }
    }
    std::vector<int> order;
    for (int k = 0; k < theHunter.sequenceCount(); k++)
    {
        order.push_back(k);
    }
    if( ! theHunter.makePath(order) ) {
        std::reverse(order.begin(),order.end());
        theHunter.makePath( order );
    }

    theHunter.displayFinal();

    raven::set::cRunWatch::Report();
    return 0;
}
