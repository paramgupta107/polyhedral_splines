/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */


#ifndef PATCHCONSUMER_HPP
#define PATCHCONSUMER_HPP

#include "../Patch/Patch.hpp"

/*
 *  Inheriting from this class allows us to change
 *  what we do with the patches. e.g. customize output format.
 *  We use BVWriter by default and provide IGSWriter as another example.
 *
 *  Other usage e.g. derivative of BB-patch along u direction:
 *  User can also add following code into customized writer to get differentiated patch.
 *
 *  std::vector<std::vector<double>> get_du(const Patch& a_Patch)
 *  {
 *      std::vector<std::vector<double>> t_Du;
 *      for(int i=1; i<a_Patch.m_DegU+1; i++)
 *      {
 *          std::vector<double> t_DuRow;
 *          for(int j=0; j<a_Patch.m_DegV+1; j++)
 *          {
 *              t_DuRow.push_back(a_Patch.m_DegU * (a_Patch.m_BBcoefs[i][j] - a_Patch.m_BBcoefs[i-1][j]));
 *          }
 *          t_Du.push_back(t_DuRow);
 *      }
 *      return t_Du;
 *  }
 *
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
