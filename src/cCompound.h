
class cCompound
{
public:
    /// @brief read row in PubChem compound database
    /// @param line compound row
    /// @return true if success
    /// https://pubchem.ncbi.nlm.nih.gov/#input_type=list&query=kNIRRlNc7mjB0NlRoJX0v2NQApZh_lmHfeEVmGz_FJR1xw&collection=compound&alias=PubChem%3A%20PubChem%20Compound%20TOC%3A%20Nature%20Chemical%20Biology

    bool read(std::string &line);

    /// @brief Edit distance to other compound
    /// @param other compound
    /// @return steps required to edit this SMILES to reach other SMILES
    /// https://en.wikipedia.org/wiki/Wagner%E2%80%93Fischer_algorithm

    int EditDistance(
        const cCompound &other);

    /// @brief human readable summary of compound
    /// @return compound ID, SMILES, name

    std::string text() const;

    void setSMILES(const std::string &SMILES)
    {
        mySMILES = SMILES;
    }
    std::string getSMILES() const
    {
        return mySMILES;
    }
    int id() const
    {
        return cid;
    }

private:
    std::string mySMILES;
    int cid;
    std::string myName;
};

class cCompoundVector
{
public:
   /// @brief read PubChem compound database
    /// @param fname file name
    /// https://pubchem.ncbi.nlm.nih.gov/#input_type=list&query=kNIRRlNc7mjB0NlRoJX0v2NQApZh_lmHfeEVmGz_FJR1xw&collection=compound&alias=PubChem%3A%20PubChem%20Compound%20TOC%3A%20Nature%20Chemical%20Biology

    void read(const std::string &fname);

    /// @brief find compounds closest to a SMILES string
    /// @param[in] SMILES string to compare
    /// @return human readable list of the closest compounds
    
    std::vector<std::string> closest(
        const std::string &SMILES);

private:
    std::vector<cCompound> myCompound;
};
