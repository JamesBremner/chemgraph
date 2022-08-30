#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <wex.h>
#include "cStarterGUI.h"
#include "cGraph.h"

class cChemGraph : public raven::graph::cGraph
{
public:
    void read(const std::string &sin);
    void readSMILES(const std::string &sin);
    std::string viz();
};

void cChemGraph::readSMILES(const std::string &sin)
{
    std::cout << sin << "\n";
    char token;
    int branch;   // index of atom where branch occurs
    int src = -1; // index of atom waiting for bond
    int ring = -1;
    bool closebracket = false;
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
                addLink(std::to_string(branch), std::to_string(idx - 1));
                closebracket = false;
            }
            else
            {
                addLink(std::to_string(src), std::to_string(idx - 1));
            }
            src = idx - 1;
            break;

        case '=':
            break;
        case '(':
            branch = idx - 1;
            break;
        case ')':
            closebracket = true;
            break;
        case '1':
            if (ring == -1)
            {
                ring = idx - 1;
                break;
            } else{
                addLink(std::to_string( ring),std::to_string( src ));
                ring = -1;
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
    std::stringstream sviz;
    sviz << "graph G {\n";
    for (auto n : nodes())
    {
        sviz << n.second.myName
             << " [label=\"" << n.second.myColor
             << "\"  penwidth = 3.0 ];\n";
    }
    for (auto &e : links())
    {
        if (e.first.first > e.first.second)
            continue;
        sviz << src(e).myName << "--"
             << dst(e).myName
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
            wex::msgbox mb(
                "Cannot find executable file\n"
                "Is graphViz installed?");

            return "";
        }
        wex::msgbox mb("system error");
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

class cGUI : public cStarterGUI
{
public:
    cGUI()
        : cStarterGUI(
              "Starter",
              {50, 50, 1000, 500}),
          lb(wex::maker::make<wex::label>(fm))
    {
        // myG.read("n 1 C n 2 O l 1 2");
        // myG.readSMILES("CNCC");
        //myG.readSMILES("CN(C)C");
        myG.readSMILES("C1CCCCC1");
        std::cout << myG.viz();

        lb.move(50, 50, 100, 30);
        lb.text("Hello World");

        show();
        run();
    }

private:
    wex::label &lb;
    cChemGraph myG;
};

main()
{
    cGUI theGUI;
    return 0;
}
