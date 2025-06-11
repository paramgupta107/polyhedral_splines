/* Based on code by Jorg Peters */

#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include "PatchConsumer.hpp"
#include "../Patch/Patch.hpp"

class STEPWriter : public PatchConsumer {
public:
    STEPWriter(const std::string a_OutFile);
    ~STEPWriter();
    void start();
    void stop();
    void consume(const Patch a_Patch);
private:
    FILE* m_OutFile;
    int m_CurrOffset;
    std::vector<int> openShellLocs;
    std::string m_FileName;
    int m_PatchNo;
    void writePatch(const Patch a_Patch);
};