#include "cutest.h"
#include "chemgraph.h"

TEST( chemgraph)
{
    cChemGraph g;
    g.readSMILES("CCO");
    auto mp = g.countAtoms();
    CHECK_EQUAL(2,mp["C"]);
    CHECK_EQUAL(1,mp["O"]);
}

main()
{
    return raven::set::UnitTest::RunAllTests();
}
