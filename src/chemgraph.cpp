#include <filesystem>
#include <fstream>
#include "chemgraph.h"
void cChemGraph::readSMILES(const std::string &sin)
{
    myG.clear();
    char token;
    int branch;   // index of atom where branch occurs
    int src = -1; // index of atom waiting for bond
    std::vector<int> ring(10,-1);
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
            findNode(findoradd(std::to_string(idx))).myColor = token;
            idx++;
            if (src == -1)
            {
                // root atom
                src = idx - 1;
                break;
            }
            if (closebracket)
            {
                addLink(
                    std::to_string(branch),
                     std::to_string(idx - 1),
                     bond );
                closebracket = false;
            }
            else
            {
                addLink(
                    std::to_string(src),
                     std::to_string(idx - 1),
                     bond );
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
        case '1': case '2':
            if( square )
                break;
            ringID = (int)token - 48;
            if (ring[ringID] == -1)
            {
                ring[ringID] = idx - 1;
            }
            else
            {
                addLink(std::to_string(ring[ringID]), std::to_string(src));
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
            findNode(findoradd(nid)).myColor = satom;
            break;

        case 'l':
            iss >> nid;
            iss >> nid2;
            addLink(nid, nid2);
            break;
        }
        lid = 'e';
    }
}
std::string cChemGraph::viz()
{
    std::map<std::string,std::string> mpColor;
    mpColor.insert( {"C","grey"} );
    mpColor.insert( {"O","red"} );
    mpColor.insert( {"N","green"} );

    std::stringstream sviz;
    sviz << "graph G {\n";
    for (auto n : nodes())
    {   
        sviz << n.second.myName
             << " [label=\"" << n.second.myColor
             << "\" color = \"" << mpColor[ n.second.myColor ]
             << "\"  penwidth = 3.0 ];\n";
    }
    for (auto &e : links())
    {
        if (e.first.first > e.first.second)
            continue;
        std::string color = "";
        if( e.second.myCost == 2 )
            color = " [penwidth = 3.0]";
        sviz << src(e).myName << "--"
             << dst(e).myName
             << color
             << "\n";
    }
    sviz << "}\n";

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
            std::cout <<
                "Cannot find executable file\n"
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

std::map<std::string,int> cChemGraph::countAtoms() 
{
    std::map<std::string,int> mpCount;
    for (const auto& n : nodes())
    {
        mpCount[ n.second.myColor ]++;
    }
    return mpCount;
}
