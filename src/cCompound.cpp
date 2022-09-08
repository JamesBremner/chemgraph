#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <vector>
#include "cCompound.h"

bool cCompound::read(std::string &line)
{
    int p = line.find("\"");
    while (p != -1)
    {
        int q = line.find("\"", p + 1);
        if (q == -1)
            return false;
        if (p < 20)
            if (!myName.length())
                myName = line.substr(p + 1, q - p - 1);
        line = line.substr(0, p) + line.substr(q + 1);
        p = line.find("\"");
    }
    std::string a;
    std::vector<std::string> cols;
    for (
        std::stringstream sst(line);
        getline(sst, a, ',');)
        cols.push_back(a);
    if (cols.size() < 14)
        return false;
    cid = atoi(cols[0].c_str());
    if (!myName.length())
        myName = cols[1];
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
        if (!C.read(line))
            continue;
        myCompound.push_back(C);
    }
}

int cCompound::EditDistance(
    const cCompound &other)
{
    const std::string a = mySMILES;
    const std::string b = other.mySMILES;
    std::vector<std::vector<int>> d(
        a.length() + 1,
        std::vector<int>(b.length() + 1));
    int cost, dmin, dtest;
    for (int i = 1; i <= a.length(); i++)
        d[i][0] = i;
    for (int i = 1; i <= b.length(); i++)
        d[0][i] = i;
    for (int j = 1; j <= b.length(); j++)
        for (int i = 1; i <= a.length(); i++)
        {
            if (a[i - 1] == b[j - 1])
                cost = 0;
            else
                cost = 1;
            dmin = d[i - 1][j] + 1;
            dtest = d[i][j - 1] + 1;
            if (dtest < dmin)
                dmin = dtest;
            dtest = d[i - 1][j - 1] + cost;
            if (dtest < dmin)
                dmin = dtest;
            d[i][j] = dmin;
        }
    return d[a.length()][b.length()];
}

std::string cCompound::text() const
{
    std::stringstream ss;
    ss << std::setw(20) << std::to_string(cid) + " " + mySMILES + " " + myName;
    return ss.str();
}

std::string cCompoundVector::closest(
    const std::string &SMILES)
{
    std::string ret = "CID       SMILES            Name\n";
    cCompound C;
    C.setSMILES(SMILES);
    for (auto &t : myCompound)
    {
        if (C.EditDistance(t) < 5)
        {
            // std::cout << t.id() << " "<< t.getSMILES() << ", ";
            ret += t.text() + "\n";
        }
    }
    return ret;
}
