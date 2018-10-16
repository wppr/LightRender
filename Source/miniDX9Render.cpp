#include "stdafx.h"
#include "miniDX9Render.h"
#include <iostream>
using namespace std;


miniDX9Render g_miniRender;
DX9RenderHardware g_rh;
map<void*, RenderBuffer> g_buffers;
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
	for (auto renderPiece : model->GetMeshsPiece())
	{
		D3DXMATRIX orgMat = model->m_RenderMatrix.m_MatrixUnit[0].m_GlobeSkeletalMatrix;

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
		PieceToRenderInfo(piece, info, model->m_RenderMatrix, false,&renderPiece.second);
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

//output: info
void PieceToRenderInfo(CPiece* piece, RenderInfo& info, CRenderMatrix& ModelMatrix,bool CalCloth, CRenderPiece* pRP)
{
	auto m_pD3dDev = g_rh.device;
	auto m_Mesh = piece->GetMesh();
	auto m_bHighRes = piece->m_bHighRes;

	DWORD Format;
	LPVERBUF VB = NULL;
	LPINDBUF IB = NULL;

	//��ʼ��BUF��С
	static UINT VerBufSize = 1165536;
	static char* RenderBuf = new char[VerBufSize];
	static UINT IndBufSize = 1165536;
	 

	int Level = 0;
	int VerNum = m_Mesh->GetVerNum(Level);
	UINT VBSize = 0;

	VBSize = VerNum * sizeof(VerNorTex);
	Format = VerNorTex::Format;


	int IndNum = m_Mesh->GetFaceNum(Level);
	UINT IBSize = IndNum * 6;

	//��BUF̫С���´���
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
	
	if (g_buffers.find(piece) != g_buffers.end()) {
		VB = g_buffers[piece].vb;
		IB = g_buffers[piece].ib;
	}
	else {
		printf("alloc buffer");
		RenderBuffer rb;
		if (m_pD3dDev && !VB)
			m_pD3dDev->CreateVertexBuffer(VerBufSize, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, Format, D3DPOOL_DEFAULT, &VB, NULL);

		if (m_pD3dDev && !IB)
			m_pD3dDev->CreateIndexBuffer(IndBufSize, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &IB, NULL);
		rb.ib = IB;
		rb.vb = VB;
		g_buffers[piece] = rb;
	}


	D3DXMATRIX& matWorld = ModelMatrix.m_MatrixUnit[0].m_CurSkeletalMatrix;

	//�������㶥��
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
	rh = &g_rh;
	rh->device = pDevice;
	box.Init(rh->device);
}

//end

void miniDX9Render::Render(RenderInfo2& info)
{
	//filter alpha object
	vector<RenderInfo*> opaqueDrawIndexs;
	vector<RenderInfo*> alphaDrawIndexs;
	for (int i = 0; i < info.objInfoList.size(); i++) {
		if (info.objInfoList[i].renderState.rs.blendState.AlphaBlendEnable) {
			alphaDrawIndexs.push_back(&info.objInfoList[i]);
		}
		else {
			opaqueDrawIndexs.push_back(&info.objInfoList[i]);
		}
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
	auto dp = &info.drawParam;
	pDevice->DrawIndexedPrimitive(dp->PrimeType, dp->VertexBase, dp->MinVertexIndex, dp->NumVertices, dp->startIndex, dp->primCount);
}