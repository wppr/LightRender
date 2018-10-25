#include "stdafx.h"
#include "miniDX9Render.h"
#include <iostream>
#include <sstream>
using namespace std;
#include "ModelToRender.h"

miniDX9Render g_miniRender;
DX9RenderHardware g_rh;

RenderBufferMgr g_bufferMgr;










void miniDX9Render::Init(LPDIRECT3DDEVICE9 pDevice)
{
	//m_pD3d = NULL;
	//m_pDevice = pDevice;
	if (Inited) return;
	rh = &g_rh;
	rh->device = pDevice;
	g_bufferMgr.device = pDevice;
	effectMgr.Init(pDevice);
	rtMgr.Init(pDevice);
	width = 1280;
	height = 800;
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

	if (effect) {
		effect->SetTechnique(info.shaderState.technique.c_str());
		UINT numPass;
		effect->Begin(&numPass, 0);
		effect->BeginPass(info.shaderState.pass);
	}
	auto dp = &info.drawParam;
	
		pDevice->DrawIndexedPrimitive(dp->PrimeType, dp->VertexBase, dp->MinVertexIndex, dp->NumVertices, dp->startIndex, dp->primCount);
	if (effect) {
		effect->EndPass();
		effect->End();
	}
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


	rh->ResetDefaultRT();
	//rh->ShowTexture(rt->m_pSurface);
	//device->SetRenderTarget(0, pBackBuffer);
	//
}

void miniDX9Render::RenderWithConfig(RenderInfo2& info)
{
	if (config.pipelineType == 1)
		RenderFixed(info);
	else if (config.pipelineType == 2)
		RenderPixel(info);
	else if (config.pipelineType == 3)
		RenderDeferred(info);
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
	
	m_pEffect =g_miniRender.effectMgr.CreateEffectFromFile("shader/fxaa.fxo", "fxaa");
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
	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
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
