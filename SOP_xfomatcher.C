#include "SOP_xfomatcher.h"

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
#include <igl/procrustes.h>

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
        "hdk_xfomatcher",                 // Internal name
        "Match Transform",                     // UI name
        SOP_Xfomatcher::myConstructor,    // How to build the SOP
        SOP_Xfomatcher::myTemplateList,   // My parameters
        2,                          // Min # of sources
        2,                          // Max # of sources
		0));        // Flag it as generator
}

PRM_Template
SOP_Xfomatcher::myTemplateList[] = {
    PRM_Template()
};

OP_Node *
SOP_Xfomatcher::myConstructor(OP_Network *net, const char *name, OP_Operator *op)
{
    return new SOP_Xfomatcher(net, name, op);
}

SOP_Xfomatcher::SOP_Xfomatcher(OP_Network *net, const char *name, OP_Operator *op)
    : SOP_Node(net, name, op)
{
    mySopFlags.setManagesDataIDs(true);
}

SOP_Xfomatcher::~SOP_Xfomatcher() {}

OP_ERROR
SOP_Xfomatcher::cookMySop(OP_Context &context)
{
    fpreal t = context.getTime();

    // We must lock our inputs before we try to access their geometry.
    // OP_AutoLockInputs will automatically unlock our inputs when we return.
    // NOTE: Don't call unlockInputs yourself when using this!
    OP_AutoLockInputs inputs(this);
    if (inputs.lock(context) >= UT_ERROR_ABORT)
        return error();
	
    duplicateSource(0, context);
	
	const GU_Detail *gdp1 = inputGeo(0);
    const GU_Detail *gdp2 = inputGeo(1);
	
	int pts1 = gdp1->getNumPoints();
    int pts2 = gdp1->getNumPoints();
    assert(pts1 == pts2);
    Eigen::Matrix<double, Eigen::Dynamic, 3> X(pts1,3);
    Eigen::Matrix<double, Eigen::Dynamic, 3> Y(pts2,3); // (containing 3d points as rows)
	
	int r = 0;
	for (GA_Iterator it(gdp1->getPointRange(NULL)); !it.atEnd(); ++it) // NULL can be replaced by a group
	{
        UT_Vector3 p = gdp1->getPos3(*it);
		X.row(r) = Eigen::Vector3d(p.x(), p.y(), p.z());
		r++;
    }
	
	r = 0;
	for (GA_Iterator it(gdp2->getPointRange(NULL)); !it.atEnd(); ++it) // NULL can be replaced by a group
	{
        UT_Vector3 p = gdp2->getPos3(*it);
		Y.row(r) = Eigen::Vector3d(p.x(), p.y(), p.z());
		r++;
    }
    
    double scale;
    Eigen::MatrixXd Rot;
    Eigen::Vector3d trans;
	
    igl::procrustes(X, Y, true, false, scale, Rot, trans);
	
	{
		UT_String                    aname = "scale";
		GA_WOAttributeRef            aoff;
		aoff = gdp->findFloatTuple(GA_ATTRIB_DETAIL, aname);
		if (!aoff.isValid()) // Not present, so create the detail attribute:
		{
			aoff = gdp->addFloatTuple(GA_ATTRIB_DETAIL, aname, 1);
		}
		GA_RWHandleF whandle(aoff.getAttribute());
		
		// Store the value in the detail attributes
		whandle.set(GA_Offset(0), scale);
	}
	
	{
		UT_String                    aname = "translation";
		GA_WOAttributeRef            aoff;
		aoff = gdp->findFloatTuple(GA_ATTRIB_DETAIL, aname);
		if (!aoff.isValid()) // Not present, so create the detail attribute:
		{
			aoff = gdp->addFloatTuple(GA_ATTRIB_DETAIL, aname, 3);
		}
		GA_RWHandleV3 whandle(aoff.getAttribute());
		whandle.set(GA_Offset(0), UT_Vector3(trans.x(),trans.y(),trans.z()) );
	}
	
	{
		UT_String                    aname = "rotation";
		GA_WOAttributeRef            aoff;
		aoff = gdp->findFloatTuple(GA_ATTRIB_DETAIL, aname);
		if (!aoff.isValid()) // Not present, so create the detail attribute:
		{
			aoff = gdp->addFloatTuple(GA_ATTRIB_DETAIL, aname, 9);
		}
		GA_RWHandleM3 whandle(aoff.getAttribute());
		
		UT_Matrix3F r_out;
		for(int i=0; i<3; ++i) {
			for(int j=0; j<3; ++j) {
				r_out[i][j] = Rot(i,j);
			}
		}
		whandle.set(GA_Offset(0), r_out);
	}

    return error();
}
