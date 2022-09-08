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
#include "cCompound.h"

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

                // readAtomFeatures();

                // readBondFeatures();

                pnAtoms.text(vC.closest(ebsmiles.text()));

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
        lbNodefeatures.text("Cluster");
        pnAtoms.move(400, 140, 500, 600);
        pnAtoms.fontName("courier");
        // lbBondfeatures.move(400, 200, 100, 30);
        // lbBondfeatures.text("Bond Features");
        // pnBonds.move(400, 240, 300, 300);

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
    cCompoundVector vC;

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
