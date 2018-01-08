//-
// ==========================================================================
// Copyright 2015 Autodesk, Inc.  All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk
// license agreement provided at the time of installation or download,
// or which otherwise accompanies this software in either electronic
// or hard copy form.
// ==========================================================================
//+

//
//  This example plugin demonstrates how to implement a Maya File Translator.
//  In this example, the user can create one or more nurbsSpheres, nurbsCylinders or nurbsCones.
//  The user can translate them around.
//
//  The Off files can be referenced by Maya files.
//  
//  It is to be noted that this example was made to be simple.  Hence, there are limitations.
//  For example, every geometry saved will have its values reset to default,
//  except their translation if the option "Show Position" has been turned on.  To find what 
//  geometries we can export, we search by name, hence, if a polygon cube contains in its 
//  named the string "nurbsSphere", it will be written out as a nurbs sphere.
//
#include <maya/MPxNode.h>
#include <maya/MFloatPointArray.h>
#include <maya/MStatus.h>
#include <maya/MObject.h>
#include <maya/MFnPlugin.h>
#include <maya/MString.h>
#include <maya/MVector.h>
#include <maya/MStringArray.h>
#include <maya/MPxFileTranslator.h>
#include <maya/MGlobal.h>
#include <maya/MItDag.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MItSelectionList.h>
#include <maya/MSelectionList.h>
#include <maya/MIOStream.h>
#include <maya/MFStream.h>
#include <maya/MFileIO.h>
#include <maya/MFnTransform.h>
#include <maya/MNamespace.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <string.h>
#include <stdlib.h>
#include <maya/MTime.h>
#include <maya/MDataBlock.h>
#include <maya/MFnMeshData.h>
#include <maya/MIOStream.h>
#include <maya/MSimple.h>
#include <maya/MGlobal.h>
#include <maya/MFnMesh.h>



#define McheckErr(stat,msg)			\
	if ( MS::kSuccess != stat ) {	\
		cerr << msg;				\
		return MS::kFailure;		\
			}

//This is the backbone for creating a MPxFileTranslator
class OffTranslator : public MPxFileTranslator {
public:

	//Constructor
	OffTranslator() {};
	//Destructor
	virtual            ~OffTranslator() {};
	static MObject	time;
	static MObject	outputMesh;
	static  MStatus initialize();

	//This tells maya that the translator can read files.
	//Basically, you can import or load with your translator.
	bool haveReadMethod() const { return true; }

	//This tells maya that the translator can write files.
	//Basically, you can export or save with your translator.
	bool haveWriteMethod() const { return true; }

	//If this method returns true, and the Off file is referenced in a scene, the write method will be
	//called when a write operation is performed on the parent file.  This use is for users who wish
	//to implement a custom file referencing system.
	//For this example, we will return false as we will use Maya's file referencing system.
	bool haveReferenceMethod() const { return false; }

	//If this method returns true, it means we support namespaces.
	bool haveNamespaceSupport()    const { return true; }

	//This method is used by Maya to create instances of the translator.
	static void* creator();

	//This returns the default extension ".Off" in this case.
	MString defaultExtension() const;

	//If this method returns true it means that the translator can handle opening files 
	//as well as importing them.
	//If the method returns false then only imports are handled. The difference between 
	//an open and an import is that the scene is cleared(e.g. 'file -new') prior to an 
	//open, which may affect the behaviour of the translator.
	bool canBeOpened() const { return true; }

	//Maya will call this method to determine if our translator
	//is capable of handling this file.
	MFileKind identifyFile(const MFileObject& fileName,
		const char* buffer,
		short size) const;

	//This function is called by maya when import or open is called.
	MStatus reader(const MFileObject& file,
		const MString& optionsString,
		MPxFileTranslator::FileAccessMode mode);

	//This function is called by maya when export or save is called.
	MStatus writer(const MFileObject& file,
		const MString& optionsString,
		MPxFileTranslator::FileAccessMode mode);

private:
	//The magic string to verify it's a off file
	//simply "<Off>"
	static MString const magic;
	int num_vertices;
	int num_faces;
	int num_edges;
	MFloatPointArray points;
	MIntArray faceCounts;
	MIntArray	faceConnects;


};

MObject OffTranslator::time;
MObject OffTranslator::outputMesh;

//Creates one instance of the OffTranslator
void* OffTranslator::creator()
{
	return new OffTranslator();
}


// Initialize our magic string
MString const OffTranslator::magic("OFF");

// An Off file is an ascii whose first line contains the string <Off>.
// The read does not support comments, and assumes that the each
// subsequent line of the file contains a valid MEL command that can
// be executed via the "executeCommand" method of the MGlobal class.
//
MStatus OffTranslator::reader(const MFileObject& file,
	const MString& options,
	MPxFileTranslator::FileAccessMode mode)
{
	const MString fname = file.fullName();

	MStatus rval(MS::kSuccess);
	const int maxLineSize = 1024;
	char buf[maxLineSize];

	ifstream inputfile(fname.asChar(), ios::in);
	if (!inputfile) {
		// open failed
		cerr << fname << ": could not be opened for reading\n";
		return MS::kFailure;
	}

	if (!inputfile.getline(buf, maxLineSize)) {
		cerr << "file " << fname << " contained no lines ... aborting\n";
		return MS::kFailure;
	}

	if (0 != strncmp(buf, magic.asChar(), magic.length())) {
		cerr << "first line of file " << fname;
		cerr << " did not contain " << magic.asChar() << " ... aborting\n";
		return MS::kFailure;
	}

	inputfile.getline(buf, maxLineSize);
	MString numString;
	numString.set(buf);
	MStringArray numList;
	numString.split(' ', numList);

	// Affichage des résultats pour test 
	/*
	MGlobal::displayInfo("première ligne :" );
	MGlobal::displayInfo(numList[0]);
	MGlobal::displayInfo(numList[1]);
	MGlobal::displayInfo(numList[2]);
	MGlobal::displayInfo(numList[3]);
	*/

	num_vertices = numList[0].asInt();
	num_faces = numList[1].asInt();
	num_edges = numList[2].asInt();
	MString ligne;
	MStringArray liste;
	float x;
	float y;
	float z;
	int face1;
	int face2;
	int face3;

	MIntArray nbr_pts;
	MIntArray	face_connects;

	for (int i = 0; i < num_vertices; i++){
		// récupérer la ligne, créer le point correspondant
		if (!inputfile.getline(buf, maxLineSize)) {
			cerr << "file " << fname << " did not contain as many vertices as said \n";
			return MS::kFailure;
		}
		ligne.set(buf);
		liste.clear();
		ligne.split(' ', liste);
		x = liste[0].asFloat();
		y = liste[1].asFloat();
		z = liste[2].asFloat();
		// Créer le point correspondant : 
		points.append(MFloatPoint(x, y, z));
		//cerr << " point : " << x << " ; " << y << " : " << z << " \n";
	}

	for (int i = 0; i < num_faces; i++){
		// récupérer la ligne, créer la face correspondante
		if (!inputfile.getline(buf, maxLineSize)) {
			cerr << "file " << fname << " did not contain as many vertices as said \n";
			return MS::kFailure;
		}
		//inputfile.getline(buf, maxLineSize);
		ligne.set(buf);
		liste.clear();
		ligne.split(' ', liste);
		nbr_pts.append(liste[0].asInt());
		face1 = liste[1].asInt();
		face2 = liste[2].asInt();
		face3 = liste[3].asInt();
		face_connects.append(liste[1].asInt());
		face_connects.append(liste[2].asInt());
		face_connects.append(liste[3].asInt());
	}

	// TODO : corriger la ligne du dessus pour prendre en compte autre chose que des triangles.
	faceCounts = nbr_pts;
	faceConnects = face_connects;
	inputfile.close();
	MFnMesh			meshFS;
	/*cerr << " num vertices : " << num_vertices;
	cerr << " num faces : " << num_faces;
	cerr << " points : " << points;
	cerr << " faceCounts : " << faceCounts;
	cerr << " faceConnects : " << faceConnects ;*/
	meshFS.create(num_vertices, num_faces,
		points, faceCounts, faceConnects,
		MObject::kNullObj, NULL);
	points.clear();
	faceCounts.clear();
	faceConnects.clear();
	numList.clear();
	liste.clear();
	ligne.clear();
	return rval;
}



// The currently recognised primitives.
const char* primitiveStrings[] = {
	"nurbsSphere",
	"nurbsCone",
	"nurbsCylinder",
};
const unsigned numPrimitives = sizeof(primitiveStrings) / sizeof(char*);

// Corresponding commands to create the primitives
const char* primitiveCommands[] = {
	"sphere",
	"cone",
	"cylinder",
};

//The writer simply goes gathers all objects from the scene.
//We will check if the object has a transform, if so, we will check
//if it's either a nurbsSphere, nurbsCone or nurbsCylinder.  If so,
//we will write it out.
MStatus OffTranslator::writer(const MFileObject& file,
	const MString& options,
	MPxFileTranslator::FileAccessMode mode)
{
	MStatus status;
	bool showPositions = false;
	unsigned int  i;
	const MString fname = file.fullName();

	ofstream newf(fname.asChar(), ios::out);
	if (!newf) {
		// open failed
		cerr << fname << ": could not be opened for reading\n";
		return MS::kFailure;
	}
	newf.setf(ios::unitbuf);

	if (options.length() > 0) {
		// Start parsing.
		MStringArray optionList;
		MStringArray theOption;
		options.split(';', optionList);    // break out all the options.

		for (i = 0; i < optionList.length(); ++i){
			theOption.clear();
			optionList[i].split('=', theOption);
			if (theOption[0] == MString("showPositions") &&
				theOption.length() > 1) {
				if (theOption[1].asInt() > 0){
					showPositions = true;
				}
				else{
					showPositions = false;
				}
			}
		}
	}

	// output our magic number
	newf << "<OFF>\n";

	MItDag dagIterator(MItDag::kBreadthFirst, MFn::kInvalid, &status);

	if (!status) {
		status.perror("Failure in DAG iterator setup");
		return MS::kFailure;
	}

	MSelectionList selection;
	MGlobal::getActiveSelectionList(selection);
	MItSelectionList selIterator(selection, MFn::kDagNode);

	bool done = false;
	while (true)
	{
		MObject currentNode;
		switch (mode)
		{
		case MPxFileTranslator::kSaveAccessMode:
		case MPxFileTranslator::kExportAccessMode:
			if (dagIterator.isDone())
				done = true;
			else {
				currentNode = dagIterator.item();
				dagIterator.next();
			}
			break;
		case MPxFileTranslator::kExportActiveAccessMode:
			if (selIterator.isDone())
				done = true;
			else {
				selIterator.getDependNode(currentNode);
				selIterator.next();
			}
			break;
		default:
			cerr << "Unrecognized write mode: " << mode << endl;
			break;
		}
		if (done)
			break;

		//We only care about nodes that are transforms
		MFnTransform dagNode(currentNode, &status);
		if (status == MS::kSuccess)
		{
			MString nodeNameNoNamespace = MNamespace::stripNamespaceFromName(dagNode.name());
			for (i = 0; i < numPrimitives; ++i) {
				if (nodeNameNoNamespace.indexW(primitiveStrings[i]) >= 0){
					// This is a node we support
					newf << primitiveCommands[i] << " -n " << nodeNameNoNamespace << endl;
					if (showPositions) {
						MVector pos;
						pos = dagNode.getTranslation(MSpace::kObject);
						newf << "move " << pos.x << " " << pos.y << " " << pos.z << endl;
						// TODO : Changer ces instructions par la construction d'un maillage
					}
				}
			}
		}//if (status == MS::kSuccess)
	}//while loop

	newf.close();
	return MS::kSuccess;
}

// Whenever Maya needs to know the preferred extension of this file format,
// it calls this method. For example, if the user tries to save a file called
// "test" using the Save As dialog, Maya will call this method and actually
// save it as "test.Off". Note that the period should *not* be included in
// the extension.
MString OffTranslator::defaultExtension() const
{
	return "off";
}


//This method is pretty simple, maya will call this function
//to make sure it is really a file from our translator.
//To make sure, we have a little magic number and we verify against it.
MPxFileTranslator::MFileKind OffTranslator::identifyFile(
	const MFileObject& fileName,
	const char* buffer,
	short size) const
{
	// Check the buffer for the "Off" magic number, the
	// string "<Off>\n"

	MFileKind rval = kNotMyFileType;

	if ((size >= (short)magic.length()) &&
		(0 == strncmp(buffer, magic.asChar(), magic.length())))
	{
		rval = kIsMyFileType;
	}
	return rval;
}


MStatus OffTranslator::initialize(){

	MFnUnitAttribute unitAttr;
	MFnTypedAttribute typedAttr;

	MStatus returnStatus;
	/*
	OffTranslator::time = unitAttr.create("time", "tm", MFnUnitAttribute::kTime, 0.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating OffTranslator time attribute\n");


	OffTranslator::outputMesh = typedAttr.create("outputMesh", "out", MFnData::kMesh, &returnStatus);
	McheckErr(returnStatus, "ERROR creating OffTranslator output attribute\n");
	typedAttr.setStorable(false);
	*/
	/*
	returnStatus = addAttribute(OffTranslator::time);
	McheckErr(returnStatus, "ERROR adding time attribute\n");

	returnStatus = addAttribute(OffTranslator::outputMesh);
	McheckErr(returnStatus, "ERROR adding outputMesh attribute\n");

	returnStatus = attributeAffects(OffTranslator::time, OffTranslator::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects\n");
	*/
	return MS::kSuccess;

}


MStatus initializePlugin(MObject obj)
{
	MStatus   status;
	MFnPlugin plugin(obj, PLUGIN_COMPANY, "3.0", "Any");

	// Register the translator with the system
	// The last boolean in this method is very important.
	// It should be set to true if the reader method in the derived class
	// intends to issue MEL commands via the MGlobal::executeCommand 
	// method.  Setting this to true will slow down the creation of
	// new objects, but allows MEL commands other than those that are
	// part of the Maya Ascii file format to function correctly.
	status = plugin.registerFileTranslator("Off",
		"OffTranslator.rgb",
		OffTranslator::creator,
		"",
		"showPositions=1",
		true);
	if (!status)
	{
		status.perror("registerFileTranslator");
		return status;
	}


	return status;
}

MStatus uninitializePlugin(MObject obj)
{
	MStatus   status;
	MFnPlugin plugin(obj);

	status = plugin.deregisterFileTranslator("Off");
	if (!status)
	{
		status.perror("deregisterFileTranslator");
		return status;
	}

	return status;
}

