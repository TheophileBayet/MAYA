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

// Example Plugin: helloWorldCmd.cpp
//
// This plugin uses DeclareSimpleCommand macro
// to do the necessary initialization for a simple command
// plugin.
//
// Once this plugin is loaded by the Plugin Manager,
// it can be run from the Maya command line (i.e. script
// editor) like this:
//
//    helloWorld
//
// which will simply print the following message
// in the Output Window:
//
//    Hello World

#include <maya/MIOStream.h>
#include <maya/MString.h>
#include <maya/MSimple.h>
#include <maya/MGlobal.h>
#include <maya/MArgList.h>
#include <maya/MFStream.h>

// Use a Maya macro to setup a simple helloWorld class
// with methods for initialization, etc.
//
DeclareSimpleCommand( cage, PLUGIN_COMPANY, "4.5");

// All we need to do is supply the doIt method
// which in this case is only a Hello World example
//
MStatus cage::doIt( const MArgList& arg)
{
	MStatus ReturnStatus;
	if (arg.length(&ReturnStatus)!= 1){
		MGlobal::displayInfo("Veuillez rentrer un argument : le nom du fichier dont on veut générer la cage\n");
	}
	unsigned int index = 0;
	MString name;
	arg.get(index, name);
	if ((name == "tux_lowres") ||
		(name == "tux_lowres.off")){
		MGlobal::displayInfo("CHARGEMENT DE LA CAGE POUR LE TUX EN BASSE RESOLUTION \n");
		MGlobal::displayInfo("\n");
		MGlobal::displayInfo("\n");
		MGlobal::executeCommand("CreatePolygonCube;\n polyCube -w 1 -h 1 -d 1 -sx 1 -sy 1 -sz 1 -ax 0 1 0 -cuv 4 -ch 1; \n setAttr \"pCube1.scaleX\" 5;\n setAttr \"pCube1.scaleZ\" 4; ", true, true);
		MGlobal::executeCommand("PolyExtrude;\n select - r pCube1.f[3];\n polyExtrudeFacet - constructionHistory 1 - keepFacesTogether 1 - pvx 0 - pvy 0 - pvz 0 - divisions 1 - twist 0 - taper 1 - off 0 - thickness 0 - smoothingAngle 30 pCube1.f[3]; setAttr \"polyExtrudeFace2.localTranslate\" - type double3 0 0 8.40226; setAttr \"polyExtrudeFace2.divisions\" 4; ");
		MGlobal::executeCommand("select -r pCube1.f[20] ;\n PolyExtrude;\n select - r pCube1.f[20];\n polyExtrudeFacet - constructionHistory 1 - keepFacesTogether 1 - pvx - 2.5 - pvy - 5.751412392 - pvz 0 - divisions 1 - twist 0 - taper 1 - off 0 - thickness 0 - smoothingAngle 30 pCube1.f[20];setAttr \"polyExtrudeFace4.localTranslate\" - type double3 0 0 1.884227; ");
		MGlobal::executeCommand("select -r pCube1.f[12] ; \n PolyExtrude;\n select - r pCube1.f[12];\n polyExtrudeFacet - constructionHistory 1 - keepFacesTogether 1 - pvx 2.5 - pvy - 5.751412392 - pvz 0 - divisions 1 - twist 0 - taper 1 - off 0 - thickness 0 - smoothingAngle 30 pCube1.f[12];setAttr \"polyExtrudeFace6.localTranslate\" - type double3 0 0 2.231914;\n polyTriangulate pCube1; ");

	}
	if ((name == "tux") ||
		(name == "tux.off")){
		MGlobal::displayInfo("CHARGEMENT DE LA CAGE POUR LE TUX \n");
		MGlobal::displayInfo("\n");
		MGlobal::displayInfo("\n");
		MGlobal::executeCommand("CreatePolygonCube;polyCube - w 1 - h 1 - d 1 - sx 1 - sy 1 - sz 1 - ax 0 1 0 - cuv 4 - ch 1; \n move - r 0 0 0.423774; \n select - r pCube1;doDelete; \n setAttr \"polyCube2.width\" 0.5; \n setAttr \"polyCube2.height\" 0.4; \n setAttr \"polyCube2.subdivisionsDepth\" 5; ",true,true);
		MGlobal::executeCommand("select -r pCube2.f[20] ; \n PolyExtrude; \n select - r pCube2.f[20]; \n polyExtrudeFacet - constructionHistory 1 - keepFacesTogether 1 - pvx - 0.25 - pvy 0 - pvz 0.6237735827 - divisions 1 - twist 0 - taper 1 - off 0 - thickness 0 - smoothingAngle 30 pCube2.f[20];", true, true);
		MGlobal::executeCommand("setAttr \"polyExtrudeFace2.localTranslate\" -type double3 0 0 0.191184 ;", true, true);
		MGlobal::executeCommand("select -r pCube2.f[15] ; \n PolyExtrude; \n select - r pCube2.f[15]; \n polyExtrudeFacet - constructionHistory 1 - keepFacesTogether 1 - pvx 0.25 - pvy 0 - pvz 0.6237735827 - divisions 1 - twist 0 - taper 1 - off 0 - thickness 0 - smoothingAngle 30 pCube2.f[15];", true, true);
		MGlobal::executeCommand("setAttr \"polyExtrudeFace3.localTranslate\" -type double3 0 0 0.212756 ;\n polyTriangulate pCube2; ", true, true);

	}
	if ((name == "olaf") ||
		(name == "olaf.off")){
		MGlobal::displayInfo("CHARGEMENT DE LA CAGE POUR OLAF \n");
		MGlobal::displayInfo("\n");
		MGlobal::displayInfo("\n");
		MGlobal::executeCommand("CreatePolygonCube;\n polyCube - w 1 - h 1 - d 1 - sx 1 - sy 1 - sz 1 - ax 0 1 0 - cuv 4 - ch 1;\n move - r 44.526148 0 65.199002;\n setAttr \"polyCube2.width\" 35.413158;\n setAttr \"polyCube2.height\" 71.773158; \n setAttr \"polyCube2.depth\" 44.024737; \n setAttr \"polyCube2.subdivisionsHeight\" 9; \n select - r pCube2.f[11:12]; ",true,true);
		MGlobal::executeCommand("PolyExtrude;\n select - r pCube2.f[11:12];\n polyExtrudeFacet - constructionHistory 1 - keepFacesTogether 1 - pvx 44.52614706 - pvy 19.93698883 - pvz 43.1866352 - divisions 1 - twist 0 - taper 1 - off 0 - thickness 0 - smoothingAngle 30 pCube2.f[11:12];\n setAttr \"polyExtrudeFace2.localTranslate\" - type double3 0 0 38.663709; ",true,true);
		MGlobal::executeCommand("select -r pCube2.f[6:7] ;\n PolyExtrude;\n select - r pCube2.f[6:7];\n polyExtrudeFacet - constructionHistory 1 - keepFacesTogether 1 - pvx 44.52614706 - pvy 19.93698502 - pvz 87.2113697 - divisions 1 - twist 0 - taper 1 - off 0 - thickness 0 - smoothingAngle 30 pCube2.f[6:7];\n setAttr \"polyExtrudeFace3.localTranslate\" - type double3 0 0 40.768434;",true,true);
		MGlobal::executeCommand("select -r pCube2.f[32] ;\n PolyExtrude;\n select - r pCube2.f[32];\n polyExtrudeFacet - constructionHistory 1 - keepFacesTogether 1 - pvx 26.8195688 - pvy - 7.974794865 - pvz 65.19900245 - divisions 1 - twist 0 - taper 1 - off 0 - thickness 0 - smoothingAngle 30 pCube2.f[32];\n setAttr \"polyExtrudeFace5.localTranslate\" - type double3 0 0 9.357705;",true,true);
		MGlobal::executeCommand("select -r pCube2.f[31] ;\n PolyExtrude;\n select - r pCube2.f[31];\n polyExtrudeFacet - constructionHistory 1 - keepFacesTogether 1 - pvx 26.8195688 - pvy - 15.94958973 - pvz 65.19900245 - divisions 1 - twist 0 - taper 1 - off 0 - thickness 0 - smoothingAngle 30 pCube2.f[31];\n setAttr \"polyExtrudeFace7.localTranslate\" - type double3 0 0 24.878592; \n  polyTriangulate pCube2; ",true,true);
		//MGlobal::executeCommand("",true,true);
	}
	if ((name == "armadillo") ||
		(name == "armadillo.off")){
		MGlobal::displayInfo("CHARGEMENT DE LA CAGE POUR LE ARMADILLO \n");
		MGlobal::displayInfo("\n");
		MGlobal::displayInfo("\n");
		MGlobal::displayInfo("Pas de génération pour le moment ");
	}
	return MS::kSuccess;
}
