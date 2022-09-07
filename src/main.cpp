#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <wex.h>
#include <window2file.h>
#include "cStarterGUI.h"
#include "chemgraph.h"

class cCompound
{
public:
    bool read(std::string &line);

private:
    std::string mySMILES;
    int cid;
};

class cCompoundVector
{
public:
    void read(const std::string &fname);

private:
    std::vector<cCompound> myCompound;
};

bool cCompound::read(std::string &line)
{
    int p = line.find("\"");
    while (p != -1)
    {
        int q = line.find("\"", p + 1);
        line = line.substr(0, p) + line.substr(q+1 );
        p = line.find("\"");
    }
    std::string a;
    std::vector<std::string> cols;
    for (
        std::stringstream sst(line);
        getline(sst, a, ',');)
        cols.push_back(a);
    if( cols.size() < 14 )
        return false;
    cid = atoi(cols[0].c_str());
    mySMILES = cols[13];
    return true;

}

void cCompoundVector::read(const std::string &fname)
{
    std::ifstream f(fname);
    if (!f.is_open())
        throw std::runtime_error(
            "cCompoundVector::read cannot open input file");

    std::string line;
    getline(f, line);
    while (getline(f, line))
    {
        cCompound C;
        if( ! C.read(line) )
            continue;
        myCompound.push_back(C);
    }
}

class cGUI : public cStarterGUI
{
public:
    cGUI()
        : cStarterGUI(
              "ChemGraph",
              {50, 50, 1000, 500}),
          ebsmiles(wex::maker::make<wex::editbox>(fm)),
          bnsmiles(wex::maker::make<wex::button>(fm)),
          graphPanel(wex::maker::make<wex::panel>(fm)),
          lbNodefeatures(wex::maker::make<wex::label>(fm)),
          pnAtoms(wex::maker::make<wex::panel>(fm)),
          lbBondfeatures(wex::maker::make<wex::label>(fm)),
          pnBonds(wex::maker::make<wex::panel>(fm))
    {
        cCompoundVector vC;
        vC.read("../data/PubChem_compound_list.csv");

        ebsmiles.move(50, 50, 300, 30);
        ebsmiles.text("");
        bnsmiles.move(400, 50, 100, 30);
        bnsmiles.text("Vizualize");
        bnsmiles.events().click(
            [this]
            {
                myG.readSMILES(ebsmiles.text());
                std::cout << myG.viz();
                graphPanel.update();

                readAtomFeatures();

                readBondFeatures();

                fm.update();
            });

        graphPanel.move(0, 100, 800, 750);

        graphPanel.events().draw(
            [&](PAINTSTRUCT &ps)
            {
                // fill graph panel with image produced by graphviz
                wex::window2file w2f;
                auto path = std::filesystem::temp_directory_path();
                auto sample = path / "sample.png";
                w2f.draw(graphPanel, sample.string());
            });

        lbNodefeatures.move(400, 100, 100, 30);
        lbNodefeatures.text("Atom Features");
        pnAtoms.move(400, 140, 300, 300);
        lbBondfeatures.move(400, 200, 100, 30);
        lbBondfeatures.text("Bond Features");
        pnBonds.move(400, 240, 300, 300);

        show();
        run();
    }

private:
    wex::editbox &ebsmiles;
    wex::button &bnsmiles;
    wex::panel &graphPanel;
    wex::label &lbNodefeatures;
    wex::panel &pnAtoms;
    wex::label &lbBondfeatures;
    wex::panel &pnBonds;
    cChemGraph myG;

    void readAtomFeatures();
    void readBondFeatures();
};

void cGUI::readAtomFeatures()
{
    std::ifstream t("atomfeatures.txt");
    std::stringstream buffer;
    buffer << t.rdbuf();
    myG.readNodeFeatures(buffer.str());
    pnBonds.text(buffer.str());
}
void cGUI::readBondFeatures()
{
    std::ifstream t("bondfeatures.txt");
    std::stringstream buffer;
    buffer << t.rdbuf();
    myG.readBondFeatures(buffer.str());
    pnBonds.text(buffer.str());
}

main()
{
    cGUI theGUI;
    return 0;
}
