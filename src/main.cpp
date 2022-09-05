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


class cGUI : public cStarterGUI
{
public:
    cGUI()
        : cStarterGUI(
              "ChemGraph",
              {50, 50, 1000, 500}),
          ebsmiles(wex::maker::make<wex::editbox>(fm)),
          bnsmiles(wex::maker::make<wex::button>(fm)),
          graphPanel(wex::maker::make<wex::panel>(fm))
    {
        // myG.read("n 1 C n 2 O l 1 2");
        // myG.readSMILES("CNCC");
        // myG.readSMILES("CN(C)C");
        // myG.readSMILES("C1CCCCC1");
        // std::cout << myG.viz();

        // myG.fragment(
        //     "C1=CC2=C(C=C1O)C(=CN2)CCN",
        //     "NC[CH2]:O:O:C"        );

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

        show();
        run();
    }

private:
    wex::editbox &ebsmiles;
    wex::button &bnsmiles;
    wex::panel &graphPanel;
    cChemGraph myG;
};

main()
{
    cGUI theGUI;
    return 0;
}
