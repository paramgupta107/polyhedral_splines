/* copyright(c)Jorg Peters [jorg.peters@gmail.com] */

#pragma once

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
        initBBcoefs();
    };

    Patch(int a_BiDeg, std::string a_Group) : m_DegU(a_BiDeg), m_DegV(a_BiDeg), m_Group(a_Group)
    {
        m_BBcoefs.resize(a_BiDeg+1);
        for(int row=0; row<a_BiDeg+1; ++row){
            m_BBcoefs[row].resize(a_BiDeg+1);
        }
        initBBcoefs();
    };

    Patch(int a_DegU, int a_DegV) : m_DegU(a_DegU), m_DegV(a_DegV)
    {
        m_BBcoefs.resize(a_DegU+1);
        for(int row=0; row<a_DegU+1; ++row){
            m_BBcoefs[row].resize(a_DegV+1);
        }
        initBBcoefs();
    };

    Patch(int a_DegU, int a_DegV, std::string a_Group) : m_DegU(a_DegU), m_DegV(a_DegV), m_Group(a_Group)
    {
        m_BBcoefs.resize(a_DegU+1);
        for(int row=0; row<a_DegU+1; ++row){
            m_BBcoefs[row].resize(a_DegV+1);
        }
        initBBcoefs();
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
    void initBBcoefs()
    {
        for(int i=0; i<m_BBcoefs.size(); i++)
        {
            for(int j=0; j<m_BBcoefs[0].size(); j++)
            {
                m_BBcoefs[i][j] = {0,0,0};
            }
        }
    }

    void degRaise()
    {
        bool t_IsRaiseU = m_DegU < 3 ? true : false;
        bool t_IsRaiseV = m_DegV < 3 ? true : false;

        // This patch doesn't need deg raise
        if(!t_IsRaiseU && !t_IsRaiseV)
        {
            return;
        }

        int t_UR = t_IsRaiseU ? m_DegU+1 : m_DegU;
        int t_VR = t_IsRaiseV ? m_DegV+1 : m_DegV;

        std::vector<std::vector<Point>> t_BBUR(t_UR+1, std::vector<Point>(m_DegV+1, {0,0,0})); // u-direction deg raised BB-coef

        // deg raise u direction
        if(t_IsRaiseU)
        {
            for(int i=0; i<=t_UR; i++)
            {
                int k = t_UR - i;
                for(int j=0; j<=m_DegV; j++)
                {
                    int a = (i-1 < 0) ? 0 : i-1;
                    int b = (i > m_DegU) ? i-1 : i;
                    t_BBUR[i][j] = (i*m_BBcoefs[a][j] + k*m_BBcoefs[b][j]) / t_UR;
                }
            }
            if(!t_IsRaiseV)
            {
                m_BBcoefs = t_BBUR;
            }
        }

        // deg raise v direction
        if(t_IsRaiseV && !t_IsRaiseU)
        {
            std::vector<std::vector<Point>> t_BBVR(m_DegU+1, std::vector<Point>(t_VR+1, {0,0,0})); // v-direction deg raised BB-coef
            for(int j=0; j<=t_VR; j++)
            {
                int k = t_VR - j;
                for(int i=0; i<=m_DegU; i++)
                {
                    int a = (j-1 < 0) ? 0 : j-1;
                    int b = (j > m_DegU) ? j-1 : j;
                    t_BBVR[i][j] = (j*m_BBcoefs[i][a] + k*m_BBcoefs[i][b]) / t_VR;
                }
            }
            m_BBcoefs = t_BBVR;
        }
        else  // deg raise u & v direction
        {
            std::vector<std::vector<Point>> t_BBR(t_UR+1, std::vector<Point>(t_VR+1, {0,0,0})); // both-direction deg raised BB-coef
            for(int j=0; j<=t_VR; j++)
            {
                int k = t_VR - j;
                for(int i=0; i<=t_UR; i++)
                {
                    int a = (j-1 < 0) ? 0 : j-1;
                    int b = (j > t_UR) ? j-1 : j;
                    t_BBR[i][j] = (j*t_BBUR[i][a] + k*t_BBUR[i][b]) / t_VR;
                }
            }
            m_BBcoefs = t_BBR;
        }

        // update member deg
        m_DegU = t_UR;
        m_DegV = t_VR;
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
