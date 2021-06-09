#ifndef BVWRITERSERIAL_HPP
#define BVWRITERSERIAL_HPP

#include <string>
#include <mutex>
#include <fstream>
#include <queue>

#include "../../PatchConsumer.hpp"
#include "../../../Patch/Patch.hpp"

class BVWriterSerial : public PatchConsumer
{
public:
    BVWriterSerial(const std::string a_OutFile);
    ~BVWriterSerial();
    void Start();
    void Stop();
    void Consume(const Patch a_Patch);
private:
    std::ofstream m_OutFile;
    void WritePatch(const Patch a_Patch);
};

#endif // BVWRITERSERIAL_HPP
