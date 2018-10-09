
#include "GLFWFramework.h"
#include "dx11Device.h"
#include "RenderData.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include <d3dcompiler.h>
#include "RenderDef.h"
//#include "json.hpp"
//using json = nlohmann::json;


ID3D11Buffer* mBoxVB;
ID3D11Buffer* mBoxIB;

//ID3DX11Effect* mFX;
//ID3DX11EffectTechnique* mTech;
//ID3DX11EffectMatrixVariable* mfxWorldViewProj;

ID3D11InputLayout* mInputLayout;

XMFLOAT4X4 mWorld;
XMFLOAT4X4 mView;
XMFLOAT4X4 mProj;
ID3D11Device* md3dDevice;
ID3D11DeviceContext* md3dContent;
struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};
void BuildGeometryBuffers()
{
	// Create vertex buffer
	Vertex vertices[] =
	{
		{ XMFLOAT3(-0.5f, -0.5f, -0.0f), (XMFLOAT4)(const float*)&Colors::White },
		{ XMFLOAT3(-0.5f, +0.5f, -0.0f), (XMFLOAT4)(const float*)&Colors::Black },
		{ XMFLOAT3(+0.5f, +0.5f, -0.0f), (XMFLOAT4)(const float*)&Colors::Red },
		{ XMFLOAT3(+0.5f, -0.5f, -0.0f), (XMFLOAT4)(const float*)&Colors::Green },
		{ XMFLOAT3(-0.5f, -0.5f, +0.0f), (XMFLOAT4)(const float*)&Colors::Blue },
		{ XMFLOAT3(-0.5f, +0.5f, +0.0f), (XMFLOAT4)(const float*)&Colors::Yellow },
		{ XMFLOAT3(+0.5f, +0.5f, +0.0f), (XMFLOAT4)(const float*)&Colors::Cyan },
		{ XMFLOAT3(+0.5f, -0.5f, +0.0f), (XMFLOAT4)(const float*)&Colors::Magenta }
	};

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * 8;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mBoxVB));


	// Create the index buffer

	UINT indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 36;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mBoxIB));
}
ID3DBlob* vsBlob;
ID3DBlob* psBlob;
ID3D11VertexShader* vs;
ID3D11PixelShader* ps;
void BuildFX()
{
//	DWORD shaderFlags = 0;
//#if defined( DEBUG ) || defined( _DEBUG )
//	shaderFlags |= D3D10_SHADER_DEBUG;
//	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
//#endif
//
//	ID3D10Blob* compiledShader = 0;
//	ID3D10Blob* compilationMsgs = 0;
//	HRESULT hr = D3DX11CompileFromFile("FX/color.fx", 0, 0, 0, "fx_5_0", shaderFlags,
//		0, 0, &compiledShader, &compilationMsgs, 0);
//
//	// compilationMsgs can store errors or warnings.
//	if (compilationMsgs != 0)
//	{
//		MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
//		ReleaseCOM(compilationMsgs);
//	}
//
//	// Even if there are no compilationMsgs, check to make sure there were no other errors.
//	if (FAILED(hr))
//	{
//	}
//
//	HR(D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(),
//		0, md3dDevice, &mFX));
//
//	// Done with compiled shader.
//	ReleaseCOM(compiledShader);
//
//	mTech = mFX->GetTechniqueByName("ColorTech");
//	mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();

	D3DCompileFromFile(L"shader/DX/vs.hlsl", NULL, NULL,
		"main", "vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, NULL, &vsBlob, NULL);
	D3DCompileFromFile(L"shader/DX/ps.hlsl", NULL, NULL,
		"main", "ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, NULL, &psBlob, NULL);

	md3dDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), NULL, &vs);
	md3dDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), NULL, &ps);



	// Create the vertex input layout.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Create the input layout
	//	D3DX11_PASS_DESC passDesc;
	//mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	md3dDevice->CreateInputLayout(vertexDesc, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &mInputLayout);
}
struct CBPerFrameData

{

	XMMATRIX worldViewProj;

} cb;
ID3D11Buffer* CBuffer;
void buildConstantBuffer(){
	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = static_cast<uint32_t>(sizeof(CBPerFrameData) );

	md3dDevice->CreateBuffer(&desc, nullptr, &CBuffer);
}
void Render(){
	//md3dContent->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	//md3dContent->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dContent->IASetInputLayout(mInputLayout);
	md3dContent->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	md3dContent->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);
	md3dContent->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);
	md3dContent->VSSetShader(vs, NULL, NULL);
	md3dContent->PSSetShader(ps, NULL, NULL);
	// Set constants
	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX worldViewProj = world*view*proj;


	/*cb.worldViewProj = worldViewProj;
	md3dContent->UpdateSubresource(CBuffer, 0, nullptr, &cb, 0, 0);
	md3dContent->VSSetConstantBuffers(0, 1, &CBuffer);*/
	//mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));



		// 36 indices for the box.
	md3dContent->DrawIndexed(3, 0, 0);
	

}

int main() {


	GLFWFrameWork fw;
	fw.Init();

	DX11Device device;
	device.InitDevice(fw.hwnd);
	md3dDevice = device.g_pd3dDevice;
	md3dContent = device.g_pImmediateContext;

	D3D11_VIEWPORT viewport = { 0 };

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = 1600;
	viewport.Height = 900;

	md3dContent->RSSetViewports(1, &viewport);
	// Setup ImGui binding
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	ImGui_ImplDX11_Init(fw.window, true,device.g_pd3dDevice, device.g_pImmediateContext);

	// Setup style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Load Fonts
	//io.Fonts->AddFontDefault();
	ImFont* font = io.Fonts->AddFontFromFileTTF("Roboto-Medium.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//auto font2=io.Fonts->AddFontFromFileTTF("Roboto-Medium.ttf", 16.0f);
	IM_ASSERT(font != NULL);
	 
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	BuildGeometryBuffers();
	BuildFX();
	buildConstantBuffer();
	//BuildVertexLayout();

	while (!glfwWindowShouldClose(fw.window)) {
		glfwPollEvents();
		device.Render();
		Render();

		ImGui_ImplDX11_NewFrame();

		// 1. Show a simple window.
		// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
		{
			static float f = 0.0f;
			static int counter = 0;
			ImGui::Text("Hello, world!");                           // Display some text (you can use a format string too)
			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our windows open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}

		// 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name your windows.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}

		// 3. Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
		if (show_demo_window)
		{
			ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
			ImGui::ShowDemoWindow(&show_demo_window);
		}

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		device.SwapBuffer();
		//glfwSwapBuffers(fw.window);

	}

	ImGui_ImplDX11_Shutdown();
	ImGui::DestroyContext();

	device.CleanupDevice();
	glfwTerminate();
	return 0;
}