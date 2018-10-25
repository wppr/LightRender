#pragma once
#include "miniDX9Render.h"
#include "Camera.h"
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

class ModelEditorApp;
class CEditModel;
extern ModelEditorApp g_app;
extern CEditModel* g_Model;
extern LPDIRECT3DDEVICE9        g_pd3dDevice;
extern D3DPRESENT_PARAMETERS    g_d3dpp;


void ReadFileToString(string path, string& out);

class CPieceGroup;


struct ModelListItem {
	string name;
	CPieceGroup* model;
};

struct Scene {
	D3DXVECTOR3	m_SunVec;
	DWORD	m_SunColor;
	DWORD	m_AmbientColor;
	DWORD	m_FogColor;
	D3DLIGHT9 DirLight;

	Scene() {
		m_FogColor = 0xffcccc88;

		m_SunVec = D3DXVECTOR3(0.4f, 0.05f, -1.0f);
		m_SunColor = 0xFFDDDDDD;
		m_AmbientColor = 0xFF888888;
		InitLight();
	}

	void InitLight() {


		DirLight.Type = D3DLIGHT_DIRECTIONAL;
		DirLight.Direction = m_SunVec;
		D3DXCOLOR sun_color(m_SunColor);
		DirLight.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

		DirLight.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
		DirLight.Attenuation0 = 1;
		DirLight.Range = 1000;
		DirLight.Falloff = 0;
		DirLight.Attenuation0 = 1;
		DirLight.Attenuation1 = 0;
		DirLight.Attenuation2 = 0;

		D3DXVec3Normalize((D3DXVECTOR3*)&DirLight.Direction, (D3DXVECTOR3*)&DirLight.Direction);


	}

	string GetPureName(string _path) {
		return fs::path(_path).filename().string();
		// return p.filename().string();
	}

	void AddModel(string path);
	//·â×°
	vector<ModelListItem> ModelList;
};




class ModelEditorApp {
public:
	string WorkDirectory = "F:/artist/data/";
	void Init();


	void GenerateGui();
	void GenMainMenu();
	void GenDemoGui();
	void GenTestUI();
	void GenLightUI();

	void RenderModel();
	void SetSceneLight();
	void ResetView();

	Scene scene;
	bool show_demo_window = true;
	bool show_another_window = false;
	int width = 1280;
	int height = 800;

	D3DXVECTOR3 m_CameraPos;
	D3DXVECTOR3 m_CameraAt;

	string tmpFile = "F:\\artist\\data\\character\\model\\fashion\\gnpc_dknnew_branch_mantuoluohua.arp";
	char path[255];
};