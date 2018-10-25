#include "stdafx.h"
#include "ModelEditorApp.h"
#include "imgui.h"
#include "EditModel.h"
#include "direct.h"
#include "ModelToRender.h"
//CD3d g_D3d;
ModelEditorApp g_app;

LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
D3DPRESENT_PARAMETERS    g_d3dpp;
CEditModel* g_Model;

void ModelEditorApp::Init()
{
	_chdir(WorkDirectory.c_str());
	g_Model = new CEditModel();
	ResetView();

}

//void ReadFileToString(string path, string& out)
//{
//	std::ifstream t(path);
//	std::stringstream buffer;
//	buffer << t.rdbuf();
//	out = buffer.str();
//}
//
////json test
//void ReadConfig()
//{
//	json j;
//	ifstream file("ModelEditorConfig.txt");
//	stringstream buffer;
//	if (file) {
//		try
//		{
//			buffer << file.rdbuf();
//			j = json::parse(buffer.str());
//			cout << j.dump(4) << endl;
//		}
//		catch (json::parse_error& e)
//		{
//			// output exception information
//			std::cout << "message: " << e.what() << '\n'
//				<< "exception id: " << e.id << '\n'
//				<< "byte position of error: " << e.byte << std::endl;
//		}
//	}
//	else {
//		cout << "read config error" << endl;
//	}
//	return;
//}


void ModelEditorApp::GenerateGui()
{
	ImGui::NewFrame();
	//GenMainMenu();
	GenDemoGui();
	GenTestUI();

	// Rendering
	ImGui::EndFrame();
}

void ModelEditorApp::GenMainMenu()
{
	static bool opt_fullscreen_persistant = true;
	static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None
		| ImGuiDockNodeFlags_PassthruDockspace;

	bool opt_fullscreen = opt_fullscreen_persistant;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (opt_flags & ImGuiDockNodeFlags_PassthruDockspace)
		ImGui::SetNextWindowBgAlpha(0.0f);
	bool open = true;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &open, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// Dockspace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
	}
	else
	{
		 //ShowDockingDisabledMessage();
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Docking"))
		{
			// Disabling fullscreen would allow the window to be moved to the front of other windows, 
			// which we can't undo at the moment without finer window depth/z control.
			//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

			if (ImGui::MenuItem("test", "", (opt_flags & ImGuiDockNodeFlags_NoSplit) != 0))                opt_flags ^= ImGuiDockNodeFlags_NoSplit;
			//if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (opt_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0)) opt_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
			if (ImGui::MenuItem("Save As..")) {}
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::End();
}

void ModelEditorApp::GenDemoGui()
{
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window) {
		ImGui::ShowDemoWindow(&show_demo_window);

	}
	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
																//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}
}

void ModelEditorApp::GenTestUI()
{
	ImGui::Begin("LoadFile");
	strcpy(path, tmpFile.c_str());
	ImGui::InputTextMultiline("path", path, 255);
	tmpFile = string(path);
	auto s=fs::current_path().string();
	uint32_t memAvailable = g_rh.GetAvailableTextureMem();
	string text = u8"当前路径：" + s + u8" 可用显存: " + to_string(memAvailable);
	ImGui::Text(text.c_str());
	if (ImGui::Button(u8"加入模型")){
		scene.AddModel(path);
	}
	ImGui::InputInt(u8"渲染方式", &g_miniRender.config.pipelineType);
	

	GenLightUI();
	ImGui::End();
}
 
void ModelEditorApp::GenLightUI()
{
	D3DLIGHT9* light = &scene.DirLight;
	ImGui::ColorEdit4("Light Diffuse", (float*)&light->Diffuse);
	ImGui::ColorEdit4("Light Specular", (float*)&light->Specular);
	ImGui::ColorEdit4("Light Ambient", (float*)&light->Ambient);
	ImVec4 color=ImGui::ColorConvertU32ToFloat4(scene.m_AmbientColor);
	ImGui::ColorEdit4("Ambient Color", (float*)&color);
	scene.m_AmbientColor = ImGui::ColorConvertFloat4ToU32(color);
	ImGui::DragFloat3("Light Pos", (float*)&light->Position);
	ImGui::DragFloat3("Light Dir", (float*)&light->Direction);
}

void ModelEditorApp::RenderModel()
{
	auto& Model = *g_Model;
	//float CurF = Model.GetCurFrameNum() + DeltaTime*Model.GetFPS() / 1000;
	//CurF = m_FrameCtrl.SetFrameNum(CurF);
	//CurTime = (DWORD)(CurF * 1000 / Model.GetFPS());
	D3DXMATRIX world,view,proj;
	D3DXMatrixIdentity(&world);
	
	view = CCamera::GetInstance()->GetView();
	proj = CCamera::GetInstance()->GetProj();
	auto& device = g_rh.device;
	device->SetTransform(D3DTS_VIEW, &view);
	device->SetTransform(D3DTS_PROJECTION, &proj);

	SetSceneLight();

	DWORD CurTime = GetTickCount();
	Model.SetCurrentTime(-1);
	Model.SetCurrentFrame(-0.03f);
	if (Model.GetRenderMatrix().m_CurrentTime != CurTime)
	{
		Model.CalculateMatrix(world, CurTime - Model.GetRenderMatrix().m_CurrentTime);
		Model.GetRenderMatrix().m_CurrentTime = CurTime;
	}
	
	RenderInfo2 info;
	EditModelToRenderInfo(g_Model, info);
	g_miniRender.RenderWithConfig(info);

}

void ModelEditorApp::SetSceneLight()
{
	auto& device = g_rh.device;
	device->SetLight(0, &scene.DirLight);
	device->LightEnable(0, TRUE);
	device->SetRenderState(D3DRS_AMBIENT, scene.m_AmbientColor);
}

void ModelEditorApp::ResetView()
{
	m_CameraPos = D3DXVECTOR3(0, 250, 600);
	m_CameraAt = D3DXVECTOR3(0, 0, 0);
	CCamera::GetInstance()->Perspective(0.47f, ((FLOAT)width) / height, 50.0f, 10000.0f);
	CCamera::GetInstance()->LookAt(m_CameraPos, m_CameraAt);
}

void Scene::AddModel(string path)
{
	ModelListItem item;
	CPieceGroup* PieceGroup = new CPieceGroup;
	item.model = PieceGroup;
	if (FAILED(PieceGroup->BinLoad(g_pd3dDevice, (LPCTSTR)path.c_str(), NULL)))
	{
		delete PieceGroup;
		return;
	}
	item.name = GetPureName(path);
	ModelList.push_back(item);

	for (int i = 0; i < PieceGroup->GetPieceNum(); i++)
	{
		CPiece* Pc = PieceGroup->GetPiece(i);
		CRenderPiece RP(*Pc, 0);
		g_Model->AddMeshPiece(RP);
	}
}
