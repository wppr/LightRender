#pragma once
#include "d3d9.h"
#include "d3dx9.h"
#include "d3dx9effect.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

//predefine
class DX9RenderHardware;

struct DepthStencilState {
	DWORD			ZTestEnable;
	DWORD			ZTestFunc;
	DWORD			ZWrite;

};
struct BlendState {
	DWORD AlphaTestEnable;
	DWORD AlphaTestFunc;
	DWORD AlphaRef;

	DWORD AlphaBlendEnable;
	DWORD SrcBlend;
	DWORD DestBlend;
	//D3DBLENDOP BlendOp;
	//DWORD BlendFactor;
	//DWORD SampleMask;
};

struct RasterizeState {
	DWORD FillMode;
	DWORD CullMode;
	 
};
struct SamplerState1 {
	DWORD MagFilter;
	DWORD MinFilter;
	DWORD MipFilter;
	DWORD MipmapBias;
	DWORD UAddress;
	DWORD VAddress;

};
struct SamplerState {
	SamplerState1 samplerState[8];
};
struct TextureList{
	LPDIRECT3DTEXTURE9 Textures[8];
	TextureList() {
		for (int i = 0; i < 8; i++) Textures[i] = NULL;
	}
};

struct TextureStageState1 {
	DWORD ColorOp;
	DWORD ColorArg1;
	DWORD ColorArg2;
	DWORD AlphaOp;
	DWORD AlphaArg1;
	DWORD AlphaArg2;
	DWORD TEXCOORDINDEX= D3DTSS_TCI_PASSTHRU;

};
struct TextureStageState4 {
	TextureStageState1 States[4];
};
struct VertexDecl
{

};

struct ViewPort {

};

struct FixedPipelineState {
	DWORD FVF;
	DWORD Texturefactor;
	D3DMATERIAL9 material;
	DWORD LightEnable;
	DWORD SpecularEnable;
	TextureStageState4 textureStageState;
};

struct RenderState {
	DepthStencilState depthStencilState;
	BlendState	blendState;
	RasterizeState rasterizeState;
};

struct CombiendRenderState
{
	RenderState rs;
	FixedPipelineState fixdRs;
	TextureList textureList;
};


struct RenderTargetDesc
{


};
enum DrawPrimitiveType {
	Primitive,
	IndexPrimitive,
	PrimitiveUp,
	IndexPrimitiveUp,
};
struct DrawParam {
	DrawPrimitiveType drawType;
	D3DPRIMITIVETYPE PrimeType;
	UINT VertexBase;
	UINT MinVertexIndex;
	UINT NumVertices;
	UINT startIndex;
	UINT primCount;

	void FromIndexTriangle(UINT vertexNum, UINT _primCount) {
		PrimeType = D3DPT_TRIANGLELIST;
		drawType = IndexPrimitive;
		NumVertices = vertexNum;
		primCount = _primCount;
		VertexBase = 0;
		startIndex = 0;
		MinVertexIndex = 0;
	}
};

//file util
void ReadFileToString(string path,string& out) {
	std::ifstream t(path);
	std::stringstream buffer;
	buffer << t.rdbuf();
	out = buffer.str();
}

struct EffectMgr {
	map<string, ID3DXEffect*> EffectMap;
	IDirect3DDevice9 *              device = NULL;
	void Init(DX9RenderHardware& rh) {
		device = rh.device;
	}
	bool CreateEffect(const void* shaderSrc,UINT length,string name,bool Compiled) {
		if (EffectMap.find(name) != EffectMap.end()) {
			if (EffectMap[name])
			return;
		}

		ID3DXEffect* effect = NULL;
		LPD3DXBUFFER pShaderErrors = NULL;
		HRESULT hr = D3DXCreateEffect(device, shaderSrc, length, NULL, NULL, 0, NULL, &effect, &pShaderErrors);
		if (hr) {
			EffectMap[name] = effect;
		}
	
	}

	void Release() {

	}
};

struct VertexShader {

};

struct IndexShader {

};

struct Camera
{
	D3DXMATRIX ViewMatrix;
	D3DXMATRIX ProjMatrix;
};





struct RenderInfo {
	D3DXMATRIX WorldMatrix;
	
	LPVERBUF VertexBuffer;
	LPINDBUF IndexBuffer;
	CombiendRenderState renderState;
	DrawParam drawParam;

};

struct RenderInfo2 {
	Camera camera;
	//对象数组 todo： pool
	vector<RenderInfo> objInfoList;
	
};


//考虑改成 TransientVertexBuffer 
//https://www.gamedevs.org/uploads/efficient-buffer-management.pdf
struct DynamicVertexBuffer {
	LPVERBUF				m_pVB;
	LPINDBUF				m_pIB;
	uint32					m_MaxVBSize;
	uint32					m_MaxIBSize;
	vector<BYTE>			m_pSysVB;
	vector<BYTE>			m_pSysIB;
	uint32					m_CurVBOffset;
	uint32					m_CurIBOffset;

#define DYNAMIC_VB_SIZE		0x800000
#define DYNAMIC_IB_SIZE		0x400000
	DynamicVertexBuffer() {
		m_pVB = NULL;
		m_pIB = NULL;
		m_MaxVBSize = 0x800000;
		m_MaxIBSize = 0x400000;
		m_pSysVB.resize(m_MaxVBSize);
		m_pSysIB.resize(m_MaxIBSize);
	}
	void Create(LPDIRECT3DDEVICE9 device) {

		device->CreateVertexBuffer(m_MaxVBSize,
			D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &m_pVB, NULL);

		device->CreateIndexBuffer(m_MaxIBSize,
			D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB, NULL);
	}
	void Reset() {
		m_CurVBOffset = 0;
		m_CurIBOffset = 0;
	}
	void AllocBuffer(UINT VerNum,UINT IndNum,RenderInfo& info) {
		info.VertexBuffer = m_pVB;
		info.IndexBuffer = m_pIB;
		info.drawParam.FromIndexTriangle(VerNum, IndNum);
		//info.drawParam
	}
	int Upload() {
		void* Buf;
		if (FAILED(m_pVB->Lock(0, 0, &Buf, D3DLOCK_DISCARD)))
		{
			//FreeAllHWData();
			//ResetStack();
			return ERROR;
		}
		memcpy(Buf, &m_pSysVB[0], m_CurVBOffset);
		m_pVB->Unlock();

		if (FAILED(m_pIB->Lock(0, 0, &Buf, D3DLOCK_DISCARD)))
		{
			//FreeAllHWData();
			//ResetStack();
			return ERROR;
		}
		memcpy(Buf, &m_pSysIB[0], m_CurIBOffset);
		m_pIB->Unlock();
		return S_OK;
	}

	void Release() {

	}
	//DX9RenderHardware* rh = NULL;
};

class DynamicVBHelper {
public:
	static void InitVB() {

	}
};


struct RenderBuffer {
	LPVERBUF vb;
	LPINDBUF ib;
};

//struct simpleBufferMgr {
	
//};


struct RenderInfoFixed {

};


struct RenderConfig {

	bool IsDeferredShadingEnable=false;
	bool IsLinearLight=false;
	bool IsFXAAEnable=false;
	
};

class RenderSystem {


};
class BufferPool {
	void createVertexBuffer() {

	}
};



class DX9RenderHardware {
public:
	IDirect3D9 *           d3d = NULL;              //D3D接口
	IDirect3DDevice9 *              device = NULL;            //D3D设备

	void SetVertexBuffer();
	void SetIndexBuffer();
	void SetVertexShader();
	void SetPixelShader();



	void SetTexture(DWORD index, LPDIRECT3DTEXTURE9 pTexture) {
		device->SetTexture(index, pTexture);
	}
	void SetDepthStencilState(DepthStencilState& ds) {
		device->SetRenderState(D3DRS_ZENABLE, ds.ZTestEnable);
		device->SetRenderState(D3DRS_ZFUNC, ds.ZTestFunc);
		device->SetRenderState(D3DRS_ZWRITEENABLE, ds.ZWrite);
	}
	void SetBlendState(BlendState& bs) {
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, bs.AlphaBlendEnable);
		device->SetRenderState(D3DRS_ALPHAREF, bs.AlphaRef);
		device->SetRenderState(D3DRS_ALPHATESTENABLE, bs.AlphaTestEnable);
		device->SetRenderState(D3DRS_ALPHAFUNC, bs.AlphaTestFunc);
		device->SetRenderState(D3DRS_SRCBLEND, bs.SrcBlend);
		device->SetRenderState(D3DRS_DESTBLEND, bs.DestBlend);
	}
	void SetRasterizeState(RasterizeState& rs) {
		device->SetRenderState(D3DRS_CULLMODE, rs.CullMode);
		device->SetRenderState(D3DRS_FILLMODE, rs.FillMode);
	}
	UINT GetAvailableTextureMem() {
		return device->GetAvailableTextureMem();
	}

	//fixed pipeline

	void SetFixedState(FixedPipelineState& fs) {
		device->SetRenderState(D3DRS_TEXTUREFACTOR, fs.Texturefactor);
		device->SetRenderState(D3DRS_LIGHTING, fs.LightEnable);
		device->SetRenderState(D3DRS_SPECULARENABLE, fs.SpecularEnable);
		device->SetMaterial(&fs.material);
	}

	void SetMaterial(D3DMATERIAL9* mat) {
		device->SetMaterial(mat);
	}
};



class miniDX9Render {
public:

	void Init(LPDIRECT3DDEVICE9 pDevice);

	void Render(RenderInfo2& info);

	void Render(RenderInfo& info);

	void UseEffect() {

	}

	void SetTextures(TextureList& list) {
		for(int i=0;i<8;i++)
			if (list.Textures[i] != NULL) {
				rh->SetTexture(i, list.Textures[i]);
			}
	}

	void SetCommonState(RenderState& state) {
		rh->SetBlendState(state.blendState);
		rh->SetDepthStencilState(state.depthStencilState);
		rh->SetRasterizeState(state.rasterizeState);
	}

	void SetCombinedState(CombiendRenderState& state) {
		SetTextures(state.textureList);
		SetCommonState(state.rs);
		rh->SetFixedState(state.fixdRs);
	}
	DX9RenderHardware* rh=NULL;
};


class App {
public:
	string WorkDirectory;

};
//void CreateDynamicVertexBuffer(UINT size, LPVERBUF buffer) {

//	rh->m_pDevice->CreateVertexBuffer(size,
//		D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &buffer, NULL);
//}
//map<string, LPVERBUF> g_VBs;
//map<string, LPINDBUF> g_IBs;









extern miniDX9Render g_miniRender;
extern DX9RenderHardware g_rh;
extern map<void*, RenderBuffer> g_buffers;
void ReadConfig();

//ModelToRender helper Func
#include "EditModel.h"

void EditModelToRenderInfo(CEditModel* model, RenderInfo2& infoList);

void PieceToRenderInfo(CPiece* piece, RenderInfo& info, CRenderMatrix& ModelMatrix, bool CalCloth, CRenderPiece* pRP);


