#include "cGraph.h"
class cChemGraph : public raven::graph::cGraph
{
public:
    void read(const std::string &sin);

    /// @brief read SMILES string
    /// @param sin SMILES string
    
    void readSMILES(const std::string &sin);

    /// @brief conver chemical graph to SMILES representation
    /// @param g graph
    /// @return SMILES string

    std::string graphtoSMILES( const raven::graph::cGraph& g );

    void readNodeFeatures(const std::string &sin);
    void readBondFeatures(const std::string &sin);
    // bool fragment(
    //     const std::string &entire,
    //     const std::string &fragment);
    std::string viz();

    /// @brief Count atoms of each type
    /// @return map of atom counts keyed by atom symbol, e.g. map["C"] = number of carbon atoms

    std::map<std::string,int> countAtoms();

private:
    std::vector< std::vector<int>> myNodeFeatures;
    std::vector< std::vector<int>> myEdgeFeatures;
};
