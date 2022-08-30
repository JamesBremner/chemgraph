#include "cGraph.h"
class cChemGraph : public raven::graph::cGraph
{
public:
    void read(const std::string &sin);
    void readSMILES(const std::string &sin);
    std::string viz();
};
