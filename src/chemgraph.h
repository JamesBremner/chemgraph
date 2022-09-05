#include "cGraph.h"
class cChemGraph : public raven::graph::cGraph
{
public:
    void read(const std::string &sin);
    void readSMILES(const std::string &sin);
    bool fragment(
        const std::string &entire,
        const std::string &fragment);
    std::string viz();
    std::map<std::string,int> countAtoms() ;
};
