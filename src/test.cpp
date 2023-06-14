#include <algorithm>
#include "cutest.h"
#include "autocell.h"
#include "cSequenceHunter.h"

TEST( overlap )
{
    std::vector<int> seq1 { 1,2,3,4,5};
    std::vector<int> seq2 { 3,4,5,6,7};
    std::vector<int> exp12 { 1,2,3,4,5,6,7};

    auto comb = overlap( seq1,seq2 );

    CHECK(std::equal(
        exp12.begin(),
        exp12.end(),
        comb.begin()));
}
main()
{
    return raven::set::UnitTest::RunAllTests();
}