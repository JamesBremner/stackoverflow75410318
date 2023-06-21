#include <algorithm>
#include "cutest.h"
#include "autocell.h"
#include "cSequenceHunter.h"

TEST(overlap)
{
    std::vector<int> seq1{1, 2, 3, 4, 5};
    std::vector<int> seq2{3, 4, 5, 6, 7};
    std::vector<int> exp12{1, 2, 3, 4, 5, 6, 7};

    auto comb = overlap(seq1, seq2);

    CHECK(std::equal(
        exp12.begin(),
        exp12.end(),
        comb.begin()));
}

TEST(tid3)
{
    cSequenceHunter theHunter;
    theHunter.read("../data/tid3.txt");
    for (
        int seqNo = 0;
        seqNo < theHunter.sequenceCount();
        seqNo++)
    {
        theHunter.findSequence(
            seqNo);
    }

    std::vector<int> order;
    for (int k = 0; k < theHunter.sequenceCount(); k++)
    {
        order.push_back(k);
    }
    if (!theHunter.makePath(order))
    {
        std::reverse(order.begin(), order.end());
        theHunter.makePath(order);
    }
    auto actual = theHunter.getPath();
    CHECK_EQUAL(5, actual.size());
    std::vector<int> expected{3, 13, 10, 20, 22};
    CHECK(std::equal(
        expected.begin(), expected.end(),
        actual.begin()));
}
main()
{
    return raven::set::UnitTest::RunAllTests();
}