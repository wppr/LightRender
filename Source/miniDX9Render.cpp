#include "stdafx.h"
#include "miniDX9Render.h"
#include <iostream>
#include "D3dd.h"
using namespace std;


miniDX9Render g_miniRender;
DX9RenderHardware g_rh;
map<void*, RenderBuffer> g_buffers;
BufferMgr bufferMgr;
App g_app;


void ReadFileToString(string path, string& out)
{
	std::ifstream t(path);
	std::stringstream buffer;
	buffer << t.rdbuf();
	out = buffer.str();
}

//json test
void ReadConfig()
{
	json j;
	ifstream file("ModelEditorConfig.txt");
	stringstream buffer;
	if (file) {
		try
		{
			buffer << file.rdbuf();
			j = json::parse(buffer.str());
			cout << j.dump(4) << endl;
		}
		catch (json::parse_error& e)
		{
			// output exception information
			std::cout << "message: " << e.what() << '\n'
				<< "exception id: " << e.id << '\n'
				<< "byte position of error: " << e.byte << std::endl;
		}
	}
	else {
		cout << "read config error" << endl;
	}
	return;
}






// helper function
#include "PieceGroup.h"
//output:info
void EditModelToRenderInfo(CEditModel* model, RenderInfo2& infoList)
{
	//const D3DXMATRIX *pMat = NULL;
	//auto& m_pPhysObj = model->m_pPhysObj;
	//if (m_pPhysObj&&m_pPhysObj->GetType() == PHYSICS_OBJ_TYPE::POT_EXPLOSE)
	//{
	//	pMat = m_pPhysObj->CalculateMatrix();
	//}
	//int id = 0;

	auto& m_RenderMatrix = model->m_RenderMatrix;


	for (auto renderPiece : model->GetMeshsPiece())
	{
		//D3DXMATRIX orgMat = model->m_RenderMatrix.m_MatrixUnit[0].m_GlobeSkeletalMatrix;

		//if (m_pPhysObj && m_pPhysObj->GetType() == PHYSICS_OBJ_TYPE::POT_EXPLOSE && pMat)
		//{
		//	m_RenderMatrix.m_MatrixUnit[0].m_CurSkeletalMatrix = m_RenderMatrix.m_MatrixUnit[0].m_GlobeSkeletalMatrix = pMat[id];
		//	++id;
		//}
		//if (m_bHideUnselected == false || m_SelectPiece == (*it).first)
		//	(*it).second.Render(Lengh, m_RenderMatrix, AlphaBlend, pD3dDev, TRUE, true);
		//if (m_SelectPiece == (*it).first && !m_bHideWireframe)
		//{
		//	CRenderStyle::Set(pD3dDev, m_SelectShade);
		//	(*it).second.Render(Lengh, m_RenderMatrix, AlphaBlend, pD3dDev, FALSE);
		//}
		auto piece = renderPiece.second.m_Piece;
		RenderInfo info;
		//m_Piece->Render(Dist, this, ModelMatrix, CalCloth, shader_type);
		PieceToRenderInfo(piece, info, m_RenderMatrix, true,&renderPiece.second);
		infoList.objInfoList.push_back(std::move(info));
	}
}


void StaticRSToRenderState(const STATIC_RS& rs, CombiendRenderState& state) {

	state.textureList.Textures[0] = rs.m_Texture_S0;
	state.textureList.Textures[1] = rs.m_Texture_S1;
	state.textureList.Textures[2] = rs.m_Texture_S2;
	state.textureList.Textures[3] = rs.m_Texture_S3;

	state.fixdRs.material = rs.m_Material;
	//state.fixdRs.FVF=rs
	state.fixdRs.Texturefactor = rs.m_Texturefactor;
	state.fixdRs.SpecularEnable = rs.m_SpecularEnable;
	state.fixdRs.LightEnable = rs.m_LightEnable;
	
	state.rs.rasterizeState.CullMode = rs.m_Cull;
	state.rs.rasterizeState.FillMode = rs.m_FillMode;

	state.rs.depthStencilState.ZWrite = rs.m_ZWR;
	state.rs.depthStencilState.ZTestEnable = rs.m_ZTestEnable;
	state.rs.depthStencilState.ZTestFunc = rs.m_ZTestFun;

	state.rs.blendState.AlphaRef = rs.m_Alpharef;
	state.rs.blendState.AlphaTestEnable = rs.m_AlphaTestEnable;
	state.rs.blendState.AlphaTestFunc = rs.m_AlphaTestFun;
	state.rs.blendState.AlphaBlendEnable = rs.m_AlphaBlendEnable;
	state.rs.blendState.SrcBlend = rs.m_SrcBlend;
	state.rs.blendState.DestBlend = rs.m_DestBlend;


	{
		int i = 0;
		auto& textureState = state.fixdRs.textureStageState.States[i];
		textureState.ColorOp = rs.m_TexOP_S0;
		textureState.ColorArg1 = rs.m_TexArg1_S0;
		textureState.ColorArg2 = rs.m_TexArg2_S0;
		textureState.AlphaOp = rs.m_AlpOP_S0;
		textureState.AlphaArg1 = rs.m_AlpArg1_S0;
		textureState.AlphaArg2 = rs.m_AlpArg2_S0;
	}
	{
		int i = 1;
		auto& textureState = state.fixdRs.textureStageState.States[i];
		textureState.ColorOp = rs.m_TexOP_S1;
		textureState.ColorArg1 = rs.m_TexArg1_S1;
		textureState.ColorArg2 = rs.m_TexArg2_S1;
		textureState.AlphaOp = rs.m_AlpOP_S1;
		textureState.AlphaArg1 = rs.m_AlpArg1_S1;
		textureState.AlphaArg2 = rs.m_AlpArg2_S1;
	}
	{
		int i = 2;
		auto& textureState = state.fixdRs.textureStageState.States[i];
		textureState.ColorOp = rs.m_TexOP_S2;
		textureState.ColorArg1 = rs.m_TexArg1_S2;
		textureState.ColorArg2 = rs.m_TexArg2_S2;
		textureState.AlphaOp = rs.m_AlpOP_S2;
		textureState.AlphaArg1 = rs.m_AlpArg1_S2;
		textureState.AlphaArg2 = rs.m_AlpArg2_S2;
	}
	{
		int i = 3;
		auto& textureState = state.fixdRs.textureStageState.States[i];
		textureState.ColorOp = rs.m_TexOP_S3;
		textureState.ColorArg1 = rs.m_TexArg1_S3;
		textureState.ColorArg2 = rs.m_TexArg2_S3;
		textureState.AlphaOp = rs.m_AlpOP_S0;
		textureState.AlphaArg1 = rs.m_AlpArg1_S3;
		textureState.AlphaArg2 = rs.m_AlpArg2_S3;
	}


}

VOID ConvertLightToViewSpace(D3DVECTOR &dir, D3DXMATRIXA16 *pMatView)
{
	D3DXVECTOR3 lightDir(dir.x, dir.y, dir.z);
	D3DXVec3TransformNormal(&lightDir, &lightDir, pMatView);
	D3DXVec3Normalize(&lightDir, &lightDir);
	dir.x = -lightDir.x;
	dir.y = -lightDir.y;
	dir.z = -lightDir.z;
}
extern CD3d g_D3d;
void SetGlobalSkinShaderParam(IDirect3DDevice9* m_pDevice, ID3DXEffect* m_pSkinEffect) {

	D3DXMATRIX World, View, Proj;
	D3DXMatrixIdentity(&World);
	m_pDevice->GetTransform(D3DTS_VIEW, &View);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &Proj);
	int boneCount = 4;

	D3DXMATRIXA16 combinedMat;
	D3DXCOLOR globalAmbient(0xffffffff);
	D3DXCOLOR AmbientColor(g_D3d.GetAmbientColor());
	D3DXCOLOR DiffuseColor(g_D3d.GetSunLightColor());
	D3DXVECTOR3 lightDir = g_D3d.GetSunLightDir();

	m_pSkinEffect->SetMatrix("matWorld", &World);
	m_pSkinEffect->SetMatrix("matView", &View);

	D3DXMatrixMultiply(&combinedMat, &World, &View);
	m_pSkinEffect->SetMatrix("matWorldView", &combinedMat);

	ConvertLightToViewSpace(lightDir, &combinedMat);

	D3DXMatrixMultiply(&combinedMat, &combinedMat, &Proj);
	m_pSkinEffect->SetMatrix("mGlobalWorldViewProjMatrix", &combinedMat);

	m_pSkinEffect->SetInt("CurNumBones", boneCount);

	D3DXVECTOR4 white = D3DXVECTOR4(1, 1, 1, 1);
	D3DXVECTOR4 black = D3DXVECTOR4(0, 0, 0, 0);


	m_pSkinEffect->SetVector("globalAmbient", (D3DXVECTOR4*)&AmbientColor);

	m_pSkinEffect->SetVector("vMaterialAmbiColor", (D3DXVECTOR4*)&white);
	m_pSkinEffect->SetVector("vMaterialDiffColor", (D3DXVECTOR4*)&white);
	m_pSkinEffect->SetVector("vMaterialEmisColor", (D3DXVECTOR4*)&black);

	int type = D3DLIGHT_DIRECTIONAL;
	m_pSkinEffect->SetValue("lights[0].iType", (void *)&type, sizeof(int));

	// 将光线方向换算到view space优化shader程序

	m_pSkinEffect->SetValue("lights[0].vDir", (void*)&lightDir.x, sizeof(D3DVECTOR));

	m_pSkinEffect->SetValue("lights[0].vAmbient", (void*)&black, sizeof(D3DCOLORVALUE));

	// 合并light diffuse 和 material diffuse 优化shader 程序			
	m_pSkinEffect->SetValue("lights[0].vDiffuse", (void*)&DiffuseColor, sizeof(D3DCOLORVALUE));

	m_pSkinEffect->SetValue("lights[0].vSpecular", (void*)&DiffuseColor, sizeof(D3DCOLORVALUE));
	//m_pSkinEffect->SetBool(m_hSpecular, TRUE);


	// 7. 设置贴图坐标偏移矩阵
	m_pSkinEffect->SetInt("nTexUVTransStage", 0);

	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
	m_pSkinEffect->SetMatrix("matUVTransform", &mat);

	//m_pSkinEffect->SetTechnique(m_hCurHLSL);
	//m_pDevice->SetVertexDeclaration(m_pDecl);


	DWORD TFactor;
	m_pDevice->GetRenderState(D3DRS_TEXTUREFACTOR, &TFactor);
	D3DXVECTOR4 vecTFactor(float((TFactor & 0xFF0000) >> 16) / 255.0f, float((TFactor & 0xFF00) >> 8) / 255.0f, float(TFactor & 0xFF) / 255.0f, float((TFactor & 0xFF000000) >> 24) / 255.0f);
	m_pSkinEffect->SetVector("TFactor", &vecTFactor);
}

//out:ShaderParam
void RSToShaderParam(const vector<CTexture*>& TextureList, const STATIC_RS &RS, ShaderState& param) {
	if (RS.m_Shader == "") {
		param.HasShader = false;
		return;
	}
	param.HasShader = true;
	param.effect = "skinAnimation2";
	map<string, LPDIRTEX> texMap;
	map<string, float> varMap;
	CIniFile iniFile;
	iniFile.OpenBuffer((char*)RS.m_Shader.c_str());

	string technique;
	char value[64];
	iniFile.GetString("Shader", "Technique", "default", value, 64);
	DWORD pass = iniFile.GetInteger("Shader", "Pass", 0);

	technique = value;
	char key[64];
	uint idx = 0;
	bool bContinue = true;
	while (bContinue)
	{
		bContinue = iniFile.GetKeyValue("Texture", key, value, 64, idx);
		if (bContinue)
		{
			int TexIdx = atoi(value);
			CTexture *pTex = TexIdx < (uint)TextureList.size() ? TextureList[TexIdx] : NULL;
			if (pTex)
			{
				string name = key;
				texMap[name] = (IDirect3DTexture9*)pTex->GetD3DTexture();
			}
		}
		idx++;
	}

	idx = 0;
	bContinue = true;
	while (bContinue)
	{
		bContinue = iniFile.GetKeyValue("Variable", key, value, 64, idx);
		if (bContinue)
		{
			string name = key;
			varMap[name] = atof(value);
		}
		idx++;
	}
	varMap["fMaterialPower"] = RS.m_Material.Power;
	param.colorMap["vMaterialSpecColor"] = RS.m_Material.Specular;

	iniFile.Close();
	param.technique = technique;
	param.pass = pass;
	param.varMap = varMap;
	param.texMap = texMap;

	//SetShaderParam(&texMap, &varMap, RS.m_Material.Specular, RS.m_Material.Power, ms_RenderStyle.m_SpecularEnable);
	
}
//output: info
void PieceToRenderInfo(CPiece* piece, RenderInfo& info, CRenderMatrix& ModelMatrix,bool CalCloth, CRenderPiece* pRP)
{
	auto m_pD3dDev = g_rh.device;
	auto m_Mesh = piece->GetMesh();
	auto m_bHighRes = piece->m_bHighRes;

	DWORD Format;
	LPVERBUF VB = NULL;
	LPINDBUF IB = NULL;

	//初始化BUF大小
	static UINT VerBufSize = 165536;
	static char* RenderBuf = new char[VerBufSize];
	static UINT IndBufSize = 165536;
	 

	int Level = 0;
	int VerNum = m_Mesh->GetVerNum(Level);
	UINT VBSize = 0;

	VBSize = VerNum * sizeof(VerNorTex);
	Format = VerNorTex::Format;


	int IndNum = m_Mesh->GetFaceNum(Level);
	UINT IBSize = IndNum * 6;

	//当BUF太小重新创建
	if (VBSize > VerBufSize)
	{
		while (VBSize > VerBufSize)
			VerBufSize = VerBufSize << 1;
		delete[] RenderBuf;
		RenderBuf = new char[VerBufSize];
		SAFE_RELEASE(VB);
	}

	if (IBSize > IndBufSize)
	{
		while (IBSize > IndBufSize)
			IndBufSize = IndBufSize << 1;
		SAFE_RELEASE(IB);
	}
	
	RenderBuffer* rb = bufferMgr.GetBuffer(piece);
	if (!rb)
		rb = bufferMgr.CreateRenderBuffer(piece, VerBufSize, IndBufSize, Format);

	VB = rb->vb;
	IB = rb->ib;
	//if (g_buffers.find(piece) != g_buffers.end()) {
	//	VB = g_buffers[piece].vb;
	//	IB = g_buffers[piece].ib;
	//}
	//else {
	//	printf("alloc buffer");
	//	RenderBuffer rb;
	//	if (m_pD3dDev && !VB)
	//		m_pD3dDev->CreateVertexBuffer(VerBufSize, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, Format, D3DPOOL_DEFAULT, &VB, NULL);

	//	if (m_pD3dDev && !IB)
	//		m_pD3dDev->CreateIndexBuffer(IndBufSize, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &IB, NULL);
	//	rb.ib = IB;
	//	rb.vb = VB;
	//	g_buffers[piece] = rb;
	//}


	D3DXMATRIX& matWorld = ModelMatrix.m_MatrixUnit[0].m_CurSkeletalMatrix;

	//软件计算顶点
	m_Mesh->CalMeshVertex(m_bHighRes, Level, RenderBuf, VerNorTex::Format, pRP, sizeof(VerNorTex), ModelMatrix, CalCloth);
	D3DXMatrixIdentity(&matWorld);
	void* Buf;

	VB->Lock(0, 0, (void**)&Buf, D3DLOCK_DISCARD);
	memcpy(Buf, RenderBuf, VBSize);
	VB->Unlock();

	void* pSysIB = m_Mesh->GetIndexBuf(Level);
	if (pSysIB)
	{
		IB->Lock(0, 0, (void**)&Buf, D3DLOCK_DISCARD);
		memcpy(Buf, pSysIB, IBSize);
		IB->Unlock();
	}

	CPieceRenderStyle* pieceRS=piece->GetRS(0);
	if (pieceRS) {
		STATIC_RS rs;
		auto rs_time = pieceRS->GetShade();
		rs_time.GetCurRenderStyle(rs, pieceRS->m_Texture, ModelMatrix.m_CurFrame*33.33f, NULL);
		StaticRSToRenderState(rs, info.renderState);

		RSToShaderParam(pieceRS->m_Texture,rs,info.shaderState);
	}

	info.VertexBuffer = VB;
	info.IndexBuffer = IB;
	info.WorldMatrix = matWorld;
	info.drawParam.FromIndexTriangle(VerNum, IndNum);
}


#include "BoxObject.h"
CBoxObject box;
void miniDX9Render::Init(LPDIRECT3DDEVICE9 pDevice)
{
	//m_pD3d = NULL;
	//m_pDevice = pDevice;
	if (Inited) return;
	rh = &g_rh;
	rh->device = pDevice;
	bufferMgr.device = pDevice;
	effectMgr.Init(pDevice);
	rtMgr.Init(pDevice);
	width = 800;
	height = 600;
	Inited = true;

	//init effect 
	fxaa.Init();
	//box.Init(rh->device);
}


void miniDX9Render::RenderFixed(RenderInfo2& info)
{
	vector<RenderInfo*> opaqueDrawIndexs;
	vector<RenderInfo*> alphaDrawIndexs;
	FilterAlphaObject(info, opaqueDrawIndexs, alphaDrawIndexs);
	for (auto& x : info.objInfoList) {
		x.shaderState.HasShader = false;
	}
	for (auto obj : opaqueDrawIndexs) {
		Render(*obj);
	}

	for (auto obj : alphaDrawIndexs) {
		Render(*obj);
	}
}

void miniDX9Render::Render(RenderInfo& info)
{
	auto pDevice = g_rh.device;
	pDevice->SetTransform(D3DTS_WORLD, &info.WorldMatrix);
	pDevice->SetFVF(VerNorTex::Format);
	pDevice->SetStreamSource(0, info.VertexBuffer, 0, sizeof(VerNorTex));
	pDevice->SetIndices(info.IndexBuffer);

	SetCombinedState(info.renderState);

	ID3DXEffect* effect = NULL;
	SetShaderStateToEffect(info.shaderState,effect);

	if (effect)
		effect->SetTechnique(info.shaderState.technique.c_str());
	UINT numPass;
	effect->Begin(&numPass,0);
	effect->BeginPass(info.shaderState.pass);

	auto dp = &info.drawParam;
	pDevice->DrawIndexedPrimitive(dp->PrimeType, dp->VertexBase, dp->MinVertexIndex, dp->NumVertices, dp->startIndex, dp->primCount);
	effect->EndPass();
	effect->End();
}

void miniDX9Render::SetShaderStateToEffect(ShaderState& ss,ID3DXEffect*& o_effect)
{
	auto device = rh->device;
	if (ss.HasShader) {
		auto effect = effectMgr.GetEffect(ss.effect);
		bool check = effect != NULL&&ss.technique != "";
		if (check) {
			SetGlobalSkinShaderParam(rh->device, effect);

			for (auto& x : ss.varMap) {
				effect->SetFloat(x.first.c_str(), x.second);
			}
			for (auto& x : ss.colorMap) {
				effect->SetVector(x.first.c_str(), (D3DXVECTOR4*)&x.second);
			}

			for (auto& x : ss.texMap) {
				effect->SetTexture(x.first.c_str(), x.second);
			}
			o_effect = effect;
			return;
		}	
	}

	//no shader
	{
		device->SetVertexShader(NULL);
		device->SetPixelShader(NULL);
	}
}

void miniDX9Render::RenderPixel(RenderInfo2& info)
{
	vector<RenderInfo*> opaqueDrawIndexs;
	vector<RenderInfo*> alphaDrawIndexs;
	FilterAlphaObject(info, opaqueDrawIndexs, alphaDrawIndexs);



	for (auto obj : opaqueDrawIndexs) {
		Render(*obj);
	}

	for (auto obj : alphaDrawIndexs) {
		Render(*obj);
	}
}

void miniDX9Render::RenderDeferred(RenderInfo2& info)
{
	vector<RenderInfo*> opaqueDrawIndexs;
	vector<RenderInfo*> alphaDrawIndexs;
	FilterAlphaObject(info, opaqueDrawIndexs, alphaDrawIndexs);

	//set gbuffer
	auto device = rh->device;
	auto rt = rtMgr.GetRT("showTex");
	auto rtdepth = rtMgr.GetRT("showTexDepth");


	device->SetRenderTarget(0, rt->m_pSurface);
	device->SetDepthStencilSurface(rtdepth->m_pSurface);
	device->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0xFF888888 , 1.0f, 0);

	// change shader
	for (auto obj : opaqueDrawIndexs) {
		Render(*obj);
	}


	//set final shader
	for (auto obj : alphaDrawIndexs) {
		Render(*obj);
	}
	fxaa.inputTexture = rt->m_pTexture;
	fxaa.Render();
	rh->ShowTexture(fxaa.OutRT->m_pSurface);
	//rh->ShowTexture(rt->m_pSurface);
	//device->SetRenderTarget(0, pBackBuffer);
	//
}

void miniDX9Render::FilterAlphaObject(RenderInfo2& info, vector<RenderInfo*>& opaqueDrawIndexs, vector<RenderInfo*>& alphaDrawIndexs)
{
	for (int i = 0; i < info.objInfoList.size(); i++) {
		if (info.objInfoList[i].renderState.rs.blendState.AlphaBlendEnable) {
			alphaDrawIndexs.push_back(&info.objInfoList[i]);
		}
		else {
			opaqueDrawIndexs.push_back(&info.objInfoList[i]);
		}
	}
}

void FXAAEffect::Init()
{
	
	m_pEffect =g_miniRender.effectMgr.CreateEffectFromFile("shader/fxaa.shader", "fxaa");
	int width = g_miniRender.width;
	int height = g_miniRender.height;
	
	m_PixelSize[0] = 1.0f / width;
	m_PixelSize[1] = 1.0f / height;

	m_hTech = m_pEffect->GetTechniqueByName("Fxaa");
	m_hTexSource = m_pEffect->GetParameterByName(NULL, "tex_source");
	m_hPixelSize = m_pEffect->GetParameterByName(NULL, "pixelSize");
	OutRT =g_miniRender.rtMgr.CreateRT("fxaaRT", width, height, D3DFMT_A8R8G8B8);

	//add check
}

void FXAAEffect::Render()
{
	if (!inputTexture) return;
	auto device = g_rh.device;
	//pDevice->GetTexture(0, &m_pOldTex);
	m_pEffect->SetTechnique(m_hTech);
	m_pEffect->SetTexture(m_hTexSource, inputTexture);
	m_pEffect->SetFloatArray(m_hPixelSize, m_PixelSize, 2);

	BlendState bs;
	bs.AlphaBlendEnable = FALSE;
	bs.AlphaTestEnable = FALSE;
	DepthStencilState ds;
	ds.ZTestEnable = FALSE;
	ds.ZWrite = FALSE;

	g_rh.SetBlendState(bs);
	g_rh.SetDepthStencilState(ds);

	device->SetFVF(D3DFVF_XYZW | D3DFVF_TEX1);
	device->SetRenderTarget(0, OutRT->m_pSurface);
	UINT uPasses;
	m_pEffect->Begin(&uPasses, 0);
	m_pEffect->BeginPass(0);
	//device->SetRenderState()
	m_pEffect->CommitChanges();
	device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_miniRender.globalValue.ScreenBuffer, 6 * sizeof(float));
	m_pEffect->EndPass();
	m_pEffect->End();

	//pDevice->SetTexture(0, m_pOldTex);

	//SAFE_RELEASE(m_pOldTex);
}
