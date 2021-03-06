#include "SceneText.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "MeshBuilder.h"
#include "Application.h"
#include "Utility.h"
#include "LoadTGA.h"
#include <sstream>
#include "KeyboardController.h"
#include "MouseController.h"
#include "SceneManager.h"
#include "GraphicsManager.h"
#include "ShaderProgram.h"
#include "EntityManager.h"

#include "Pickup/Pickup.h"

#include "GenericEntity.h"
#include "GroundEntity.h"
#include "TextEntity.h"
#include "SpriteEntity.h"
#include "Light.h"
#include "SkyBox/SkyBoxEntity.h"
#include "SceneGraph\SceneGraph.h"
#include "SpatialPartition\SpatialPartition.h"

#include "RenderHelper.h"

#include "../Lua/LuaInterface.h"

#include <iostream>
using namespace std;

//SceneText* SceneText::sInstance = new SceneText(SceneManager::GetInstance());

SceneText::SceneText()
{
}

//SceneText::SceneText(SceneManager* _sceneMgr)
//{
//	_sceneMgr->AddScene("Start", this);
//}

SceneText::~SceneText()
{
	CSpatialPartition::GetInstance()->RemoveCamera();
	CSceneGraph::GetInstance()->Destroy();
}

void SceneText::Init()
{
    /*
	currProg = GraphicsManager::GetInstance()->LoadShader("default", "Shader//Texture.vertexshader", "Shader//Texture.fragmentshader");
	
	// Tell the shader program to store these uniform locations
	currProg->AddUniform("MVP");
	currProg->AddUniform("MV");
	currProg->AddUniform("MV_inverse_transpose");
	currProg->AddUniform("material.kAmbient");
	currProg->AddUniform("material.kDiffuse");
	currProg->AddUniform("material.kSpecular");
	currProg->AddUniform("material.kShininess");
	currProg->AddUniform("lightEnabled");
	currProg->AddUniform("numLights");
	currProg->AddUniform("lights[0].type");
	currProg->AddUniform("lights[0].position_cameraspace");
	currProg->AddUniform("lights[0].color");
	currProg->AddUniform("lights[0].power");
	currProg->AddUniform("lights[0].kC");
	currProg->AddUniform("lights[0].kL");
	currProg->AddUniform("lights[0].kQ");
	currProg->AddUniform("lights[0].spotDirection");
	currProg->AddUniform("lights[0].cosCutoff");
	currProg->AddUniform("lights[0].cosInner");
	currProg->AddUniform("lights[0].exponent");
	currProg->AddUniform("lights[1].type");
	currProg->AddUniform("lights[1].position_cameraspace");
	currProg->AddUniform("lights[1].color");
	currProg->AddUniform("lights[1].power");
	currProg->AddUniform("lights[1].kC");
	currProg->AddUniform("lights[1].kL");
	currProg->AddUniform("lights[1].kQ");
	currProg->AddUniform("lights[1].spotDirection");
	currProg->AddUniform("lights[1].cosCutoff");
	currProg->AddUniform("lights[1].cosInner");
	currProg->AddUniform("lights[1].exponent");
	currProg->AddUniform("colorTextureEnabled");
	currProg->AddUniform("colorTexture");
	currProg->AddUniform("textEnabled");
	currProg->AddUniform("textColor");
	
	// Tell the graphics manager to use the shader we just loaded
	GraphicsManager::GetInstance()->SetActiveShader("default");

    currProg->UpdateInt("numLights", 1);
    currProg->UpdateInt("textEnabled", 0);
    */

	lights[0] = new Light();
	GraphicsManager::GetInstance()->AddLight("lights[0]", lights[0]);
	lights[0]->type = Light::LIGHT_DIRECTIONAL;
	lights[0]->position.Set(0, 20, 0);
	lights[0]->color.Set(1, 1, 1);
	lights[0]->power = 1;
	lights[0]->kC = 1.f;
	lights[0]->kL = 0.01f;
	lights[0]->kQ = 0.001f;
	lights[0]->cosCutoff = cos(Math::DegreeToRadian(45));
	lights[0]->cosInner = cos(Math::DegreeToRadian(30));
	lights[0]->exponent = 3.f;
	lights[0]->spotDirection.Set(0.f, 1.f, 0.f);
	lights[0]->name = "lights[0]";

	lights[1] = new Light();
	GraphicsManager::GetInstance()->AddLight("lights[1]", lights[1]);
	lights[1]->type = Light::LIGHT_DIRECTIONAL;
	lights[1]->position.Set(1, 1, 0);
	lights[1]->color.Set(1, 1, 0.5f);
	lights[1]->power = 0.4f;
	lights[1]->name = "lights[1]";
	
	// Create the playerinfo instance, which manages all information about the player
	playerInfo = CPlayerInfo::GetInstance();
	playerInfo->Init();

	// Create and attach the camera to the scene
	//camera.Init(Vector3(0, 0, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	camera.Init(playerInfo->GetPos(), playerInfo->GetTarget(), playerInfo->GetUp());
	playerInfo->AttachCamera(&camera);
	GraphicsManager::GetInstance()->AttachCamera(&camera);

	// Load all the meshes
	MeshBuilder::GetInstance()->GenerateAxes("reference");
	MeshBuilder::GetInstance()->GenerateCrossHair("crosshair");
	MeshBuilder::GetInstance()->GenerateQuad("quad", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("quad")->textureID = LoadTGA("Image//calibri.tga");
	MeshBuilder::GetInstance()->GenerateText("text", 16, 16);
	MeshBuilder::GetInstance()->GetMesh("text")->textureID = LoadTGA("Image//Fonts/digital.tga");
	MeshBuilder::GetInstance()->GetMesh("text")->material.kAmbient.Set(1, 0, 0);
    //MeshBuilder::GetInstance()->GetMesh("text")->LoadFontData("Image//Fonts/FontData_Impact.csv");
	MeshBuilder::GetInstance()->GenerateRing("ring", Color(1, 0, 1), 36, 1, 0.5f);
	MeshBuilder::GetInstance()->GenerateSphere("lightball", Color(1, 1, 1), 18, 36, 1.f);
	MeshBuilder::GetInstance()->GenerateSphere("sphere", Color(1, 0, 0), 18, 36, 0.5f);
	MeshBuilder::GetInstance()->GenerateCone("cone", Color(0.5f, 1, 0.3f), 36, 10.f, 10.f);
	MeshBuilder::GetInstance()->GenerateCube("cube", Color(1.0f, 1.0f, 0.0f), 1.0f);
	MeshBuilder::GetInstance()->GetMesh("cone")->material.kDiffuse.Set(0.99f, 0.99f, 0.99f);
	MeshBuilder::GetInstance()->GetMesh("cone")->material.kSpecular.Set(0.f, 0.f, 0.f);
	MeshBuilder::GetInstance()->GenerateQuad("GRASS_DARKGREEN", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("GRASS_DARKGREEN")->textureID = LoadTGA("Image//grass_darkgreen.tga");
	MeshBuilder::GetInstance()->GenerateQuad("GEO_GRASS_LIGHTGREEN", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("GEO_GRASS_LIGHTGREEN")->textureID = LoadTGA("Image//grass_lightgreen.tga");
    MeshBuilder::GetInstance()->GenerateQuad("floor", Color(1, 1, 1), 1.f);
    MeshBuilder::GetInstance()->GetMesh("floor")->textureID = LoadTGA("Image//floor.tga");
	MeshBuilder::GetInstance()->GenerateCube("cubeSG", Color(1.0f, 0.64f, 0.0f), 1.0f);

	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_FRONT", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_BACK", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_LEFT", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_RIGHT", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_TOP", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_BOTTOM", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_FRONT")->textureID = LoadTGA("Image//SkyBox//skybox_front.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_BACK")->textureID = LoadTGA("Image//SkyBox//skybox_back.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_LEFT")->textureID = LoadTGA("Image//SkyBox//skybox_left.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_RIGHT")->textureID = LoadTGA("Image//SkyBox//skybox_right.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_TOP")->textureID = LoadTGA("Image//SkyBox//skybox_top.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_BOTTOM")->textureID = LoadTGA("Image//SkyBox//skybox_bottom.tga");
	MeshBuilder::GetInstance()->GenerateRay("laser", 10.0f);
	MeshBuilder::GetInstance()->GenerateQuad("GRIDMESH", Color(1, 1, 1), 1.f);

    // ------------ WEAPONS ------------ //
    MeshBuilder::GetInstance()->GenerateOBJ("NeedleGun", "OBJ//needle gun.obj");
    MeshBuilder::GetInstance()->GetMesh("NeedleGun")->textureID = LoadTGA("Image//needle gun.tga");
    MeshBuilder::GetInstance()->GenerateOBJ("Needle", "OBJ//needle.obj");
    MeshBuilder::GetInstance()->GetMesh("Needle")->textureID = LoadTGA("Image//needle.tga");
	MeshBuilder::GetInstance()->GenerateOBJ("Grenade", "OBJ//grenade.obj");
	MeshBuilder::GetInstance()->GetMesh("Grenade")->textureID = LoadTGA("Image//grenade_UV.tga");

    // ------------ ENVIRONMENT ------------ //
    MeshBuilder::GetInstance()->GenerateQuad("wall", Color(1, 1, 1), 1.f, 8);
    MeshBuilder::GetInstance()->GetMesh("wall")->textureID = LoadTGA("Image//wall.tga");
    MeshBuilder::GetInstance()->GenerateQuad("ceiling", Color(1, 1, 1), 1.f, 2);
    MeshBuilder::GetInstance()->GetMesh("ceiling")->textureID = LoadTGA("Image//ceiling.tga");
    MeshBuilder::GetInstance()->GenerateQuad("carpet", Color(1, 1, 1), 1.f, 4);
    MeshBuilder::GetInstance()->GetMesh("carpet")->textureID = LoadTGA("Image//ground_carpet.tga");

    MeshBuilder::GetInstance()->GenerateOBJ("high_res_lamppost", "OBJ//lamppost_high_res.obj");
    MeshBuilder::GetInstance()->GetMesh("high_res_lamppost")->textureID = LoadTGA("Image//Buildings//lamppost_high_res_UV.tga");
    MeshBuilder::GetInstance()->GenerateOBJ("med_res_lamppost", "OBJ//lamppost_med_res.obj");
    MeshBuilder::GetInstance()->GetMesh("med_res_lamppost")->textureID = LoadTGA("Image//Buildings//lamppost_med_res_UV.tga");
    MeshBuilder::GetInstance()->GenerateOBJ("low_res_lamppost", "OBJ//lamppost_low_res.obj");
    MeshBuilder::GetInstance()->GetMesh("low_res_lamppost")->textureID = LoadTGA("Image//Buildings//lamppost_low_res_UV.tga");

    MeshBuilder::GetInstance()->GenerateOBJ("high_res_tower", "OBJ//tower_high_res.obj");
    MeshBuilder::GetInstance()->GetMesh("high_res_tower")->textureID = LoadTGA("Image//Buildings//tower_high_res_UV.tga");
    MeshBuilder::GetInstance()->GenerateOBJ("med_res_tower", "OBJ//tower_high_res.obj");
    MeshBuilder::GetInstance()->GetMesh("med_res_tower")->textureID = LoadTGA("Image//Buildings//tower_med_res_UV.tga");
    MeshBuilder::GetInstance()->GenerateOBJ("low_res_tower", "OBJ//tower_low_res.obj");
    MeshBuilder::GetInstance()->GetMesh("low_res_tower")->textureID = LoadTGA("Image//Buildings//tower_low_res_UV.tga");

    MeshBuilder::GetInstance()->GenerateOBJ("castle_wall", "OBJ//wall3.obj");
    MeshBuilder::GetInstance()->GetMesh("castle_wall")->textureID = LoadTGA("Image//Buildings//wall_UV.tga");

    MeshBuilder::GetInstance()->GenerateOBJ("high_res_windmill_base", "OBJ//Windmill//windmill_base_high_res.obj");
    MeshBuilder::GetInstance()->GetMesh("high_res_windmill_base")->textureID = LoadTGA("Image//Windmill//windmill_base_high_res_UV.tga");
    MeshBuilder::GetInstance()->GenerateOBJ("med_res_windmill_base", "OBJ//Windmill//windmill_base_med_res.obj");
    MeshBuilder::GetInstance()->GetMesh("med_res_windmill_base")->textureID = LoadTGA("Image//Windmill//windmill_base_med_res_UV.tga");
    MeshBuilder::GetInstance()->GenerateOBJ("low_res_windmill_base", "OBJ//Windmill//windmill_base_low_res.obj");
    MeshBuilder::GetInstance()->GetMesh("low_res_windmill_base")->textureID = LoadTGA("Image//Windmill//windmill_base_low_res_UV.tga");

    MeshBuilder::GetInstance()->GenerateOBJ("high_res_windmill_rotator", "OBJ//Windmill//windmill_rotator_high_res.obj");
    MeshBuilder::GetInstance()->GetMesh("high_res_windmill_rotator")->textureID = LoadTGA("Image//Windmill//windmill_rotator_high_res_UV.tga");
    MeshBuilder::GetInstance()->GenerateOBJ("med_res_windmill_rotator", "OBJ//Windmill//windmill_rotator_med_res.obj");
    MeshBuilder::GetInstance()->GetMesh("med_res_windmill_rotator")->textureID = LoadTGA("Image//Windmill//windmill_rotator_med_res_UV.tga");
    MeshBuilder::GetInstance()->GenerateOBJ("low_res_windmill_rotator", "OBJ//Windmill//windmill_rotator_low_res.obj");
    MeshBuilder::GetInstance()->GetMesh("low_res_windmill_rotator")->textureID = LoadTGA("Image//Windmill//windmill_rotator_low_res_UV.tga");

    MeshBuilder::GetInstance()->GenerateOBJ("high_res_windmill_blades", "OBJ//Windmill//windmill_blades_high_res.obj");
    MeshBuilder::GetInstance()->GetMesh("high_res_windmill_blades")->textureID = LoadTGA("Image//Windmill//windmill_blades_high_res_UV.tga");
    MeshBuilder::GetInstance()->GenerateOBJ("low_res_windmill_blades", "OBJ//Windmill//windmill_blades_med_res.obj");
    MeshBuilder::GetInstance()->GetMesh("low_res_windmill_blades")->textureID = LoadTGA("Image//Windmill//windmill_blades_low_res_UV.tga");


	// Set up the Spatial Partition and pass it to the EntityManager to manage
	CSpatialPartition::GetInstance()->Init(100, 100, 10, 10);
	CSpatialPartition::GetInstance()->SetMesh("GRIDMESH");
	CSpatialPartition::GetInstance()->SetCamera(&camera);
	CSpatialPartition::GetInstance()->SetLevelOfDetails(40000.0f, 160000.0f);
	EntityManager::GetInstance()->SetSpatialPartition(CSpatialPartition::GetInstance());

    // ----------- Basic Blocks ----------- //
    MeshBuilder::GetInstance()->GenerateOBJ("high_res_cube", "OBJ//cube_high_res.obj");
    MeshBuilder::GetInstance()->GetMesh("high_res_cube")->textureID = LoadTGA("Image//Cube//cube_high_res_UV2.tga");
    MeshBuilder::GetInstance()->GenerateOBJ("med_res_cube", "OBJ//cube_low_res.obj");
    MeshBuilder::GetInstance()->GetMesh("med_res_cube")->textureID = LoadTGA("Image//Cube//cube_med_res_UV.tga");
    MeshBuilder::GetInstance()->GenerateOBJ("low_res_cube", "OBJ//cube_low_res.obj");
    MeshBuilder::GetInstance()->GetMesh("low_res_cube")->textureID = LoadTGA("Image//Cube//cube_low_res_UV.tga");

    // ----------- Enemy ----------- //
    MeshBuilder::GetInstance()->GenerateOBJ("high_res_enemy_head", "OBJ//Enemy///enemy_head_high_res.obj");
    MeshBuilder::GetInstance()->GetMesh("high_res_enemy_head")->textureID = LoadTGA("Image//Enemy//enemy_head_high_res_UV.tga");
    MeshBuilder::GetInstance()->GenerateOBJ("low_res_enemy_head", "OBJ//Enemy///enemy_head_low_res.obj");
    MeshBuilder::GetInstance()->GetMesh("low_res_enemy_head")->textureID = LoadTGA("Image//Enemy//enemy_head_low_res_UV.tga");

    MeshBuilder::GetInstance()->GenerateOBJ("high_res_enemy_body", "OBJ//Enemy///enemy_body_high_res.obj");
    MeshBuilder::GetInstance()->GetMesh("high_res_enemy_body")->textureID = LoadTGA("Image//Enemy//enemy_body_high_res_UV.tga");
    MeshBuilder::GetInstance()->GenerateOBJ("low_res_enemy_body", "OBJ//Enemy///enemy_body_low_res.obj");
    MeshBuilder::GetInstance()->GetMesh("low_res_enemy_body")->textureID = LoadTGA("Image//Enemy//enemy_body_low_res_UV.tga");

    MeshBuilder::GetInstance()->GenerateOBJ("high_res_enemy_arm", "OBJ//Enemy///enemy_arm_high_res.obj");
    MeshBuilder::GetInstance()->GetMesh("high_res_enemy_arm")->textureID = LoadTGA("Image//Enemy//enemy_arm_high_res_UV.tga");
    MeshBuilder::GetInstance()->GenerateOBJ("low_res_enemy_arm", "OBJ//Enemy///enemy_arm_low_res.obj");
    MeshBuilder::GetInstance()->GetMesh("low_res_enemy_arm")->textureID = LoadTGA("Image//Enemy//enemy_arm_low_res_UV.tga");

    MeshBuilder::GetInstance()->GenerateOBJ("high_res_enemy_leg", "OBJ//Enemy///enemy_leg_high_res.obj");
    MeshBuilder::GetInstance()->GetMesh("high_res_enemy_leg")->textureID = LoadTGA("Image//Enemy//enemy_leg_high_res_UV.tga");
    MeshBuilder::GetInstance()->GenerateOBJ("low_res_enemy_leg", "OBJ//Enemy///enemy_leg_low_res.obj");
    MeshBuilder::GetInstance()->GetMesh("low_res_enemy_leg")->textureID = LoadTGA("Image//Enemy//enemy_leg_low_res_UV.tga");

    MeshBuilder::GetInstance()->GenerateOBJ("low_res_enemy_overall", "OBJ//Enemy///enemy_overall_low_res.obj");
    MeshBuilder::GetInstance()->GetMesh("low_res_enemy_overall")->textureID = LoadTGA("Image//Enemy//enemy_overall_low_res_UV.tga");

    // ----------- Pickups ----------- //
    MeshBuilder::GetInstance()->GenerateOBJ("high_res_diamond", "OBJ//diamond_high_res.obj");
    MeshBuilder::GetInstance()->GetMesh("high_res_diamond")->textureID = LoadTGA("Image//Diamond/diamond_high_res.tga");
    MeshBuilder::GetInstance()->GenerateOBJ("med_res_diamond", "OBJ//diamond_med_res.obj");
    MeshBuilder::GetInstance()->GetMesh("med_res_diamond")->textureID = LoadTGA("Image//Diamond/diamond_med_res.tga");
    MeshBuilder::GetInstance()->GenerateOBJ("low_res_diamond", "OBJ//diamond_low_res.obj");
    MeshBuilder::GetInstance()->GetMesh("low_res_diamond")->textureID = LoadTGA("Image//Diamond/diamond_low_res.tga");

    MeshBuilder::GetInstance()->GenerateOBJ("high_res_ammo", "OBJ//cube_high_res.obj");
    MeshBuilder::GetInstance()->GetMesh("high_res_ammo")->textureID = LoadTGA("Image//ammo_high_res.tga");
    MeshBuilder::GetInstance()->GenerateOBJ("med_res_ammo", "OBJ//cube_low_res.obj");
    MeshBuilder::GetInstance()->GetMesh("med_res_ammo")->textureID = LoadTGA("Image//ammo_med_res.tga");
    MeshBuilder::GetInstance()->GenerateOBJ("low_res_ammo", "OBJ//cube_low_res.obj");
    MeshBuilder::GetInstance()->GetMesh("low_res_ammo")->textureID = LoadTGA("Image//ammo_low_res.tga");

    // ----------- 2D HUD ----------- //
    MeshBuilder::GetInstance()->GenerateQuad("HUD_needle", Color(1, 1, 1), 1.f);
    MeshBuilder::GetInstance()->GetMesh("HUD_needle")->textureID = LoadTGA("Image//HUD/HUD_needle.tga");
    MeshBuilder::GetInstance()->GenerateQuad("HUD_grenade", Color(1, 1, 1), 1.f);
    MeshBuilder::GetInstance()->GetMesh("HUD_grenade")->textureID = LoadTGA("Image//HUD/HUD_grenade.tga");
    MeshBuilder::GetInstance()->GenerateQuad("HUD_diamond", Color(1, 1, 1), 1.f);
    MeshBuilder::GetInstance()->GetMesh("HUD_diamond")->textureID = LoadTGA("Image//HUD/HUD_diamond.tga");

	// Create entities into the scene
	Create::Entity("reference", Vector3(0.0f, 0.0f, 0.0f)); // Reference
	Create::Entity("lightball", Vector3(lights[0]->position.x, lights[0]->position.y, lights[0]->position.z)); // Lightball

	//GenericEntity* aCube = Create::Entity("cube", Vector3(-20.0f, 0.0f, -20.0f));
	//aCube->SetCollider(true);
	//aCube->SetAABB(Vector3(0.5f, 0.5f, 0.5f), Vector3(-0.5f, -0.5f, -0.5f));
	//aCube->InitLOD("cube", "sphere", "cubeSG");
    //
	//// Add the pointer to this new entity to the Scene Graph
	//CSceneNode* theNode = CSceneGraph::GetInstance()->AddNode(aCube);
	//if (theNode == NULL)
	//{
	//	cout << "EntityManager::AddEntity: Unable to add to scene graph!" << endl;
	//}
    //
	//GenericEntity* anotherCube = Create::Entity("cube", Vector3(-20.0f, 1.1f, -20.0f));
	//anotherCube->SetCollider(true);
	//anotherCube->SetAABB(Vector3(0.5f, 0.5f, 0.5f), Vector3(-0.5f, -0.5f, -0.5f));
	//CSceneNode* anotherNode = theNode->AddChild(anotherCube);
	//if (anotherNode == NULL)
	//{
	//	cout << "EntityManager::AddEntity: Unable to add to scene graph!" << endl;
	//}
	//
	//GenericEntity* baseCube = Create::Asset("cube", Vector3(0.0f, 0.0f, 0.0f));
	//CSceneNode* baseNode = CSceneGraph::GetInstance()->AddNode(baseCube);
    //
	//CUpdateTransformation* baseMtx = new CUpdateTransformation();
	//baseMtx->ApplyUpdate(1.0f, 0.0f, 0.0f, 1.0f);
	//baseMtx->SetSteps(-60, 60);
	//baseNode->SetUpdateTransformation(baseMtx);
    //
	//GenericEntity* childCube = Create::Asset("cubeSG", Vector3(0.0f, 0.0f, 0.0f));
	//CSceneNode* childNode = baseNode->AddChild(childCube);
	//childNode->ApplyTranslate(0.0f, 2.0f, 0.0f);
    //
	//GenericEntity* grandchildCube = Create::Asset("cubeSG", Vector3(0.0f, 0.0f, 0.0f));
	//CSceneNode* grandchildNode = childNode->AddChild(grandchildCube);
	//grandchildNode->ApplyTranslate(0.0f, 0.0f, 1.0f);
	//CUpdateTransformation* aRotateMtx = new CUpdateTransformation();
	//aRotateMtx->ApplyUpdate(1.0f, 0.0f, 0.0f, 1.0f);
	//aRotateMtx->SetSteps(-120, 60);
	//grandchildNode->SetUpdateTransformation(aRotateMtx);

	//groundEntity = Create::Ground("GRASS_DARKGREEN", "GEO_GRASS_LIGHTGREEN");
    groundEntity = Create::Ground("carpet", "carpet");

//	Create::Text3DObject("text", Vector3(0.0f, 0.0f, 0.0f), "DM2210", Vector3(10.0f, 10.0f, 10.0f), Color(0, 1, 1));
	Create::Sprite2DObject("crosshair", Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f));

	SkyBoxEntity* theSkyBox = Create::SkyBox("wall", "wall",
											 "wall", "wall",
											 "ceiling", "carpet");

    // ----------- Spawn Buildings ----------- //
    SpawnArena(Vector3(10, 1.f, 10));
    SpawnTunnel(Vector3(-10, 0.f, -10));
    SpawnArena(Vector3(-20, 1.f, 30));
    SpawnJumpCourse(Vector3(-20, 0, -20));

    SpawnWindmill(Vector3(0, 0, -30));

	//SpawnCastle(Vector3(-20, -0, 0));

    // ----------- Spawn Pickups ----------- //
    SpawnDiamond(Vector3(-50, 5, 20));
    SpawnDiamond(Vector3(80, 5, 70));
    SpawnDiamond(Vector3(50, 5, 0));
    SpawnAmmo(Vector3(-50, 3, 30));
    SpawnAmmo(Vector3(-50, 3, 60));
    SpawnAmmo(Vector3(40, 3, -10));

	// Customise the ground entity
	groundEntity->SetPosition(Vector3(0, 0, 0));
	groundEntity->SetScale(Vector3(100.0f, 100.0f, 100.0f));
	groundEntity->SetGrids(Vector3(10.0f, 1.0f, 10.0f));
	playerInfo->SetTerrain(groundEntity);

	// Create a new CEnemy
	theEnemy = new CEnemy();
    theEnemy->SetTerrain(groundEntity);
	theEnemy->Init();

    vector<Vector3>waypoints;
    waypoints.push_back(Vector3(-50, 5, 50));
    waypoints.push_back(Vector3(15, 5, 15));
    waypoints.push_back(Vector3(8, 5, 14));
    waypoints.push_back(Vector3(17, 5, 8));
    theEnemy->SetWaypoints(waypoints);

    // Create a new CEnemy
    theEnemy = new CEnemy();
    theEnemy->SetTerrain(groundEntity);
    theEnemy->Init();

    vector<Vector3>waypoints2;
    waypoints2.push_back(Vector3(100, 5, -15));
    waypoints2.push_back(Vector3(-20, 5, 15));

    theEnemy->SetWaypoints(waypoints2);


	// Setup the 2D entities
	float halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2.0f;
	float halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2.0f;
	float fontSize = 25.0f;
	float halfFontSize = fontSize / 2.0f;
    for (int i = 0; i < 7; ++i)
    {
        textObj[i] = Create::Text2DObject("text", Vector3(-halfWindowWidth + 0.5f * halfFontSize, halfWindowHeight - fontSize*i - halfFontSize, 0.0f), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f, 1.0f, 0.0f));
    }
    textObj[0]->SetText("INFLATABLE WORLD");

    textObj[3] = Create::Text2DObject("text", Vector3(0.5f * halfFontSize, halfWindowHeight - halfFontSize, 0.0f), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f, 1.0f, 0.0f));
    textObj[4] = Create::Text2DObject("text", Vector3(-halfWindowWidth + 3.f * fontSize, -halfWindowHeight + fontSize, 0.0f), "", Vector3(0.9f * fontSize, 0.9f * fontSize, 0.9f * fontSize), Color(0.0f, 1.0f, 0.0f));
    textObj[5] = Create::Text2DObject("text", Vector3(-halfWindowWidth + 5.f * fontSize, -halfWindowHeight + fontSize, 0.0f), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f, 1.0f, 0.0f));
    textObj[6] = Create::Text2DObject("text", Vector3(0.5f* halfWindowWidth + 2.f * fontSize + 0.5f * halfFontSize, -halfWindowHeight + fontSize, 0.0f), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f, 1.0f, 0.0f));

    HUDObj[0] = Create::Sprite2DObjectOutsideEntityManager("HUD_needle", Vector3(-halfWindowWidth + 1.5f * fontSize, -halfWindowHeight + 1.5f * fontSize, 0.f), Vector3(50, 50, 50));
    HUDObj[1] = Create::Sprite2DObjectOutsideEntityManager("HUD_grenade", Vector3(-halfWindowWidth + 1.5f *  fontSize, -halfWindowHeight + 1.5f * fontSize, 0.f), Vector3(50, 50, 50));
    HUDObj[2] = Create::Sprite2DObject("HUD_diamond", Vector3(halfWindowWidth - 7.5f * fontSize, -halfWindowHeight + 1.5f * fontSize, 0.f), Vector3(50, 50, 50));

    playerInfo->GetHeldWeapon()->SetWeaponType(HeldWeapon::WEAPON_NEEDLEGUN);
}

void SceneText::Update(double dt)
{
    // Update the player position and other details based on keyboard and mouse inputs
    playerInfo->Update(dt);

	// Update our entities
	EntityManager::GetInstance()->Update(dt, playerInfo);

	// THIS WHOLE CHUNK TILL <THERE> CAN REMOVE INTO ENTITIES LOGIC! Or maybe into a scene function to keep the update clean
	//if(KeyboardController::GetInstance()->IsKeyDown('1'))
	//	glEnable(GL_CULL_FACE);
	//if(KeyboardController::GetInstance()->IsKeyDown('2'))
	//	glDisable(GL_CULL_FACE);
	if(KeyboardController::GetInstance()->IsKeyDown('3'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if(KeyboardController::GetInstance()->IsKeyDown('4'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	//if(KeyboardController::GetInstance()->IsKeyDown('5'))
	//{
	//	lights[0]->type = Light::LIGHT_POINT;
	//}
	//else if(KeyboardController::GetInstance()->IsKeyDown('6'))
	//{
	//	lights[0]->type = Light::LIGHT_DIRECTIONAL;
	//}
	//else if(KeyboardController::GetInstance()->IsKeyDown('7'))
	//{
	//	lights[0]->type = Light::LIGHT_SPOT;
	//}
    
	//if(KeyboardController::GetInstance()->IsKeyDown('I'))
	//	lights[0]->position.z -= (float)(10.f * dt);
	//if(KeyboardController::GetInstance()->IsKeyDown('K'))
	//	lights[0]->position.z += (float)(10.f * dt);
	//if(KeyboardController::GetInstance()->IsKeyDown('J'))
	//	lights[0]->position.x -= (float)(10.f * dt);
	//if(KeyboardController::GetInstance()->IsKeyDown('L'))
	//	lights[0]->position.x += (float)(10.f * dt);
	//if(KeyboardController::GetInstance()->IsKeyDown('O'))
	//	lights[0]->position.y -= (float)(10.f * dt);
	//if(KeyboardController::GetInstance()->IsKeyDown('P'))
	//	lights[0]->position.y += (float)(10.f * dt);

	if (KeyboardController::GetInstance()->IsKeyReleased('M'))
	{
		CSceneNode* theNode = CSceneGraph::GetInstance()->GetNode(1);
		Vector3 pos = theNode->GetEntity()->GetPosition();
		theNode->GetEntity()->SetPosition(Vector3(pos.x + 50.0f, pos.y, pos.z + 50.0f));
	}
	if (KeyboardController::GetInstance()->IsKeyReleased('N'))
	{
		CSpatialPartition::GetInstance()->PrintSelf();
	}

	// if the left mouse button was released
	//if (MouseController::GetInstance()->IsButtonReleased(MouseController::LMB))
	//{
	//	cout << "Left Mouse Button was released!" << endl;
	//}
	//if (MouseController::GetInstance()->IsButtonReleased(MouseController::RMB))
	//{
	//	cout << "Right Mouse Button was released!" << endl;
	//}
	//if (MouseController::GetInstance()->IsButtonReleased(MouseController::MMB))
	//{
	//	cout << "Middle Mouse Button was released!" << endl;
	//}
	//if (MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_XOFFSET) != 0.0)
	//{
	//	cout << "Mouse Wheel has offset in X-axis of " << MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_XOFFSET) << endl;
	//}
	//if (MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_YOFFSET) != 0.0)
	//{
	//	cout << "Mouse Wheel has offset in Y-axis of " << MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_YOFFSET) << endl;
	//}
	// <THERE>

	//camera.Update(dt); // Can put the camera into an entity rather than here (Then we don't have to write this)

	GraphicsManager::GetInstance()->UpdateLights(dt);

	// Update the 2 text object values. NOTE: Can do this in their own class but i'm lazy to do it now :P
	// Eg. FPSRenderEntity or inside RenderUI for LightEntity
    //std::ostringstream ss;
    //ss.precision(5);
    //float fps = (float)(1.f / dt);
    //ss << "FPS: " << fps;
    //textObj[2]->SetText(ss.str());

    std::ostringstream ss1;
    ss1 << "Player(" << (int)playerInfo->GetPos().x << "," << (int)playerInfo->GetPos().y << "," << (int)playerInfo->GetPos().z << ")";
    textObj[1]->SetText(ss1.str());

    // how many balloons destroyed
    std::ostringstream ss2;
    //ss2 << "Balloons Destroyed:" << 0;
    textObj[3]->SetText(ss2.str());

    // ammo left
    std::ostringstream ss3;
    ss3 << playerInfo->GetHeldWeapon()->GetCurrentWeapon()->GetTotalRound() + playerInfo->GetHeldWeapon()->GetCurrentWeapon()->GetMagRound();
    textObj[4]->SetText(ss3.str());

    // total max ammo that can be carried
    std::ostringstream ss4;
    ss4 << "/" << playerInfo->GetHeldWeapon()->GetCurrentWeapon()->GetMaxTotalRound();
    textObj[5]->SetText(ss4.str());

    // how many diamonds left
    std::ostringstream ss5;
    ss5 << playerInfo->GetDiamondsLeftToCollect() << " left";
    textObj[6]->SetText(ss5.str());
}

void SceneText::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GraphicsManager::GetInstance()->UpdateLightUniforms();

	// Setup 3D pipeline then render 3D
	GraphicsManager::GetInstance()->SetPerspectiveProjection(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
	GraphicsManager::GetInstance()->AttachCamera(&camera);
	EntityManager::GetInstance()->Render();
    playerInfo->RenderWeapon();
    CSpatialPartition::GetInstance()->RenderSingleGrid(playerInfo->GetPos());

	// Setup 2D pipeline then render 2D
	int halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2;
	int halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2;
	GraphicsManager::GetInstance()->SetOrthographicProjection(-halfWindowWidth, halfWindowWidth, -halfWindowHeight, halfWindowHeight, -10, 10);
	GraphicsManager::GetInstance()->DetachCamera();
	EntityManager::GetInstance()->RenderUI();

    // Render HUD objects
    MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
    modelStack.PushMatrix();
    modelStack.Translate(HUDObj[0]->GetPosition().x, HUDObj[0]->GetPosition().y, HUDObj[0]->GetPosition().z);
    modelStack.Scale(HUDObj[0]->GetScale().x, HUDObj[0]->GetScale().y, HUDObj[0]->GetScale().z);
    if (playerInfo->GetHeldWeapon()->GetWeaponType() == HeldWeapon::WEAPON_NEEDLEGUN)
    {
        RenderHelper::RenderMesh(HUDObj[0]->GetMesh());
    }
    else if (playerInfo->GetHeldWeapon()->GetWeaponType() == HeldWeapon::WEAPON_GRENADE)
    {
        RenderHelper::RenderMesh(HUDObj[1]->GetMesh());
    }
    modelStack.PopMatrix();
}

void SceneText::Exit()
{
	// Detach camera from other entities
	GraphicsManager::GetInstance()->DetachCamera();
	playerInfo->DetachCamera();

	if (playerInfo->DropInstance() == false)
	{
#if _DEBUGMODE==1
		cout << "Unable to drop PlayerInfo class" << endl;
#endif
	}

	// Delete the lights
	delete lights[0];
	delete lights[1];
}

void SceneText::SpawnArena(Vector3 spawnPos)
{
    Vector3 scale(5, 3, 5);

    // ----------- LOW RES ----------- //
    GenericBalloon* lowResStructure = Create::Balloon("low_res_cube", Vector3(0, 0, 0));
    lowResStructure->SetCollider(false);
    lowResStructure->SetScale(Vector3(10, 5, 14));
    lowResStructure->InitLOD("", "", "low_res_cube");
    lowResStructure->SetPosition(spawnPos);
    lowResStructure->SetLowResRender(true);
    // Add to SceneGraph
    CSceneNode* lowResNode = CSceneGraph::GetInstance()->AddNode(lowResStructure);
    lowResNode->SetScale(scale.x, scale.y, scale.z);
    //lowResNode->ApplyTranslate(spawnPos.x, spawnPos.y, spawnPos.x);


    // ----------- INDIVIDUAL PARTS ----------- //
    // Base Block / Floor of the arena
    GenericBalloon* baseBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
    baseBlock->SetCollider(true);
    baseBlock->SetScale(Vector3(10, 2, 10));
    baseBlock->SetAABB(Vector3(5, 1, 5), Vector3(-5, -1, -5));
    baseBlock->InitLOD("high_res_cube", "med_res_cube", "");
    baseBlock->SetPosition(spawnPos);
    CSceneNode* baseNode = lowResNode->AddChild(baseBlock);
    
    // Left Wall - Btm
    GenericBalloon* leftWallBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
    leftWallBlock->SetCollider(true);
    leftWallBlock->SetScale(Vector3(0.5, 1, 9));
    leftWallBlock->SetAABB(Vector3(0.25f, 0.5f, 4.5f), Vector3(-0.25f, -0.5f, -4.5f));
    leftWallBlock->InitLOD("high_res_cube", "med_res_cube", "");
    leftWallBlock->SetPosition(spawnPos);
    CSceneNode* leftWallNode = lowResNode->AddChild(leftWallBlock);
    leftWallNode->ApplyTranslate(-4.75f, 1.5f, 0.f);

    // Left Wall - Top
    leftWallBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
    leftWallBlock->SetCollider(true);
    leftWallBlock->SetScale(Vector3(0.5, 1, 9));
    leftWallBlock->SetAABB(Vector3(0.25f, 0.5f, 4.5f), Vector3(-0.25f, -0.5f, -4.5f));
    leftWallBlock->InitLOD("high_res_cube", "med_res_cube", "");
    leftWallBlock->SetPosition(spawnPos);
    CSceneNode* leftWallNode2 = lowResNode->AddChild(leftWallBlock);
    leftWallNode2->ApplyTranslate(-4.75f, 2.5f, 0.f);

    // Right Wall - Btm
    GenericBalloon* rightWallBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
    rightWallBlock->SetCollider(true);
    rightWallBlock->SetScale(Vector3(0.5, 1, 9));
    rightWallBlock->SetAABB(Vector3(0.25f, 0.5f, 4.5f), Vector3(-0.25f, -0.5f, -4.5f));
    rightWallBlock->InitLOD("high_res_cube", "med_res_cube", "");
    rightWallBlock->SetPosition(spawnPos);
    CSceneNode* rightWallNode = lowResNode->AddChild(rightWallBlock);
    rightWallNode->ApplyTranslate(4.75f, 1.5f, 0.f);

    // Right Wall - Top
    rightWallBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
    rightWallBlock->SetCollider(true);
    rightWallBlock->SetScale(Vector3(0.5, 1, 9));
    rightWallBlock->SetAABB(Vector3(0.25f, 0.5f, 4.5f), Vector3(-0.25f, -0.5f, -4.5f));
    rightWallBlock->InitLOD("high_res_cube", "med_res_cube", "");
    rightWallBlock->SetPosition(spawnPos);
    CSceneNode* rightWallNode2 = lowResNode->AddChild(rightWallBlock);
    rightWallNode2->ApplyTranslate(4.75f, 2.5f, 0.f);

    // Back Wall - Btm
    GenericBalloon* backWallBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
    backWallBlock->SetCollider(true);
    backWallBlock->SetScale(Vector3(9, 1, 0.5));
    backWallBlock->SetAABB(Vector3(4.5f, 0.5f, 0.25f), Vector3(-4.5f, -0.5f, -0.25f));
    backWallBlock->InitLOD("high_res_cube", "med_res_cube", "");
    backWallBlock->SetPosition(spawnPos);
    CSceneNode* backWallNode = lowResNode->AddChild(backWallBlock);
    backWallNode->ApplyTranslate(0.f, 1.5f, -4.75f);
    //backWallNode->ApplyRotate(90, 0, 1, 0);

    // Back Wall - Top
    backWallBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
    backWallBlock->SetCollider(true);
    backWallBlock->SetScale(Vector3(9, 1, 0.5));
    backWallBlock->SetAABB(Vector3(4.5f, 0.5f, 0.25f), Vector3(-4.5f, -0.5f, -0.25f));
    backWallBlock->InitLOD("high_res_cube", "med_res_cube", "");
    backWallBlock->SetPosition(spawnPos);
    CSceneNode* backWallNode2 = lowResNode->AddChild(backWallBlock);
    backWallNode2->ApplyTranslate(0.f, 2.5f, -4.75f);
    //backWallNode2->ApplyRotate(90, 0, 1, 0);

    // Front Left Wall - Btm
    GenericBalloon* frontLeftWallBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
    frontLeftWallBlock->SetCollider(true);
    frontLeftWallBlock->SetScale(Vector3(3, 1, 0.5));
    frontLeftWallBlock->SetAABB(Vector3(1.5f, 0.5f, 0.25f), Vector3(-1.5f, -0.5f, -0.25f));
    frontLeftWallBlock->InitLOD("high_res_cube", "med_res_cube", "");
    frontLeftWallBlock->SetPosition(spawnPos);
    CSceneNode* frontLeftWallNode = lowResNode->AddChild(frontLeftWallBlock);
    frontLeftWallNode->ApplyTranslate(-3.f, 1.5f, 4.75f);
    //frontLeftWallNode->ApplyRotate(90, 0, 1, 0);

    // Front Left Wall - Top
    frontLeftWallBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
    frontLeftWallBlock->SetCollider(true);
    frontLeftWallBlock->SetScale(Vector3(3, 1, 0.5));
    frontLeftWallBlock->SetAABB(Vector3(1.5f, 0.5f, 0.25f), Vector3(-1.5f, -0.5f, -0.25f));
    frontLeftWallBlock->InitLOD("high_res_cube", "med_res_cube", "");
    frontLeftWallBlock->SetPosition(spawnPos);
    CSceneNode* frontLeftWallNode2 = lowResNode->AddChild(frontLeftWallBlock);
    frontLeftWallNode2->ApplyTranslate(-3.f, 2.5f, 4.75f);
    //frontLeftWallNode2->ApplyRotate(90, 0, 1, 0);

    // Front Right Wall - Btm
    GenericBalloon* frontRightWallBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
    frontRightWallBlock->SetCollider(true);
    frontRightWallBlock->SetScale(Vector3(3, 1, 0.5));
    frontRightWallBlock->SetAABB(Vector3(1.5f, 0.5f, 0.25f), Vector3(-1.5f, -0.5f, -0.25f));
    frontRightWallBlock->InitLOD("high_res_cube", "med_res_cube", "");
    frontRightWallBlock->SetPosition(spawnPos);
    CSceneNode* frontRightWallNode = lowResNode->AddChild(frontRightWallBlock);
    frontRightWallNode->ApplyTranslate(3.f, 1.5f, 4.75f);
    //frontRightWallNode->ApplyRotate(90, 0, 1, 0);

    // Front Right Wall - Top
    frontRightWallBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
    frontRightWallBlock->SetCollider(true);
    frontRightWallBlock->SetScale(Vector3(3, 1, 0.5));
    frontRightWallBlock->SetAABB(Vector3(1.5f, 0.5f, 0.25f), Vector3(-1.5f, -0.5f, -0.25f));
    frontRightWallBlock->InitLOD("high_res_cube", "med_res_cube", "");
    frontRightWallBlock->SetPosition(spawnPos);
    CSceneNode* frontRightWallNode2 = lowResNode->AddChild(frontRightWallBlock);
    frontRightWallNode2->ApplyTranslate(3.f, 2.5f, 4.75f);
    //frontRightWallNode2->ApplyRotate(90, 0, 1, 0);

    // First Step
    GenericBalloon* firstStepBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
    firstStepBlock->SetCollider(true);
    firstStepBlock->SetScale(Vector3(3, 1, 2));
    firstStepBlock->SetAABB(Vector3(1.5f, 0.5f, 1), Vector3(-1.5f, -0.5f, -1));
    firstStepBlock->InitLOD("high_res_cube", "med_res_cube", "");
    firstStepBlock->SetPosition(spawnPos);
    CSceneNode* firstStepNode = lowResNode->AddChild(firstStepBlock);
    firstStepNode->ApplyTranslate(0.f, -0.5f, 6.f);

    //// Second Step
    //GenericBalloon* secondStepBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
    //secondStepBlock->SetCollider(true);
    //secondStepBlock->SetScale(Vector3(3, 1, 1));
    //secondStepBlock->SetAABB(Vector3(1.5f, 0.5f, 0.5f), Vector3(-1.5f, -0.5f, -0.5f));
    //secondStepBlock->InitLOD("high_res_cube", "med_res_cube", "");
    //secondStepBlock->SetPosition(spawnPos);
    //CSceneNode* secondStepNode = lowResNode->AddChild(secondStepBlock);
    //secondStepNode->ApplyTranslate(0.f, 0.5f, 5.5f);
}

void SceneText::SpawnTunnel(Vector3 spawnPos)
{
    // ----------- LOW RES ----------- //
    // Tunnel Structure Low Res
    GenericBalloon* lowResTunnel = Create::Balloon("low_res_cube", Vector3(0, 0, 0));
    lowResTunnel->SetCollider(false);
    lowResTunnel->SetScale(Vector3(3.f, 10, 11.f));
    lowResTunnel->InitLOD("", "", "low_res_cube");
    lowResTunnel->SetLowResRender(true);
    lowResTunnel->SetPosition(spawnPos);
    // Add to Scene Graph
    CSceneNode* lowResNode = CSceneGraph::GetInstance()->AddNode(lowResTunnel);
    lowResNode->SetScale(3, 3, 3);
    //lowResNode->ApplyTranslate(spawnPos.x, spawnPos.y, spawnPos.x);


    // ----------- INDIVIDUAL PARTS ----------- //
    // Base Block / Floor of the tunnel
    GenericBalloon* baseBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
    baseBlock->SetCollider(true);
    baseBlock->SetScale(Vector3(6, 1, 12));
    baseBlock->SetAABB(Vector3(3, 0.5f, 6), Vector3(-2, -0.5f, -6));
    baseBlock->InitLOD("high_res_cube", "med_res_cube", "");
    baseBlock->SetPosition(spawnPos);
    CSceneNode* baseNode = lowResNode->AddChild(baseBlock);

    // Tunnel Left Wall - Btm
    GenericBalloon* tunnelLeftWallBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
    tunnelLeftWallBlock->SetCollider(true);
    tunnelLeftWallBlock->SetScale(Vector3(1, 1, 5));
    tunnelLeftWallBlock->SetAABB(Vector3(0.5f, 0.5f, 2.5f), Vector3(-0.5f, -0.5f, -2.5f));
    tunnelLeftWallBlock->InitLOD("high_res_cube", "med_res_cube", "");
    tunnelLeftWallBlock->SetPosition(spawnPos);
    CSceneNode* tunnelLeftWallNode = lowResNode->AddChild(tunnelLeftWallBlock);
    tunnelLeftWallNode->ApplyTranslate(-2.5f, 1.0f, 2.5f);

    // Tunnel Left Wall - Mid
    tunnelLeftWallBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
    tunnelLeftWallBlock->SetCollider(true);
    tunnelLeftWallBlock->SetScale(Vector3(1, 1, 5));
    tunnelLeftWallBlock->SetAABB(Vector3(0.5f, 0.5f, 2.5f), Vector3(-0.5f, -0.5f, -2.5f));
    tunnelLeftWallBlock->InitLOD("high_res_cube", "med_res_cube", "");
    tunnelLeftWallBlock->SetPosition(spawnPos);
    CSceneNode* tunnelLeftWallNode2 = lowResNode->AddChild(tunnelLeftWallBlock);
    tunnelLeftWallNode2->ApplyTranslate(-2.5f, 2.0f, 2.5f);

    // Tunnel Left Wall - Top
    tunnelLeftWallBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
    tunnelLeftWallBlock->SetCollider(true);
    tunnelLeftWallBlock->SetScale(Vector3(1, 1, 5));
    tunnelLeftWallBlock->SetAABB(Vector3(0.5f, 0.5f, 2.5f), Vector3(-0.5f, -0.5f, -2.5f));
    tunnelLeftWallBlock->InitLOD("high_res_cube", "med_res_cube", "");
    tunnelLeftWallBlock->SetPosition(spawnPos);
    CSceneNode* tunnelLeftWallNode3 = lowResNode->AddChild(tunnelLeftWallBlock);
    tunnelLeftWallNode3->ApplyTranslate(-2.0f, 3.0f, 2.5f);

    // Tunnel Left Wall - Roof
    tunnelLeftWallBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
    tunnelLeftWallBlock->SetCollider(true);
    tunnelLeftWallBlock->SetScale(Vector3(1.5, 1, 5));
    tunnelLeftWallBlock->SetAABB(Vector3(0.75f, 0.5f, 2.5f), Vector3(-0.75f, -0.5f, -2.5f));
    tunnelLeftWallBlock->InitLOD("high_res_cube", "med_res_cube", "");
    tunnelLeftWallBlock->SetPosition(spawnPos);
    CSceneNode* tunnelLeftWallNode4 = lowResNode->AddChild(tunnelLeftWallBlock);
    tunnelLeftWallNode4->ApplyTranslate(-1.75f, 4.0f, 2.5f);

    // Tunnel Right Wall - Btm
    GenericBalloon* tunnelRightWallBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
    tunnelRightWallBlock->SetCollider(true);
    tunnelRightWallBlock->SetScale(Vector3(1, 1, 5));
    tunnelRightWallBlock->SetAABB(Vector3(0.5f, 0.5f, 2.5f), Vector3(-0.5f, -0.5f, -2.5f));
    tunnelRightWallBlock->InitLOD("high_res_cube", "med_res_cube", "");
    tunnelRightWallBlock->SetPosition(spawnPos);
    CSceneNode* tunnelRightWallNode = lowResNode->AddChild(tunnelRightWallBlock);
    tunnelRightWallNode->ApplyTranslate(2.5f, 1.0f, 2.5f);

    // Tunnel Right Wall - Mid
    tunnelRightWallBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
    tunnelRightWallBlock->SetCollider(true);
    tunnelRightWallBlock->SetScale(Vector3(1, 1, 5));
    tunnelRightWallBlock->SetAABB(Vector3(0.5f, 0.5f, 2.5f), Vector3(-0.5f, -0.5f, -2.5f));
    tunnelRightWallBlock->InitLOD("high_res_cube", "med_res_cube", "");
    tunnelRightWallBlock->SetPosition(spawnPos);
    CSceneNode* tunnelRightWallNode2 = lowResNode->AddChild(tunnelRightWallBlock);
    tunnelRightWallNode2->ApplyTranslate(2.5f, 2.0f, 2.5f);

    // Tunnel Right Wall - Top
    tunnelRightWallBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
    tunnelRightWallBlock->SetCollider(true);
    tunnelRightWallBlock->SetScale(Vector3(1, 1, 5));
    tunnelRightWallBlock->SetAABB(Vector3(0.5f, 0.5f, 2.5f), Vector3(-0.5f, -0.5f, -2.5f));
    tunnelRightWallBlock->InitLOD("high_res_cube", "med_res_cube", "");
    tunnelRightWallBlock->SetPosition(spawnPos);
    CSceneNode* tunnelRightWallNode3 = lowResNode->AddChild(tunnelRightWallBlock);
    tunnelRightWallNode3->ApplyTranslate(2.0f, 3.0f, 2.5f);

    // Tunnel Right Wall - Roof
    tunnelRightWallBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
    tunnelRightWallBlock->SetCollider(true);
    tunnelRightWallBlock->SetScale(Vector3(1.5, 1, 5));
    tunnelRightWallBlock->SetAABB(Vector3(0.75f, 0.5f, 2.5f), Vector3(-0.75f, -0.5f, -2.5f));
    tunnelRightWallBlock->InitLOD("high_res_cube", "med_res_cube", "");
    tunnelRightWallBlock->SetPosition(spawnPos);
    CSceneNode* tunnelRightWallNode4 = lowResNode->AddChild(tunnelRightWallBlock);
    tunnelRightWallNode4->ApplyTranslate(1.75f, 4.0f, 2.5f);

    // Ladder Walls, Btm - Top
    for (int height = 1; height <= 4; ++height)
    {
        GenericBalloon* ladderLeftWallBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
        ladderLeftWallBlock->SetCollider(true);
        ladderLeftWallBlock->SetScale(Vector3(0.5, 1, 4));
        ladderLeftWallBlock->SetAABB(Vector3(0.25f, 0.5f, 2), Vector3(-0.25f, -0.5f, -2));
        ladderLeftWallBlock->InitLOD("high_res_cube", "med_res_cube", "");
        ladderLeftWallBlock->SetPosition(spawnPos);
        CSceneNode* ladderLeftWallNode = lowResNode->AddChild(ladderLeftWallBlock);
        ladderLeftWallNode->ApplyTranslate(-2.75, height, -2.0f);

        GenericBalloon* ladderRightWallBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
        ladderRightWallBlock->SetCollider(true);
        ladderRightWallBlock->SetScale(Vector3(0.5, 1, 4));
        ladderRightWallBlock->SetAABB(Vector3(0.25f, 0.5f, 2), Vector3(-0.25f, -0.5f, -2));
        ladderRightWallBlock->InitLOD("high_res_cube", "med_res_cube", "");
        ladderRightWallBlock->SetPosition(spawnPos);
        CSceneNode* ladderRightWallNode = lowResNode->AddChild(ladderRightWallBlock);
        ladderRightWallNode->ApplyTranslate(2.75, height, -2.0f);
    }

    // Ladder Backwall
    GenericBalloon* ladderBackWallBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
    ladderBackWallBlock->SetCollider(true);
    ladderBackWallBlock->SetScale(Vector3(5, 5, 2));
    ladderBackWallBlock->SetAABB(Vector3(2, 2.5f, 1), Vector3(-2, -2.5f, -1));
    ladderBackWallBlock->InitLOD("high_res_cube", "med_res_cube", "");
    ladderBackWallBlock->SetPosition(spawnPos);
    CSceneNode* ladderBackWallNode = lowResNode->AddChild(ladderBackWallBlock);
    ladderBackWallNode->ApplyTranslate(0, 3, -5);
}

void SceneText::SpawnJumpCourse(Vector3 spawnPos)
{
    // ----------- LOW RES ----------- //
    // Jump Structure Low Res
    GenericBalloon* lowResJumpCourse = Create::Balloon("low_res_cube", Vector3(0, 0, 0));
    lowResJumpCourse->SetCollider(false);
    lowResJumpCourse->SetScale(Vector3(5.f, 4.f, 15.f));
    lowResJumpCourse->InitLOD("", "", "low_res_cube");
    lowResJumpCourse->SetLowResRender(true);
    lowResJumpCourse->SetPosition(spawnPos);
    // Add to Scene Graph
    CSceneNode* lowResNode = CSceneGraph::GetInstance()->AddNode(lowResJumpCourse);
    lowResNode->SetScale(3, 3, 3);

    // ----------- INDIVIDUAL PARTS ----------- //
    // Base Block / Floor of the course
    GenericBalloon* baseBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
    baseBlock->SetCollider(true);
    baseBlock->SetScale(Vector3(5, 1, 15));
    baseBlock->SetAABB(baseBlock->GetScale() * 0.5, -baseBlock->GetScale() * 0.5);
    baseBlock->InitLOD("high_res_cube", "med_res_cube", "");
    baseBlock->SetPosition(spawnPos);
    CSceneNode* baseNode = lowResNode->AddChild(baseBlock);
    baseNode->ApplyTranslate(0, 0, -2.5);
    
    for (int i = 0; i < 4; ++i)
    {
        // Left Wall
        GenericBalloon* leftWall = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
        leftWall->SetCollider(true);
        leftWall->SetScale(Vector3(1, 1, 3));
        leftWall->SetAABB(leftWall->GetScale() * 0.5, -leftWall->GetScale() * 0.5);
        leftWall->InitLOD("high_res_cube", "med_res_cube", "");
        leftWall->SetPosition(spawnPos);
        CSceneNode* leftWallNode = lowResNode->AddChild(leftWall);
        leftWallNode->ApplyTranslate(-2, 2, 3.5 - (i * 3));

        leftWall = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
        leftWall->SetCollider(true);
        leftWall->SetScale(Vector3(1, 1, 3));
        leftWall->SetAABB(leftWall->GetScale() * 0.5, -leftWall->GetScale() * 0.5);
        leftWall->InitLOD("high_res_cube", "med_res_cube", "");
        leftWall->SetPosition(spawnPos);
        leftWallNode = lowResNode->AddChild(leftWall);
        leftWallNode->ApplyTranslate(-2, 1, 3.5 - (i * 3));

        // Right Wall
        GenericBalloon* rightWall = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
        rightWall->SetCollider(true);
        rightWall->SetScale(Vector3(1, 1, 3));
        rightWall->SetAABB(rightWall->GetScale() * 0.5, -rightWall->GetScale() * 0.5);
        rightWall->InitLOD("high_res_cube", "med_res_cube", "");
        rightWall->SetPosition(spawnPos);
        CSceneNode* rightWallNode = lowResNode->AddChild(rightWall);
        rightWallNode->ApplyTranslate(2, 2, 3.5 - (i * 3));

        rightWall = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
        rightWall->SetCollider(true);
        rightWall->SetScale(Vector3(1, 1, 3));
        rightWall->SetAABB(rightWall->GetScale() * 0.5, -rightWall->GetScale() * 0.5);
        rightWall->InitLOD("high_res_cube", "med_res_cube", "");
        rightWall->SetPosition(spawnPos);
        rightWallNode = lowResNode->AddChild(rightWall);
        rightWallNode->ApplyTranslate(2, 1, 3.5 - (i * 3));
    }

    // Stairs (1st)
    for (int i = 0; i < 2; ++i)
    {
        GenericBalloon* stairsBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
        stairsBlock->SetCollider(true);
        stairsBlock->SetScale(Vector3(3, 1, 1));
        stairsBlock->SetAABB(stairsBlock->GetScale() * 0.5, -stairsBlock->GetScale() * 0.5);
        stairsBlock->InitLOD("high_res_cube", "med_res_cube", "");
        stairsBlock->SetPosition(spawnPos);
        CSceneNode* stairsBlockNode = lowResNode->AddChild(stairsBlock);
        stairsBlockNode->ApplyTranslate(0, 1 + i, -3.5);

        stairsBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
        stairsBlock->SetCollider(true);
        stairsBlock->SetScale(Vector3(3, 1, 1));
        stairsBlock->SetAABB(stairsBlock->GetScale() * 0.5, -stairsBlock->GetScale() * 0.5);
        stairsBlock->InitLOD("high_res_cube", "med_res_cube", "");
        stairsBlock->SetPosition(spawnPos);
        stairsBlockNode = lowResNode->AddChild(stairsBlock);
        stairsBlockNode->ApplyTranslate(0, 1 + i, -4.5);
    }

    // Stairs (2nd)
    for (int i = 0; i < 3; ++i)
    {
        GenericBalloon* stairsBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
        stairsBlock->SetCollider(true);
        stairsBlock->SetScale(Vector3(3, 1, 1));
        stairsBlock->SetAABB(stairsBlock->GetScale() * 0.5, -stairsBlock->GetScale() * 0.5);
        stairsBlock->InitLOD("high_res_cube", "med_res_cube", "");
        stairsBlock->SetPosition(spawnPos);
        CSceneNode* stairsBlockNode = lowResNode->AddChild(stairsBlock);
        stairsBlockNode->ApplyTranslate(0, 1 + i, -4.5);

        stairsBlock = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
        stairsBlock->SetCollider(true);
        stairsBlock->SetScale(Vector3(3, 1, 1));
        stairsBlock->SetAABB(stairsBlock->GetScale() * 0.5, -stairsBlock->GetScale() * 0.5);
        stairsBlock->InitLOD("high_res_cube", "med_res_cube", "");
        stairsBlock->SetPosition(spawnPos);
        stairsBlockNode = lowResNode->AddChild(stairsBlock);
        stairsBlockNode->ApplyTranslate(0, 1 + i, -5.5);
    }

    // Back Wall
    GenericBalloon* backWall = Create::Balloon("high_res_cube", Vector3(0, 0, 0));
    backWall->SetCollider(true);
    backWall->SetScale(Vector3(5, 5, 2));
    backWall->SetAABB(backWall->GetScale() * 0.5, -backWall->GetScale() * 0.5);
    backWall->InitLOD("high_res_cube", "med_res_cube", "");
    backWall->SetPosition(spawnPos);
    CSceneNode* backWallNode = lowResNode->AddChild(backWall);
    backWallNode->ApplyTranslate(0, 3, -9);
}

void SceneText::SpawnLamp(Vector3 spawnPos, CSceneNode* baseEntity)
{
    // Base
    GenericEntity* baseBlock = Create::Entity("high_res_lamppost", Vector3(0, 0, 0));
    baseBlock->SetCollider(true);
    baseBlock->SetScale(Vector3(5, 5, 5));
    baseBlock->SetAABB(Vector3(0.5f, 0.5f, 0.5f), Vector3(-0.5f, -0.5f, -0.5f));
    baseBlock->InitLOD("high_res_lamppost", "med_res_lamppost", "low_res_lamppost");

	CSceneNode* LampNode = baseEntity->AddChild(baseBlock);
	LampNode->ApplyTranslate(spawnPos.x, spawnPos.y, spawnPos.z);
}

void SceneText::SpawnTower(Vector3 spawnPos, CSceneNode* baseEntity)
{
    // Base
    GenericEntity* baseBlock = Create::Entity("high_res_tower", Vector3(0, 0, 0));
    baseBlock->SetCollider(true);
    baseBlock->SetScale(Vector3(4.f, 4.f, 4.f));
    baseBlock->SetAABB(Vector3(0.5f, 0.5f, 0.5f), Vector3(-0.5f, -0.5f, -0.5f));
    baseBlock->InitLOD("high_res_tower", "med_res_tower", "low_res_tower");

	CSceneNode* TowerNode = baseEntity->AddChild(baseBlock);
}

void SceneText::SpawnWall(Vector3 spawnPos, float rotate, CSceneNode* baseEntity)
{
    // Base
    GenericEntity* baseBlock = Create::Entity("castle_wall", Vector3(0, 0, 0));
    baseBlock->SetCollider(true);
    baseBlock->SetScale(Vector3(5, 5, 5));
    baseBlock->SetPosition(spawnPos);
    baseBlock->SetAABB(Vector3(0.5f, 0.5f, 0.5f), Vector3(-0.5f, -0.5f, -0.5f));
    baseBlock->InitLOD("castle_wall", "castle_wall", "castle_wall");

    if (baseEntity)
    {
        CSceneNode* WallNode = baseEntity->AddChild(baseBlock);
        WallNode->ApplyRotate(rotate, 0, 1, 0);
    }
}

void SceneText::SpawnCastle(Vector3 spawnPos)
{
	// ----------- LOW RES ----------- //
	// Castle Structure Low Res
	GenericEntity* lowResCastle = Create::Entity("low_res_cube", Vector3(0, 0, 0));
	lowResCastle->SetCollider(false);
	lowResCastle->SetScale(Vector3(5, 5, 5));
    lowResCastle->SetPosition(spawnPos);
	lowResCastle->InitLOD("", "", "low_res_cube");

	// Add to Scene Graph
	CSceneNode* lowResNode = CSceneGraph::GetInstance()->AddNode(lowResCastle);
    lowResCastle->SetLowResRender(true);

	// ----------- INDIVIDUAL PARTS ----------- //
	float offset = 5;

	//// Front Wall - Right
	//SpawnTower(Vector3(5, 0, 0) * offset, lowResNode);
	//SpawnWall(Vector3(8, 0, 0) * offset, 0, lowResNode);
	//SpawnWall(Vector3(13, 0, 0) * offset, 0, lowResNode);
	//
	//// Front - Right Connector
	//SpawnTower(Vector3(16, 0, 0) * offset, lowResNode);

	//// Front Wall - Left
	//SpawnTower(Vector3(-6, 0, 0) * offset, lowResNode);
	//SpawnWall(Vector3(-9, 0, 0) * offset, 0, lowResNode);
	//SpawnWall(Vector3(-14, 0, 0) * offset, 0, lowResNode);
	//
	//// Front - Left Connector
	//SpawnTower(Vector3(-17, 0, 0) * offset, lowResNode);

	//// Left Wall
	//SpawnWall(Vector3(-17, 0, -23) * offset, -90, lowResNode);
	//SpawnWall(Vector3(-17, 0, -18) * offset, -90, lowResNode);
	//SpawnWall(Vector3(-17, 0, -13) * offset, -90, lowResNode);
	//SpawnWall(Vector3(-17, 0, -8) * offset, -90, lowResNode);
	//SpawnWall(Vector3(-17, 0, -3) * offset, -90, lowResNode);

	//// Left - Back Connector
	//SpawnTower(Vector3(-17, 0, -13) * offset, lowResNode);

	//// Back Wall
	//SpawnWall(Vector3(-14, 0, -13) * offset, 180, lowResNode);
	//SpawnWall(Vector3(-9, 0, -13) * offset, 180, lowResNode);
	//SpawnTower(Vector3(-6, 0, -13) * offset, lowResNode);
	//SpawnWall(Vector3(-3, 0, -13) * offset, 180, lowResNode);
	//SpawnWall(Vector3(2, 0, -13) * offset, 180, lowResNode);
	//SpawnTower(Vector3(5, 0, -13) * offset, lowResNode);
	//SpawnWall(Vector3(8, 0, -13) * offset, 180, lowResNode);
	//SpawnWall(Vector3(13, 0, -13) * offset, 180, lowResNode);

	//// Back - Right Connector
	//SpawnTower(Vector3(16, 0, -13) * offset, lowResNode);

	//// Right Wall
	//SpawnWall(Vector3(16, 0, -23) * offset, 90, lowResNode);
	//SpawnWall(Vector3(16, 0, -18) * offset, 90, lowResNode);
	//SpawnWall(Vector3(16, 0, -13) * offset, 90, lowResNode);
	//SpawnWall(Vector3(16, 0, -8) * offset, 90, lowResNode);
	//SpawnWall(Vector3(16, 0, -3) * offset, 90, lowResNode);


	// Front Wall - Right
	SpawnTower(Vector3(4, 0, 0) * offset, lowResNode);
	SpawnWall(Vector3(9.5, 0, 0) * offset, 0, lowResNode);

	// Front - Right Connector
	SpawnTower(Vector3(15, 0, 0) * offset, lowResNode);

	// Right Wall
	SpawnWall(Vector3(15, 0, -5.5) * offset, 90, lowResNode);

	// Right - Back Connector
	SpawnTower(Vector3(15, 0, -10.5) * offset, lowResNode);

	// Front Wall - Left
	SpawnTower(Vector3(-5, 0, 0) * offset, lowResNode);
	SpawnWall(Vector3(-10.5, 0, 0) * offset, 0, lowResNode);

	// Front - Left Connector
	SpawnTower(Vector3(-16, 0, 0) * offset, lowResNode);

	// Left Wall
	SpawnWall(Vector3(-16, 0, -5.5) * offset, 90, lowResNode);

	// Left - Back Connector
	SpawnTower(Vector3(-16, 0, -10.5) * offset, lowResNode);

	// Back Wall
	SpawnWall(Vector3(9.5, 0, -10.5) * offset, 180, lowResNode);
	SpawnWall(Vector3(-0.5, 0, -10.5) * offset, 180, lowResNode);
	SpawnWall(Vector3(-10.5, 0, -10.5) * offset, 180, lowResNode);
}

void SceneText::SpawnWindmill(Vector3 spawnPos)
{
    // ----------- LOW RES ----------- //
    // Jump Structure Low Res
    GenericBalloon* lowResWindmill = Create::Balloon("low_res_cube", Vector3(0, 0, 0));
    lowResWindmill->SetCollider(false);
    lowResWindmill->SetScale(Vector3(5.f, 5.f, 5.f));
    lowResWindmill->InitLOD("", "", "");
    lowResWindmill->SetLowResRender(true);
    lowResWindmill->SetPosition(spawnPos);
    // Add to Scene Graph
    CSceneNode* lowResNode = CSceneGraph::GetInstance()->AddNode(lowResWindmill);
    lowResNode->SetScale(7, 7, 7);

    // ----------- INDIVIDUAL PARTS ----------- //
    // Base Block
    GenericEntity* baseBlock = Create::Asset("high_res_windmill_base", Vector3(0, 0, 0));
    baseBlock->SetCollider(true);
    baseBlock->SetScale(Vector3(1, 2.5, 1));
    baseBlock->SetAABB(baseBlock->GetScale() * 0.5, -baseBlock->GetScale() * 0.5);
    baseBlock->InitLOD("high_res_windmill_base", "low_res_windmill_base", "low_res_windmill_base");
    baseBlock->SetPosition(spawnPos);
    CSceneNode* baseNode = lowResNode->AddChild(baseBlock);

    // Rotator Block
    GenericEntity* rotatorBlock = Create::Asset("high_res_windmill_rotator", Vector3(0, 0, 0));
    rotatorBlock->SetCollider(true);
    rotatorBlock->SetScale(Vector3(1, 1, 1));
    rotatorBlock->SetAABB(rotatorBlock->GetScale() * 0.5, -rotatorBlock->GetScale() * 0.5);
    rotatorBlock->InitLOD("high_res_windmill_rotator", "low_res_windmill_rotator", "low_res_windmill_rotator");
    rotatorBlock->SetPosition(spawnPos);
    CSceneNode* rotatorNode = baseNode->AddChild(rotatorBlock);
    rotatorNode->ApplyTranslate(0, 5.5, 0);

    // Blades Block
    GenericEntity* bladesBlock = Create::Asset("high_res_windmill_blades", Vector3(0, 0, 0));
    bladesBlock->SetCollider(true);
    bladesBlock->SetScale(Vector3(0.8, 0.8, 0.8));
    bladesBlock->SetAABB(bladesBlock->GetScale() * 0.5, -bladesBlock->GetScale() * 0.5);
    bladesBlock->InitLOD("high_res_windmill_blades", "low_res_windmill_blades", "low_res_windmill_blades");
    bladesBlock->SetPosition(spawnPos);
    CSceneNode* bladesNode = rotatorNode->AddChild(bladesBlock);
    bladesNode->ApplyTranslate(0, 0, 1.2);

    CUpdateTransformation* bladesRotate = new CUpdateTransformation();
    bladesRotate->ApplyUpdate(1.0f, 0.0f, 0.0f, 1.0f);
    bladesRotate->SetSteps(-720, 720);
    bladesNode->SetUpdateTransformation(bladesRotate);
}

void SceneText::SpawnDiamond(const Vector3& spawnPos)
{
    // Base
    CPickup* diamond = Create::Pickup("high_res_diamond", "Diamond", spawnPos);
    diamond->SetCollider(true);
    Vector3 scale(2.5f, 2.5f, 2.5f);
    diamond->SetScale(scale);
    diamond->SetPosition(spawnPos);
    diamond->SetAABB(Vector3(2.5f, 2.5f, 2.5f), Vector3(-2.5f, -2.5f, -2.5f));
    diamond->InitLOD("high_res_diamond", "med_res_diamond", "low_res_diamond");
}

void SceneText::SpawnAmmo(const Vector3& spawnPos)
{
    // Base
    CPickup* ammo = Create::Pickup("high_res_ammo", "Ammo", spawnPos);
    ammo->SetCollider(true);
    Vector3 scale(2.5f, 2.5f, 2.5f);
    ammo->SetScale(scale);
    ammo->SetPosition(spawnPos);
    ammo->SetAABB(Vector3(2.5f, 2.5f, 2.5f), Vector3(-2.5f, -2.5f, -2.5f));
    ammo->InitLOD("high_res_ammo", "med_res_ammo", "low_res_ammo");
}