//-
// ==========================================================================
// Copyright 1995,2006,2008 Autodesk, Inc. All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk
// license agreement provided at the time of installation or download,
// or which otherwise accompanies this software in either electronic
// or hard copy form.
// ==========================================================================
//+

#include <maya/MTime.h>
#include <maya/MFnMesh.h>
#include <maya/MPoint.h>
#include <maya/MFloatPoint.h>
#include <maya/MGlobal.h>
#include <maya/MFloatPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MPxSelectionContext.h>
#include <maya/MPxContextCommand.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnPlugin.h>
#include <maya/MPxManipContainer.h>
#include <maya/MManipData.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MModelMessage.h>
#include <maya/MItSelectionList.h>

#include <maya/MPxNode.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MFnMeshData.h>

#include <maya/MIOStream.h>

MStatus returnStatus;

#define McheckErr(stat,msg)			\
	if ( MS::kSuccess != stat ) {	\
		cerr << msg;				\
		return MS::kFailure;		\
	}

class cageHandler : public MPxNode
{
public:
					cageHandler() {};
	virtual 		~cageHandler() {};
	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	// quand l'entrée change, recalcul.
	static  void*	creator();
	static  MStatus initialize();

	static MObject	cage;
	static MObject  inputMesh;
	static MObject	outputMesh;
	static MTypeId	id;

protected:
	MObject createMesh(const MTime& time, MObject& outData, MStatus& stat);
};

MObject cageHandler::cage;
MObject cageHandler::inputMesh;
MObject cageHandler::outputMesh;
MTypeId cageHandler::id( 0x80000 );

void* cageHandler::creator()
{
	return new cageHandler;
}

MStatus cageHandler::initialize()
{
	MFnUnitAttribute unitAttr;
	MFnTypedAttribute typedAttr;

	MStatus returnStatus;

	cageHandler::outputMesh = typedAttr.create( "outputMesh", "out",
												 MFnData::kMesh,
												 &returnStatus ); 
	McheckErr(returnStatus, "ERROR creating cageHandler output attribute\n");
	typedAttr.setStorable(false);

	cageHandler::inputMesh = typedAttr.create("inputMesh", "inM",
		MFnData::kMesh,
		&returnStatus);
	McheckErr(returnStatus, "ERROR creating cageHandler input attribute\n");
	typedAttr.setStorable(false);

	cageHandler::cage = typedAttr.create("cageMesh", "cage",
		MFnData::kMesh,
		&returnStatus);

	McheckErr(returnStatus, "ERROR creating cageHandler output attribute\n");
	typedAttr.setStorable(false);
	returnStatus = addAttribute(cageHandler::cage);
	McheckErr(returnStatus, "ERROR adding cage attribute\n");
	
	returnStatus = addAttribute(cageHandler::inputMesh);
	McheckErr(returnStatus, "ERROR adding inputMesh attribute\n");

	returnStatus = addAttribute(cageHandler::outputMesh);
	McheckErr(returnStatus, "ERROR adding outputMesh attribute\n");

	returnStatus = attributeAffects(cageHandler::inputMesh,
								    cageHandler::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");

	returnStatus = attributeAffects(cageHandler::cage,
		cageHandler::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");
	
	return MS::kSuccess;
}

MObject cageHandler::createMesh(const MTime& time,
							  MObject& outData,
							  MStatus& stat)

{
	int				numVertices, frame;
	float			cubeSize;
	MFloatPointArray		points;
	MFnMesh			meshFS;

	// Scale the cube on the frame number, wrap every 10 frames.
	frame = (int)time.as( MTime::kFilm );
	if (frame == 0)
	  frame = 1;
	cubeSize		    		= 0.5f * (float)( frame % 10);

	const int numFaces			= 6;
	numVertices					= 8;
	const int numFaceConnects	= 24;

	MFloatPoint vtx_1( -cubeSize, -cubeSize, -cubeSize );
	MFloatPoint vtx_2(  cubeSize, -cubeSize, -cubeSize );
	MFloatPoint vtx_3(  cubeSize, -cubeSize,  cubeSize );
	MFloatPoint vtx_4( -cubeSize, -cubeSize,  cubeSize );
	MFloatPoint vtx_5( -cubeSize,  cubeSize, -cubeSize );
	MFloatPoint vtx_6( -cubeSize,  cubeSize,  cubeSize );
	MFloatPoint vtx_7(  cubeSize,  cubeSize,  cubeSize );
	MFloatPoint vtx_8(  cubeSize,  cubeSize, -cubeSize );
	points.append( vtx_1 );
	points.append( vtx_2 );
	points.append( vtx_3 );
	points.append( vtx_4 );
	points.append( vtx_5 );
	points.append( vtx_6 );
	points.append( vtx_7 );
	points.append( vtx_8 );

	// Set up an array containing the number of vertices
	// for each of the 6 cube faces (4 verticies per face)
	//
	int face_counts[numFaces] = { 4, 4, 4, 4, 4, 4 };
	MIntArray faceCounts( face_counts, numFaces );

	// Set up and array to assign vertices from points to each face 
	//
	int face_connects[ numFaceConnects ] = {	0, 1, 2, 3,
												4, 5, 6, 7,
												3, 2, 6, 5,
												0, 3, 5, 4,
												0, 4, 7, 1,
												1, 7, 6, 2	};
	MIntArray faceConnects( face_connects, numFaceConnects );

	MObject newMesh = meshFS.create(numVertices, numFaces,
									points, faceCounts, faceConnects,
									outData, &stat);

	return newMesh;
}

MStatus cageHandler::compute(const MPlug& plug, MDataBlock& data)

{
	MStatus returnStatus;

	if (plug == outputMesh) {
		/* Get output object */

		MDataHandle outputHandle = data.outputValue(outputMesh, &returnStatus);
		McheckErr(returnStatus, "ERROR getting polygon data handle\n");

		MFnMeshData dataCreator;
		MObject newOutputData = dataCreator.create(&returnStatus);
		McheckErr(returnStatus, "ERROR creating outputData");


		outputHandle.set(newOutputData);
		data.setClean( plug );
	} else
		return MS::kUnknownParameter;

	return MS::kSuccess;
}

MStatus initializePlugin(MObject obj)
{
	MStatus   status;
	MFnPlugin plugin(obj, PLUGIN_COMPANY, "3.0", "Any");

	status = plugin.registerNode("cageHandler", cageHandler::id,
						 cageHandler::creator, cageHandler::initialize);
	if (!status) {
		status.perror("registerNode");
		return status;
	}

	return status;
}

MStatus uninitializePlugin(MObject obj)
{
	MStatus	  status;
	MFnPlugin plugin(obj);

	status = plugin.deregisterNode(cageHandler::id);
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}

	return status;
}
