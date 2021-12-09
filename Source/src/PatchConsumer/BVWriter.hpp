/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#ifndef BVWRITER_HPP
#define BVWRITER_HPP

#include <string>
#include <mutex>
#include <fstream>
#include <queue>

#include "PatchConsumer.hpp"
#include "../Patch/Patch.hpp"

class BVWriter : public PatchConsumer
{
public:
    BVWriter(const std::string a_OutFile);
    ~BVWriter();
    void Start();
    void Stop();
    void Consume(const Patch a_Patch);
private:
    std::ofstream m_OutFile;
    void WritePatch(const Patch a_Patch);
};

#endif // BVWRITER_HPP
