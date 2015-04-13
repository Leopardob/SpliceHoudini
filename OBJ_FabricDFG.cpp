#include "OBJ_FabricDFG.h"

#include <PRM/PRM_Include.h>
#include <CH/CH_LocalVariable.h>

#include <UT/UT_DMatrix3.h>

#include <ImathVec.h>

#define FEC_PROVIDE_STL_BINDINGS

using namespace OpenSpliceHoudini;
using std::cout;
using std::endl;
using std::boolalpha;

OP_Node* OBJ_FabricDFG::myConstructor(OP_Network* net, const char* name, OP_Operator* op)
{
    return new OBJ_FabricDFG(net, name, op);
}

OBJ_FabricDFG::OBJ_FabricDFG(OP_Network* net, const char* name, OP_Operator* op)
    : FabricDFGOP<OBJ_Geometry>(net, name, op)
{
}

OBJ_FabricDFG::~OBJ_FabricDFG()
{
}

// this function copies the original parameter and then adds an invisible flag
static void copyParmWithInvisible(PRM_Template& src, PRM_Template& dest)
{
    PRM_Name* new_name;

    new_name = new PRM_Name(src.getToken(), src.getLabel(), src.getExpressionFlag());
    new_name->harden();
    dest.initialize((PRM_Type)(src.getType() | PRM_TYPE_INVISIBLE),
                    src.getTypeExtended(),
                    src.exportLevel(),
                    src.getVectorSize(),
                    new_name,
                    src.getFactoryDefaults(),
                    src.getChoiceListPtr(),
                    src.getRangePtr(),
                    src.getCallback(),
                    src.getSparePtr(),
                    src.getParmGroup(),
                    (const char*)src.getHelpText(),
                    src.getConditionalBasePtr());
}

OP_TemplatePair* OBJ_FabricDFG::buildTemplatePair(OP_TemplatePair* prevstuff)
{

    // The parm templates here are not created as a static list because
    // if that static list was built before the OBJbaseTemplate static list
    // (which it references) then that list would be corrupt. Thus we have
    // to force our static list to be created after OBJbaseTemplate.
    static PRM_Template* theTemplate = 0;

    if (!theTemplate)
    {
        PRM_Template* obj_template;
        int i;
        int size;
        UT_String parm_name;

        obj_template = OBJ_Geometry::getTemplateList(OBJ_PARMS_PLAIN);
        size = PRM_Template::countTemplates(obj_template);
        theTemplate = new PRM_Template[size + 1]; // add +1 for sentinel
        for (i = 0; i < size; i++)
        {
            theTemplate[i] = obj_template[i];
            theTemplate[i].getToken(parm_name);

            // leave only the translation parameter visible (and its containing
            // switcher)
            if (parm_name != "t" && parm_name != "stdswitcher")
                copyParmWithInvisible(obj_template[i], theTemplate[i]);
        }
    }

    // Here, we have to "inherit" template pairs from geometry and beyond. To
    // do this, we first need to instantiate our template list, then add the
    // base class templates.
    OP_TemplatePair* dfg, *geo;
    dfg = new OP_TemplatePair(myTemplateList, prevstuff);
    geo = new OP_TemplatePair(theTemplate, dfg);

    return geo;
}

int OBJ_FabricDFG::applyInputIndependentTransform(OP_Context& context, UT_DMatrix4& mat)
{
    // try
    // {
    //     // call OBJ_Geometry::applyInputIndependentTransform() so that we don't
    //     // lose any information
    //     int modified = OBJ_Geometry::applyInputIndependentTransform(context, mat);

    //     loadGraph();
    //     fpreal now = context.getTime();
    //     setMultiParameterInputPorts(now);
    //     executeGraph();

    //     FabricServices::DFGWrapper::PortPtr port = getView().getGraph()->getPort("t");
    //     if (port->isValid() && port->getDataType() == "Vec3")
    //     {
    //         float t[3];
    //         std::vector<FabricCore::RTVal> args(2);
    //         args[0] = FabricCore::RTVal::ConstructExternalArray(*(getView().getClient()), "Float32", 3, &t);
    //         args[1] = FabricCore::RTVal::ConstructUInt32(*(getView().getClient()), 0 /* offset */);

    //         port->getRTVal().callMethod("", "get", 2, &args[0]);
    //         mat.pretranslate(t[0], t[1], t[2]);
    //     }

    //     port = getView().getExecutable().getPort("r");
    //     if (port->isValid() && port->getDataType() == "Vec3")
    //     {
    //         float r[3];
    //         std::vector<FabricCore::RTVal> args(2);
    //         args[0] = FabricCore::RTVal::ConstructExternalArray(*(getView().getClient()), "Float32", 3, &r);
    //         args[1] = FabricCore::RTVal::ConstructUInt32(*(getView().getClient()), 0 /* offset */);

    //         port->getRTVal().callMethod("", "get", 2, &args[0]);
    //         mat.prerotate(r[0], r[1], r[2], UT_XformOrder());
    //     }

    //     port = getView().getExecutable().getPort("s");
    //     if (port->isValid() && port->getDataType() == "Vec3")
    //     {
    //         float s[3];
    //         std::vector<FabricCore::RTVal> args(2);
    //         args[0] = FabricCore::RTVal::ConstructExternalArray(*(getView().getClient()), "Float32", 3, &s);
    //         args[1] = FabricCore::RTVal::ConstructUInt32(*(getView().getClient()), 0 /* offset */);

    //         port->getRTVal().callMethod("", "get", 2, &args[0]);
    //         mat.prescale(s[0], s[1], s[2]);
    //     }

    //     flags().setTimeDep(true);

    //     // return 1 to indicate that we have modified the input matrix.
    //     // if we didn't modify mat, then we should return 0 instead.
    //     return 1;
    // }
    // catch (FabricCore::Exception e)
    // {
    //     printf("FabricCore::Exception from OBJ_FabricDFG::applyInputIndependentTransform:\n %s\n", e.getDesc_cstr());
    //     return 0;
    // }
}

// OP_ERROR OBJ_FabricDFG::cookMyObj(OP_Context& context)
// {
//     // OP_ERROR errorstatus;
//     // OBJ_Geometry::cookMyObj computes the local and global transform, and
//     // dirties the inverse of the global transform matrix. These are stored
//     // in myXform, myWorldXform, and myIWorldXform, respectively.
//     // errorstatus = OBJ_Geometry::cookMyObj(context);

//     myXform.identity();
//     myXform.scale(1.0, 1.0, 1.0);
//     myXform.rotate(0.0, 3.14/4.0, 0.0, UT_XformOrder());
//     myXform.translate(0.0, 0.0, 0.0);
//     myWorldXform = myXform;

//     // inverseDirty();

//     // loadGraph();

//     // fpreal now = context.getTime();

//     // setMultiParameterInputPorts(now);
//     // getView().getBinding()->execute();

//     // const FabricDFGView::OutputPortsNames& outPortsMat44Names = getView().getOutputPortsMat44Names();
//     // if (outPortsMat44Names.size() > 0)
//     // {
//     //     FabricCore::RTVal rtMat44 = getView().getMat44RTVal(outPortsMat44Names[0].c_str());
//     //     if (rtMat44.isValid())
//     //     {
//     //         std::vector<float> comp(16);
//     //         std::vector<FabricCore::RTVal> args(2);
//     //         args[0] =
//     //             FabricCore::RTVal::ConstructExternalArray(*(getView().getClient()), "Float32", comp.size(),
//     &comp[0]);
//     //         args[1] = FabricCore::RTVal::ConstructUInt32(*(getView().getClient()), 0);

//     //         try
//     //         {
//     //             rtMat44.callMethod("", "get", 2, &args[0]);

//     //             UT_DMatrix4 dfgXfo(comp[0],
//     //                                comp[1],
//     //                                comp[2],
//     //                                comp[3],
//     //                                comp[4],
//     //                                comp[5],
//     //                                comp[6],
//     //                                comp[7],
//     //                                comp[8],
//     //                                comp[9],
//     //                                comp[10],
//     //                                comp[11],
//     //                                comp[12],
//     //                                comp[13],
//     //                                comp[14],
//     //                                comp[15]);

//     //             myXform = dfgXfo;
//     //             myWorldXform = dfgXfo;
//     //         }
//     //         catch (FabricCore::Exception e)
//     //         {
//     //             printf("Error: %s\n", e.getDesc_cstr());
//     //             return error();
//     //         }
//     //     }
//     // }

//     return error();
// }
