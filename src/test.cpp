#include "cutest.h"
#include "chemgraph.h"

TEST( readSMILES)
{
    cChemGraph g;
    g.readSMILES("CCO");
    auto mp = g.countAtoms();
    CHECK_EQUAL(2,mp["C"]);
    CHECK_EQUAL(1,mp["O"]);

    g.readSMILES("C=CN");
    g.viz();
}

TEST( readGraph )
{
    raven::graph::cGraph ingraph;
    ingraph.add( "C1","H1");
    ingraph.add( "C1","H2");
    ingraph.add( "C1","H3");
    ingraph.add( "C1","C2");
    ingraph.add( "C2","H4");
    ingraph.add( "C2","H5");
    ingraph.add( "C2","O1");
    ingraph.add( "C1","H6");

    cChemGraph chemgraph;
    chemgraph.graphtoSMILES( ingraph );

}

main()
{
    return raven::set::UnitTest::RunAllTests();
}
