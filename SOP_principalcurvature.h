#ifndef __SOP_xfomatcher_h__
#define __SOP_xfomatcher_h__

#include <SOP/SOP_Node.h>

namespace OYW {
class SOP_Principalcurvature : public SOP_Node
{
public:
	SOP_Principalcurvature(OP_Network *net, const char *name, OP_Operator *op);
    virtual ~SOP_Principalcurvature();

    static OP_Node *myConstructor(OP_Network*, const char *, OP_Operator *);

    static PRM_Template myTemplateList[];
protected:
    virtual OP_ERROR cookMySop(OP_Context &context);

    void ATTRIBNAME(UT_String &str, fpreal t) { evalString(str, 0, 0, t); }

    fpreal VALUE(fpreal t) { return evalFloat(1, 0, t); }
};
} // End OYW namespace

#endif
