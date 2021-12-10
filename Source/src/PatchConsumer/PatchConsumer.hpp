/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */


#ifndef PATCHCONSUMER_HPP
#define PATCHCONSUMER_HPP

#include "../Patch/Patch.hpp"

/*
 *  Inheriting from this class allows us to change
 *  what we do with the patches. e.g. customize output format.
 *  Here we provide IGSWriter as another example.
 *
 *  We use BVWriter by default.
 */
class PatchConsumer
{
public:
    virtual ~PatchConsumer() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void consume(Patch a_Patch) = 0;
};


#endif // PATCHCONSUMER_HPP
