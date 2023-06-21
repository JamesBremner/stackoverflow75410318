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

    read(theHunter,argv[1]);

    std::cout << "Searching " << argv[1] << "\n";
    theHunter.displayMatrix();
    theHunter.findSequence();
    theHunter.makePath();
    theHunter.displayFinal();

    raven::set::cRunWatch::Report();
    return 0;
}
