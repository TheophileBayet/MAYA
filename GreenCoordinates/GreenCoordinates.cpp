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

#include <maya/MFnPlugin.h>
#include <maya/MPxTransform.h>
#include <maya/MPxTransformationMatrix.h>
#include <maya/MTransformationMatrix.h>

#include <maya/MTime.h>
#include <maya/MFnMesh.h>
#include <maya/MPoint.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatPointArray.h>
#include <maya/MPointArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MIntArray.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshFaceVertex.h>
#include <maya/MDoubleArray.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnPlugin.h>
#include <maya/MPxNode.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MFnMeshData.h>
#include <maya/MIOStream.h>

//#using namespace std

class Green : public MPxNode
{
public:
	Green() {};
	virtual         ~Green() {};
	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	static  void*    creator();
	static  MStatus initialize();
	static MObject    outputMesh;
	static MObject    inputMesh;
	static MObject    inputCage;
	static MTypeId    id;
protected:
	//void createMesh(MObject& outData, MStatus& stat);
	void calcWeights(MFloatPointArray inputMeshPoints, MItMeshPolygon Cagefaces, MFloatVector eta, MFloatArray &Phi, MFloatArray &Psi);
	//void recalcMesh(MObject& outData, MFloatPointArray inputMeshPoints, MFloatPointArray CagePoints, MFloatVectorArray CageNormals, MItMeshPolygon Cagefaces, MStatus& stat);
	double GCTriInt(MVector p, MVector v1, MVector v2, MFloatVector eta);
	//MObject createCage(MObject inputCage, MObject& outData, MStatus& stat);
	// Helpers
	/*MObject createQuads(const MFloatPointArray& points, MObject& outData, MStatus& stat);
	MObject createReverseQuads(const MFloatPointArray& points, MObject& outData, MStatus& stat);
	MObject createTris(const MFloatPointArray& points, MObject& outData, MStatus& stat);
	MObject createReverseTris(const MFloatPointArray& points, MObject& outData, MStatus& stat);*/
};

void* Green::creator()
{
	return new Green;
}
MStatus Green::initialize()
{
	MFnUnitAttribute unitAttr;
	MFnTypedAttribute typedAttr;
	MStatus returnStatus;
	Green::outputMesh = typedAttr.create("outputMesh", "out",
		MFnData::kMesh,
		&returnStatus);
	//McheckErr(returnStatus, "ERROR creating Green output attribute\n");
	//typedAttr.setStorable(false);
	Green::inputMesh = typedAttr.create("inputMesh", "inM",
		MFnData::kMesh,
		&returnStatus);
	//McheckErr(returnStatus, "ERROR creating Green output attribute\n");
	//typedAttr.setStorable(false);
	Green::inputCage = typedAttr.create("inputCage", "inC",
		MFnData::kMesh,
		&returnStatus);
	//McheckErr(returnStatus, "ERROR creating Green output attribute\n");
	//typedAttr.setStorable(false);
	returnStatus = addAttribute(Green::inputMesh);
	//McheckErr(returnStatus, "ERROR adding inputMesh attribute\n");
	returnStatus = addAttribute(Green::inputCage);
	//McheckErr(returnStatus, "ERROR adding inputCage attribute\n");
	returnStatus = addAttribute(Green::outputMesh);
	//McheckErr(returnStatus, "ERROR adding outputMesh attribute\n");
	attributeAffects(inputCage, outputMesh);
	attributeAffects(inputMesh, outputMesh);
	return MS::kSuccess;
}

MStatus initializePlugin(MObject obj)
{
	MStatus   status;
	MFnPlugin plugin(obj, PLUGIN_COMPANY, "4.0", "Any");
	status = plugin.registerNode("Green", Green::id,
		Green::creator, Green::initialize);
	if (!status) {
		status.perror("registerNode");
		return status;
	}
	return status;
}
MStatus uninitializePlugin(MObject obj)
{
	MStatus      status;
	MFnPlugin plugin(obj);
	status = plugin.deregisterNode(Green::id);
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}
	return status;
}

double Green::GCTriInt(MVector p, MVector v1, MVector v2, MFloatVector eta){
	MFloatVector first = v2 - v1;
	MFloatVector second = p - v1;
	MFloatVector third = v2 - p;
	MFloatVector fourth = v1 - p;
	MFloatVector fifth = p - eta;
	double alpha = (first*second) / (first.length()*second.length());
	alpha = acos(alpha);
	double beta = (third*fourth) / (third.length()*fourth.length());
	double lambda = second.length()*second.length() * sin(alpha)*sin(alpha);
	double c = fifth.length()*fifth.length();
	double theta1 = M_PI - alpha;
	double theta2 = M_PI - alpha - beta;
	double S1 = sin(theta1);
	double S2 = sin(theta2);
	double C1 = cos(theta1);
	double C2 = cos(theta2);
	double I1 = 2 * sqrt(c) * atan((sqrt(c)*C1)/(sqrt(lambda+S1*S1*c)));
	I1 += sqrt(lambda) * log( ((2*sqrt(lambda)*S1*S1)/((1-C1)*(1-C1))) * (1 - ((2*c*C1) / (c*(1+C1)+lambda+sqrt(lambda*lambda+lambda*c*S1*S1)))) );
	I1 = I1 * (-S1 / (2 * abs(S1)));
	double I2 = 2 * sqrt(c) * atan((sqrt(c)*C2) / (sqrt(lambda + S2*S2*c)));
	I2 += sqrt(lambda) * log(((2 * sqrt(lambda)*S2*S2) / ((1 - C2)*(1 - C2))) * (1 - ((2 * c*C2) / (c*(1 + C2) + lambda + sqrt(lambda*lambda + lambda*c*S2*S2)))));
	I2 = I2 * (-S2 / (2 * abs(S2)));
	float res = I1 - I2 - sqrt(c)*beta;
	res = abs(res);
	res = res * (-1 / (4 * M_PI));
	return res;
}

void Green::calcWeights(MFloatPointArray inputMeshPoints, MItMeshPolygon Cagefaces, MFloatVector eta, MFloatArray &Phi, MFloatArray &Psi){
	// Initialization
	Phi.clear();
	Psi.clear();
	// Loop over the faces
	MPointArray listPoint;
	for (Cagefaces.reset(); Cagefaces.isDone() != true; Cagefaces.next()){
		MFloatVectorArray vect = MFloatVectorArray();
		Cagefaces.getPoints(listPoint);
		for (int i = 0; i < 3; i++){
			listPoint[i] = listPoint[i] - MPoint(eta);
		}
		MVector n;
		Cagefaces.getNormal(n);
		MVector p = (MVector(listPoint[0])* n) * n;
		MFloatArray s = MFloatArray();
		MFloatArray first = MFloatArray();
		MFloatArray second = MFloatArray();
		MFloatVectorArray q = MFloatVectorArray();
		MFloatVectorArray norm = MFloatVectorArray();
		MFloatVector zero = MFloatVector(0, 0);
		for (int i = 0; i < 3; i++){
			s[i] = (MVector(listPoint[i] - p) ^ MVector(listPoint[i + 1] - p))* n;
			s[i] = s[i] / abs(s[i]);
			first = GCTriInt(p, listPoint[i], listPoint[i + 1], zero);
			second = GCTriInt(zero, listPoint[i], listPoint[i + 1], zero);
			q[i] = MVector(listPoint[i + 1]) ^ MVector(listPoint[i]);
			norm[i] = q[i] / q[i].length();
		}
		double I = 0;
		for (int i = 0; i < 3; i++){
			I += s[i] * first[i];
		}
		I = -abs(I);
		Psi[Cagefaces.index()] = -I;
		MFloatVector w = n*I;
		for (int i = 0; i < 3; i++){
			w += norm[i] * second[i];
		}
		for (int i = 0; i < 3; i++){
			double tmp = (norm[i + 1]* w) / (norm[i + 1]* listPoint[i]);
		}
	}
}
