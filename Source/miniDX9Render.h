#pragma once
#include "d3d9.h"
#include "d3dx9.h"
#include "d3dx9effect.h"
#include <iostream>
#include "nlohmann/json.hpp"
using namespace std;
using json = nlohmann::json;

//predefine
class DX9RenderHardware;
class EffectMgr;
//#define SAFE_RELEASE(p) 	{ if ( p ) { (p)->Release(); (p) = NULL;	} } 


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



struct EffectMgr {
	map<string, ID3DXEffect*> EffectMap;
	IDirect3DDevice9 *              device = NULL;
	void Init(IDirect3DDevice9 * d) {
		device = d;

		CreateEffectFromFile("shader/skin_animation2.fxo", "skinAnimation2");
		
		
	}

	ID3DXEffect* CreateEffectFromFile(string path,string name) {
		std::ifstream file(path, std::ios::binary | std::ios::ate);
		if (file) {
			std::streamsize size = file.tellg();
			file.seekg(0, std::ios::beg);

			std::vector<char> buffer(size);
			if (file.read(buffer.data(), size))
			{
				bool r = CreateEffectFromSrc(&buffer[0], buffer.size(), name);
				cout << "create effect "+path+":"<< name+ " : "<< r << endl;
				if (r) return GetEffect(name);
			}
		}
		return NULL;
	}

	bool CreateEffectFromSrc(const void* shaderSrc,UINT length,string name) {
		if (EffectMap.find(name) != EffectMap.end()) {
			if (EffectMap[name])
			return true;
		}

		ID3DXEffect* effect = NULL;
		LPD3DXBUFFER pShaderErrors = NULL;
		HRESULT hr = D3DXCreateEffect(device, shaderSrc, length, NULL, NULL, 0, NULL, &effect, &pShaderErrors);
		if (hr==S_OK) {
			EffectMap[name] = effect;
			return true;
		}
		return false;
	}
	ID3DXEffect* GetEffect(string name) {
		if (EffectMap.find(name) != EffectMap.end()) {
			return EffectMap[name];
		}
		return NULL;
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


struct ShaderValue {
	array<float,16> value;
	int size=0;
};

struct ShaderState {
	bool HasShader = false;
	string effect;
	string technique;
	int pass=0;

	map<string, IDirect3DTexture9*> texMap;
	map<string, float> varMap;
	map<string, D3DCOLORVALUE> colorMap;
};

struct RenderTarget
{
	uint32_t				m_Width;
	uint32_t				m_Height;
	D3DFORMAT			m_Format;
	LPDIRECT3DTEXTURE9  m_pTexture=NULL;
	LPDIRECT3DSURFACE9	m_pSurface=NULL;

};

struct RenderTargetMgr
{
	IDirect3DDevice9* device = NULL;
	void Init(IDirect3DDevice9* pdevice) {
		device = pdevice;
		uint32_t w = 1280, h = 800;
		CreateRT("showTex", w, h, D3DFMT_A8R8G8B8);
		CreateDepthRT("showTexDepth", w, h, D3DFMT_D24S8);
		
		

		//gbuffer
		//CreateRT("g_diffuse", w, h, D3DFMT_A8R8G8B8);
		//CreateRT("g_normal", w, h, D3DFMT_A8R8G8B8);
		//CreateRT("g_customInfo", w, h, D3DFMT_A8R8G8B8);
		//CreateRT("g_depth", w, h, D3DFMT_R32F);


		
	}

	RenderTarget* CreateRT(string name, uint32_t width, uint32_t height, D3DFORMAT format) {
		return CreateRTImp(name, width, height, format, D3DUSAGE_RENDERTARGET);
	}
	RenderTarget* CreateDepthRT(string name, uint32_t width, uint32_t height, D3DFORMAT format) {
		return CreateRTImp(name, width, height, format, D3DUSAGE_DEPTHSTENCIL);
	}
	RenderTarget* CreateRTImp(string name, uint32_t width, uint32_t height, D3DFORMAT format,DWORD Usage) {
		RenderTarget rt;

		rt.m_Width = width;
		rt.m_Height = height;
		rt.m_Format = format;
		
		HRESULT hr = D3DXCreateTexture(device, width, height, 1, Usage, format, D3DPOOL_DEFAULT, &rt.m_pTexture);
		if (SUCCEEDED(hr))
		{
			hr = rt.m_pTexture->GetSurfaceLevel(0, &rt.m_pSurface);
			RenderTargetMap[name] = rt;
			return &RenderTargetMap[name];
		}
		return NULL;
	}

	

	RenderTarget* GetRT(string name) {
		if (RenderTargetMap.find(name) != RenderTargetMap.end())
			return &RenderTargetMap[name];
		else
			return NULL;
	}

	void Release(RenderTarget* rt) {
		SAFE_RELEASE(rt->m_pSurface);
		SAFE_RELEASE(rt->m_pTexture);

	}

	map<string, RenderTarget> RenderTargetMap;
};

struct RenderTargetState {

};

struct RenderInfo {
	D3DXMATRIX WorldMatrix;
	
	LPDIRECT3DVERTEXBUFFER9 VertexBuffer;
	LPDIRECT3DINDEXBUFFER9 IndexBuffer;
	CombiendRenderState renderState;
	DrawParam drawParam;
	ShaderState shaderState;
};

struct RenderInfo2 {
	Camera camera;
	//对象数组 todo： pool
	vector<RenderInfo> objInfoList;
	
};



//考虑改成 TransientVertexBuffer 
//https://www.gamedevs.org/uploads/efficient-buffer-management.pdf
//struct DynamicVertexBuffer {
//	LPVERBUF				m_pVB;
//	LPINDBUF				m_pIB;
//	uint32					m_MaxVBSize;
//	uint32					m_MaxIBSize;
//	vector<BYTE>			m_pSysVB;
//	vector<BYTE>			m_pSysIB;
//	uint32					m_CurVBOffset;
//	uint32					m_CurIBOffset;
//
//#define DYNAMIC_VB_SIZE		0x800000
//#define DYNAMIC_IB_SIZE		0x400000
//	DynamicVertexBuffer() {
//		m_pVB = NULL;
//		m_pIB = NULL;
//		m_MaxVBSize = 0x800000;
//		m_MaxIBSize = 0x400000;
//		m_pSysVB.resize(m_MaxVBSize);
//		m_pSysIB.resize(m_MaxIBSize);
//	}
//	void Create(LPDIRECT3DDEVICE9 device) {
//
//		device->CreateVertexBuffer(m_MaxVBSize,
//			D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &m_pVB, NULL);
//
//		device->CreateIndexBuffer(m_MaxIBSize,
//			D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB, NULL);
//	}
//	void Reset() {
//		m_CurVBOffset = 0;
//		m_CurIBOffset = 0;
//	}
//	void AllocBuffer(UINT VerNum,UINT IndNum,RenderInfo& info) {
//		info.VertexBuffer = m_pVB;
//		info.IndexBuffer = m_pIB;
//		info.drawParam.FromIndexTriangle(VerNum, IndNum);
//		//info.drawParam
//	}
//	int Upload() {
//		void* Buf;
//		if (FAILED(m_pVB->Lock(0, 0, &Buf, D3DLOCK_DISCARD)))
//		{
//			//FreeAllHWData();
//			//ResetStack();
//			return ERROR;
//		}
//		memcpy(Buf, &m_pSysVB[0], m_CurVBOffset);
//		m_pVB->Unlock();
//
//		if (FAILED(m_pIB->Lock(0, 0, &Buf, D3DLOCK_DISCARD)))
//		{
//			//FreeAllHWData();
//			//ResetStack();
//			return ERROR;
//		}
//		memcpy(Buf, &m_pSysIB[0], m_CurIBOffset);
//		m_pIB->Unlock();
//		return S_OK;
//	}
//
//	void Release() {
//
//	}
//	//DX9RenderHardware* rh = NULL;
//};
//
//class DynamicVBHelper {
//public:
//	static void InitVB() {
//
//	}
//};





struct RenderInfoFixed {

};


struct RenderConfig {

	int pipelineType = 1;
	bool IsDeferredShadingEnable=false;
	bool IsLinearLight=false;
	bool IsFXAAEnable=false;
	
};

struct RenderBuffer {
	IDirect3DVertexBuffer9 * vb;
	IDirect3DIndexBuffer9 * ib;
};
class RenderBufferMgr {
public:
	map<void*, RenderBuffer> PieceTobuffers;
	IDirect3DDevice9* device;
	RenderBuffer* GetBuffer(void* piece) {
		if (PieceTobuffers.find(piece) != PieceTobuffers.end()) {
			return &PieceTobuffers[piece];
		}
		else
			return NULL;
	}

	RenderBuffer* CreateRenderBuffer(void* piece,UINT VerBufSize, UINT IndBufSize, DWORD Format) {
		printf("alloc buffer");
		RenderBuffer rb;
		device->CreateVertexBuffer(VerBufSize, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, Format, D3DPOOL_DEFAULT, &rb.vb, NULL);

		device->CreateIndexBuffer(IndBufSize, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &rb.ib, NULL);
		PieceTobuffers[piece] = rb;
		return &PieceTobuffers[piece];
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

	void ShowTexture(IDirect3DSurface9* surface, D3DTEXTUREFILTERTYPE filter= D3DTEXF_POINT) {
		LPDIRECT3DSURFACE9 pBackBuffer;
		device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
		device->StretchRect(surface, NULL, pBackBuffer, NULL, filter);
	}

	void ResetDefaultRT() {
		LPDIRECT3DSURFACE9 pBackBuffer;
		device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
		device->SetRenderTarget(0, pBackBuffer);
	}
};

class Pipeline {
public:
	virtual void Init(){}
	virtual void Render(){}
};

class FXAAEffect {
public:
	void Init();
	void Render();

	//
	LPDIRECT3DTEXTURE9 inputTexture=NULL;
	RenderTarget* OutRT;


	float			 m_PixelSize[2];
	D3DXHANDLE		 m_hTech;
	D3DXHANDLE		 m_hTexSource;
	D3DXHANDLE		 m_hPixelSize;
	ID3DXEffect* m_pEffect;
};


struct GlobalValue {
	float ScreenBuffer[24];
	float w = 1280;
	float h = 800;
	GlobalValue() {
		float offU, offV;
		offU = 0;
		offV = 0;
		ScreenBuffer[0] = -1.0f -offU; ScreenBuffer[1] = -1.0f + offV; ScreenBuffer[2] = 0.0f; ScreenBuffer[3] = 1.0f; ScreenBuffer[4] = 0.0f; ScreenBuffer[5] = 1.0f;
		ScreenBuffer[6] = -1.0f - offU; ScreenBuffer[7] = 1.0f + offV; ScreenBuffer[8] = 0.0f; ScreenBuffer[9] = 1.0f; ScreenBuffer[10] = 0.0f; ScreenBuffer[11] = 0.0f;
		ScreenBuffer[12] = 1.0f - offU; ScreenBuffer[13] = -1.0f + offV; ScreenBuffer[14] = 0.0f; ScreenBuffer[15] = 1.0f; ScreenBuffer[16] = 1.0f; ScreenBuffer[17] = 1.0f;
		ScreenBuffer[18] = 1.0f - offU; ScreenBuffer[19] = 1.0f + offV; ScreenBuffer[20] = 0.0f; ScreenBuffer[21] = 1.0f; ScreenBuffer[22] = 1.0f; ScreenBuffer[23] = 0.0f;
	}
};


class miniDX9Render {
public:

	void Init(LPDIRECT3DDEVICE9 pDevice);

	void RenderFixed(RenderInfo2& info);

	void RenderPixel(RenderInfo2& info);

	void RenderDeferred(RenderInfo2& info);

	void RenderWithConfig(RenderInfo2& info);

	void FilterAlphaObject(RenderInfo2& info, vector<RenderInfo*>& opaqueDrawIndexs, vector<RenderInfo*>& alphaDrawIndexs);

	void Render(RenderInfo& info);


	void SetTextures(TextureList& list) {
		for(int i=0;i<8;i++)
			if (list.Textures[i] != NULL) {
				rh->SetTexture(i, list.Textures[i]);
			}
	}
	void SetRenderTarget(UINT index, RenderTarget* rt) {
		//if (rt->m_pSurface) {
		rh->device->SetRenderTarget(index, rt->m_pSurface);
		//}
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

	void SetShaderStateToEffect(ShaderState& ss, ID3DXEffect*& o_effect);

	void OnResize(UINT w,UINT h){}
	void OnDeviceLost(){}
	void OnDeviceReset(){}

	bool Inited = false;
	RenderConfig config;
	UINT width, height;
	EffectMgr effectMgr;
	DX9RenderHardware* rh=NULL;
	RenderTargetMgr rtMgr;
	GlobalValue globalValue;
	FXAAEffect fxaa;
	
};




//file util






extern miniDX9Render g_miniRender;
extern DX9RenderHardware g_rh;

extern RenderBufferMgr g_bufferMgr;










