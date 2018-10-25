#include "stdafx.h"
#include "ModelToRender.h"
//#include "D3dd.h"

// helper function
#include "PieceGroup.h"
#include "ModelEditorApp.h"




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
		PieceToRenderInfo(piece, info, m_RenderMatrix, true, &renderPiece.second);
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

void SetGlobalSkinShaderParam(IDirect3DDevice9* m_pDevice, ID3DXEffect* m_pSkinEffect) {

	D3DXMATRIX World, View, Proj;
	D3DXMatrixIdentity(&World);
	m_pDevice->GetTransform(D3DTS_VIEW, &View);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &Proj);
	int boneCount = 4;

	D3DXMATRIXA16 combinedMat;
	D3DXCOLOR globalAmbient(0xffffffff);

	D3DXCOLOR AmbientColor = g_app.scene.m_AmbientColor;
	D3DXCOLOR DiffuseColor= g_app.scene.DirLight.Diffuse;
	D3DXVECTOR3 lightDir = g_app.scene.DirLight.Direction;

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
void PieceToRenderInfo(CPiece* piece, RenderInfo& info, CRenderMatrix& ModelMatrix, bool CalCloth, CRenderPiece* pRP)
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

	RenderBuffer* rb = g_bufferMgr.GetBuffer(piece);
	if (!rb)
		rb = g_bufferMgr.CreateRenderBuffer(piece, VerBufSize, IndBufSize, Format);

	VB = rb->vb;
	IB = rb->ib;


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

	CPieceRenderStyle* pieceRS = piece->GetRS(0);
	if (pieceRS) {
		STATIC_RS rs;
		auto rs_time = pieceRS->GetShade();
		rs_time.GetCurRenderStyle(rs, pieceRS->m_Texture, ModelMatrix.m_CurFrame*33.33f, NULL);
		StaticRSToRenderState(rs, info.renderState);

		RSToShaderParam(pieceRS->m_Texture, rs, info.shaderState);
	}

	info.VertexBuffer = VB;
	info.IndexBuffer = IB;
	info.WorldMatrix = matWorld;
	info.drawParam.FromIndexTriangle(VerNum, IndNum);
}


#include "BoxObject.h"
CBoxObject box;

