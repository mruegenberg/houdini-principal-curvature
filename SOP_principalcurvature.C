#include "SOP_principalcurvature.h"

#include <GU/GU_Detail.h>
#include <GEO/GEO_PrimPoly.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include <PRM/PRM_Include.h>
#include <CH/CH_LocalVariable.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_Interrupt.h>
#include <SYS/SYS_Math.h>
#include <OP/OP_AutoLockInputs.h>
#include <limits.h>

// removes crashes at the cost of some speed
#define EIGEN_DONT_VECTORIZE 1
#define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT 1

#include <Eigen/Dense>
#include <igl/principal_curvature.h>

#include <cstdio>

using namespace OYW;

//
// Help is stored in a "wiki" style text file.  This text file should be copied
// to $HOUDINI_PATH/help/nodes/sop/star.txt
//
// See the sample_install.sh file for an example.
//

///
/// newSopOperator is the hook that Houdini grabs from this dll
/// and invokes to register the SOP.  In this case we add ourselves
/// to the specified operator table.
///
void
newSopOperator(OP_OperatorTable *table)
{
    table->addOperator(new OP_Operator(
        "hdk_principalcurvature",                 // Internal name
        "Principal Curvature Directions",                     // UI name
        SOP_Principalcurvature::myConstructor,    // How to build the SOP
        SOP_Principalcurvature::myTemplateList,   // My parameters
        1,                          // Min # of sources
        1,                          // Max # of sources
        0));        // Flag it as generator
}

PRM_Template
SOP_Principalcurvature::myTemplateList[] = {
    PRM_Template()
};

OP_Node *
SOP_Principalcurvature::myConstructor(OP_Network *net, const char *name, OP_Operator *op)
{
    return new SOP_Principalcurvature(net, name, op);
}

SOP_Principalcurvature::SOP_Principalcurvature(OP_Network *net, const char *name, OP_Operator *op)
    : SOP_Node(net, name, op)
{
    // this indicates that we manually indicate which attributes changed.
    // leaving this out is less efficient, but more robust.
    mySopFlags.setManagesDataIDs(true);
}

SOP_Principalcurvature::~SOP_Principalcurvature() { ; }

OP_ERROR
SOP_Principalcurvature::cookMySop(OP_Context &context)
{
    // We must lock our inputs before we try to access their geometry.
    // OP_AutoLockInputs will automatically unlock our inputs when we return.
    // NOTE: Don't call unlockInputs yourself when using this!
    OP_AutoLockInputs inputs(this);
    if (inputs.lock(context) >= UT_ERROR_ABORT)
        return error();

    duplicateSource(0, context);

    const GU_Detail *gdp1 = inputGeo(0);

    // translate Houdini geo to Eigen matrices
    int ptCount = gdp1->getNumPoints();
    int faceCount = gdp1->getNumPrimitives();
    Eigen::Matrix<double, Eigen::Dynamic, 3> V(ptCount,3); // point coords
    Eigen::Matrix<int, Eigen::Dynamic, 3> F(faceCount,3); // point indices

    {
        int r = 0;
        for (GA_Iterator it(gdp1->getPointRange(NULL)); !it.atEnd(); ++it) // NULL can be replaced by a group
        {
            UT_Vector3 p = gdp1->getPos3(*it);
            V.row(r) = Eigen::Vector3d(p.x(), p.y(), p.z());
            r++;
        }
        
        UT_Array< const GA_Primitive * >prims;
        gdp->getPrimitivesOfType(GA_PRIMPOLY, prims);
        
        r = 0;
        for(int i=0; i<prims.size(); ++i) {
            printf("set prim %d\n", i);
            const GA_Primitive *prim = prims(i);
            if(prim->getPointRange(NULL).getEntries() > 3) {
                addError(SOP_MESSAGE, "Mesh is not triangulated.");
                return error();
            }
            F.row(r) = Eigen::Vector3i(prim->getPointIndex(0), prim->getPointIndex(1), prim->getPointIndex(2));
            r++;
        }
    }

    // Eigen::MatrixXd PD1,PD2;
    Eigen::Matrix<double, Eigen::Dynamic, 3> PD1(ptCount,3);
    Eigen::Matrix<double, Eigen::Dynamic, 3> PD2(ptCount,3);
    // Eigen::VectorXd PV1,PV2;
    Eigen::Matrix<double, Eigen::Dynamic, 1> PV1(ptCount,1);
    Eigen::Matrix<double, Eigen::Dynamic, 1> PV2(ptCount,1);
    igl::principal_curvature(V,F,PD1,PD2,PV1,PV2);

    {
        UT_String                    aname = "maxcurvature";
        GA_WOAttributeRef            aoff;
        aoff = gdp->findFloatTuple(GA_ATTRIB_POINT, aname);
        if (!aoff.isValid()) // Not present, so create the detail attribute:
        {
            aoff = gdp->addFloatTuple(GA_ATTRIB_POINT, aname, 1);
        }
        GA_RWHandleF whandle(aoff.getAttribute());

        int i = 0;
		GA_Offset ptoff;
        GA_FOR_ALL_PTOFF(gdp1, ptoff)
    	{
            double pv1 = PV1(i);
    	    whandle.set(ptoff, pv1);
    	    i++;
    	}
    }

    {
        UT_String                    aname = "mincurvature";
        GA_WOAttributeRef            aoff;
        aoff = gdp->findFloatTuple(GA_ATTRIB_POINT, aname);
        if (!aoff.isValid()) // Not present, so create the detail attribute:
        {
            aoff = gdp->addFloatTuple(GA_ATTRIB_POINT, aname, 1);
        }
        GA_RWHandleF whandle(aoff.getAttribute());

        int i = 0;
		GA_Offset ptoff;
        GA_FOR_ALL_PTOFF(gdp1, ptoff)
        {
            double pv1 = PV2(i);
            whandle.set(ptoff, pv1);
            i++;
        }
    }

    {
        UT_String                    aname = "min_principalcurvature";
        GA_WOAttributeRef            aoff;
        aoff = gdp->findFloatTuple(GA_ATTRIB_POINT, aname);
        if (!aoff.isValid()) // Not present, so create the detail attribute:
        {
            aoff = gdp->addFloatTuple(GA_ATTRIB_POINT, aname, 3);
        }
        GA_RWHandleV3 whandle(aoff.getAttribute());
		
		GA_Offset ptoff;
        int i = 0;
        GA_FOR_ALL_PTOFF(gdp1, ptoff)
        {
            Eigen::Vector3d pv1 = PD1.row(i);
            whandle.set(ptoff, UT_Vector3(pv1.x(), pv1.y(), pv1.z()));
            i++;
        }
    }

    {
        UT_String                    aname = "max_principalcurvature";
        GA_WOAttributeRef            aoff;
        aoff = gdp->findFloatTuple(GA_ATTRIB_POINT, aname);
        if (!aoff.isValid()) // Not present, so create the detail attribute:
        {
            aoff = gdp->addFloatTuple(GA_ATTRIB_POINT, aname, 3);
        }
        GA_RWHandleV3 whandle(aoff.getAttribute());
		
		GA_Offset ptoff;
        int i = 0;
        GA_FOR_ALL_PTOFF(gdp1, ptoff)
        {
            Eigen::Vector3d pv2 = PD2.row(i);
            whandle.set(ptoff, UT_Vector3(pv2.x(), pv2.y(), pv2.z()));
            i++;
        }
    }

    return error();
}
