#include <windows.h>
#include <filesystem>
#include <iostream>
#include <fstream>

#include "chemgraph.h"
#include "GraphTheory.h"

void cChemGraph::readSMILES(const std::string &sin)
{
    clear();
    char token;
    int branch;   // index of atom where branch occurs
    int src = -1; // index of atom waiting for bond
    std::vector<int> ring(10, -1);
    int ringID;
    int bond = 1;
    bool closebracket = false;
    bool square = false;
    int idx = 0;
    for (int p = 0; p < sin.length(); p++)
    {
        token = sin[p];
        std::cout << token << " " << branch << " " << src << "\n";
        switch (token)
        {
        case 'C':
        case 'N':
        case 'O':
            wVertexAttr(
                add(std::to_string(idx)),
                {std::string(1, token)});
            idx++;
            if (src == -1)
            {
                // root atom
                src = idx - 1;
                break;
            }
            if (closebracket)
            {
                int ie = add(
                    std::to_string(branch),
                    std::to_string(idx - 1));
                wEdgeAttr(ie, {std::to_string(bond)});
                closebracket = false;
            }
            else
            {
                int ie = add(
                    std::to_string(src),
                    std::to_string(idx - 1));
                wEdgeAttr(ie, {std::to_string(bond)});
            }
            bond = 1;
            src = idx - 1;
            break;

        case '=':
            bond = 2;
            break;
        case 'H':
            // Hydrogen is ignored
            break;
        case '[':
            square = true;
            break;
        case ']':
            square = false;
            break;
        case '(':
            branch = idx - 1;
            break;
        case ')':
            closebracket = true;
            break;
        case '1':
        case '2':
            if (square)
                break;
            ringID = (int)token - 48;
            if (ring[ringID] == -1)
            {
                ring[ringID] = idx - 1;
            }
            else
            {
                add(std::to_string(ring[ringID]), std::to_string(src));
                ring[ringID] = -1;
            }
            break;
        }
    }
}
void cChemGraph::read(const std::string &sin)
{
    std::istringstream iss(sin);
    char lid;
    std::string nid, nid2;
    std::string satom;
    while (true)
    {
        iss >> lid;
        if (lid == 'e')
            break;
        switch (lid)
        {
        case 'n':
            iss >> nid;
            iss >> satom;
            // findNode(findoradd(nid)).myColor = satom;

            break;

        case 'l':
            iss >> nid;
            iss >> nid2;
            add(nid, nid2);
            break;
        }
        lid = 'e';
    }
}
std::string cChemGraph::viz()
{
    std::map<std::string, std::string> mpColor;
    mpColor.insert({"C", "grey"});
    mpColor.insert({"O", "red"});
    mpColor.insert({"N", "green"});

    std::stringstream sviz;
    sviz << "graph G {\n";
    for (int ni = 0; ni < vertexCount(); ni++)
    {
        auto atom = rVertexAttr(ni, 0);
        sviz << ni
             << " [label=\"" << atom
             << "\" color = \"" << mpColor[atom]
             << "\"  penwidth = 3.0 ];\n";
    }
    for (auto &e : edgeList())
    {
        std::string color = "";
        if (rEdgeAttr(find(e.first, e.second), 0) == "2")
            color = " [penwidth = 3.0]";
        sviz << e.first << "--"
             << e.second
             << color
             << "\n";
    }
    sviz << "}\n";

    std::cout << sviz.str() << "\n";

    auto path = std::filesystem::temp_directory_path();
    std::cout << "RunDOT " << path << "\n";
    auto gdot = path / "g.dot";
    std::ofstream f(gdot);
    if (!f.is_open())
        throw std::runtime_error("Cannot open " + gdot.string());
    f << sviz.str();

    f.close();

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    auto sample = path / "sample.png";
    std::string scmd = "dot -Kfdp -n -Tpng -Tdot -o " + sample.string() + " " + gdot.string();

    // std::cout << scmd << "\n";

    // Retain keyboard focus, minimize module2 window
    si.wShowWindow = SW_SHOWNOACTIVATE | SW_MINIMIZE;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USEPOSITION;
    si.dwX = 600;
    si.dwY = 200;

    if (!CreateProcess(NULL,                 // No module name (use command line)
                       (char *)scmd.c_str(), // Command line
                       NULL,                 // Process handle not inheritable
                       NULL,                 // Thread handle not inheritable
                       FALSE,                // Set handle inheritance to FALSE
                       CREATE_NEW_CONSOLE,   //  creation flags
                       NULL,                 // Use parent's environment block
                       NULL,                 // Use parent's starting directory
                       &si,                  // Pointer to STARTUPINFO structure
                       &pi)                  // Pointer to PROCESS_INFORMATION structure
    )
    {
        int syserrno = GetLastError();
        if (syserrno == 2)
        {
            std::cout << "Cannot find executable file\n"
                         "Is graphViz installed?\n";

            return "";
        }
        std::cout << "system error\n";
        // SetStatusText(wxString::Format("Sysem error no (%d)\n", GetLastError()));
        // wchar_t *lpMsgBuf;
        // FormatMessage(
        //     FORMAT_MESSAGE_ALLOCATE_BUFFER |
        //         FORMAT_MESSAGE_FROM_SYSTEM |
        //         FORMAT_MESSAGE_IGNORE_INSERTS,
        //     NULL,
        //     (DWORD)syserrno,
        //     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        //     (LPWSTR)&lpMsgBuf,
        //     0, NULL);

        // char errorbuf[200];
        // snprintf(errorbuf, 199,
        //          "Error is %S",
        //          lpMsgBuf);
        // LocalFree(lpMsgBuf);

        return "";
    }

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    Sleep(1000);

    return sviz.str();
}

std::map<std::string, int> cChemGraph::countAtoms()
{
    std::map<std::string, int> mpCount;
    for (int vi = 0; vi < vertexCount(); vi++)
    {
        mpCount[rVertexAttr(vi, 0)]++;
    }
    return mpCount;
}

void cChemGraph::readNodeFeatures(const std::string &sin)
{
    // each line is one feature value for every node
    // std::string line;
    // std::istringstream ssin( sin );
    // while( getline(ssin, line))
    // {
    //     std::vector<int> vf;
    //     std::istringstream sline( line );
    //     for( int kn = 0; kn < nodeCount(); kn++ ) {
    //         int v;
    //         sline >> v;
    //         vf.push_back( v );

    //     }
    //     myNodeFeatures.push_back( vf );
    // }
}
void cChemGraph::readBondFeatures(const std::string &sin)
{
}

std::string cChemGraph::graphtoSMILES(const raven::graph::cGraph &ingraph)
{
    std::string SMILES;              // the SMILES representation of the chemical graph
    std::vector<std::string> vLabel; // the label locations

    raven::graph::cGraph g = ingraph;

    // remove bonds to Hydrogen
    for (auto &bond : g.edgeList())
    {
        if (g.userName(bond.first)[0] == 'H' || g.userName(bond.second)[0] == 'H')
            g.remove(bond.first, bond.second);
    }

    // check for cycles
    auto vCycle = dfs_cycle_finder(g);
    for (int k = 0; k < vCycle.size(); k++)
    {
        g.remove(vCycle[k][0], vCycle[k][1]);
        g.wVertexAttr(vCycle[k][0], {std::to_string(k + 1)});
        g.wVertexAttr(vCycle[k][1], {std::to_string(k + 1)});
    }

    // depth first search of graph
    int prev = -1;
    dfs(g, 0,
        [&](int v)
        {
            // check for double bond
            if (prev != -1)
            {
                if (g.rEdgeAttr(g.find(prev, v), 0) == "2")
                {
                    SMILES += "=";
                    vLabel.push_back("-");
                }
            }

            // get the atom label
            auto label = g.userName(v);

            switch (label[0])
            {
            case 'C':
            case 'O':
            case 'N':
                vLabel.push_back(label);
                SMILES += label[0];
                break;
            default:
                vLabel.push_back("-");
                break;
            }

            // check for cycle break
            auto kc = g.rVertexAttr(v, 0);
            if (!kc.empty())
            {
                SMILES += kc;
                vLabel.push_back("-");
            }

            prev = v;

            return true;
        });

    std::cout << "SMILES: " << SMILES << "\n";
    std::cout << "\nlabel locations in SMILES string:\n";
    for (int k = 0; k < vLabel.size(); k++)
        std::cout << k << "\t" << vLabel[k] << "\n";

    return SMILES;
}
