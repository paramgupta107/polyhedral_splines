#ifndef PATCH_HPP
#define PATCH_HPP

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <iostream>

typedef OpenMesh::PolyMesh_ArrayKernelT<>::Point Point;

struct Patch
{
    Patch() {};
    Patch(int a_BiDeg) : m_DegU(a_BiDeg), m_DegV(a_BiDeg)
    {
        m_BBcoefs.resize(a_BiDeg+1);
        for(int row=0; row<a_BiDeg+1; ++row){
            m_BBcoefs[row].resize(a_BiDeg+1);
        }
        init_BB_coefs();
    };

    Patch(int a_BiDeg, std::string a_Group) : m_DegU(a_BiDeg), m_DegV(a_BiDeg), m_Group(a_Group)
    {
        m_BBcoefs.resize(a_BiDeg+1);
        for(int row=0; row<a_BiDeg+1; ++row){
            m_BBcoefs[row].resize(a_BiDeg+1);
        }
        init_BB_coefs();
    };

    Patch(int a_DegU, int a_DegV) : m_DegU(a_DegU), m_DegV(a_DegV)
    {
        m_BBcoefs.resize(a_DegU+1);
        for(int row=0; row<a_DegU+1; ++row){
            m_BBcoefs[row].resize(a_DegV+1);
        }
        init_BB_coefs();
    };

    Patch(int a_DegU, int a_DegV, std::string a_Group) : m_DegU(a_DegU), m_DegV(a_DegV), m_Group(a_Group)
    {
        m_BBcoefs.resize(a_DegU+1);
        for(int row=0; row<a_DegU+1; ++row){
            m_BBcoefs[row].resize(a_DegV+1);
        }
        init_BB_coefs();
    };

    void operator=(const Patch a_Other)
    {
        m_DegU = a_Other.m_DegU;
        m_DegV = a_Other.m_DegV;
        m_Group = a_Other.m_Group;
        m_BBcoefs = a_Other.m_BBcoefs;
    }

    bool isValid() const
    {
        int t_NumCpts = 0;
        for(size_t row=0; row<m_BBcoefs.size(); ++row){
            t_NumCpts += m_BBcoefs[row].size();
        }
        int t_OrderU = m_DegU+1;
        int t_OrderV = m_DegV+1;
        int t_ExpectedCpts = t_OrderU*t_OrderV;

        return (t_NumCpts == t_ExpectedCpts);
    }

    // Init each cpts as {0,0,0}
    void init_BB_coefs()
    {
        for(int i=0; i<m_BBcoefs.size(); i++)
        {
            for(int j=0; j<m_BBcoefs[0].size(); j++)
            {
                m_BBcoefs[i][j] = {0,0,0};
            }
        }
    }

    // Please find patch type in
    // https://www.cise.ufl.edu/research/SurfLab/bview/#file-format
    const std::string m_PatchType = "5";

    int m_DegU;
    int m_DegV;
    std::string m_Group = "Group 0 default";

    // m_BBcoefs should be a m_DegU+1 x m_DegV+1 2D array
    std::vector<std::vector<Point>> m_BBcoefs;
};


#endif //PATCH_HPP
