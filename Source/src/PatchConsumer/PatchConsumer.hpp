/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */ 


#ifndef PATCH_CONSUMER_HPP
#define PATCH_CONSUMER_HPP

#include "../Patch/Patch.hpp"

/*
 *  Inheriting from this class allows us to change
 *  what we do with the patches.
 *
 *  For normal use, we implement a BVWriter.
 *
 */
class PatchConsumer
{
public:
    virtual ~PatchConsumer() = default;
    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual void Consume(Patch a_Patch) = 0;
};


#endif // PATCH_CONSUMER_HPP
