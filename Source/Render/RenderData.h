#pragma once
#include "RenderDef.h"
struct VertexDecl {

};

struct ShaderState {

};

struct DepthStencilState{

};

struct SamplerState {

};

struct TextureStageState {

};
struct FRasterizerStateInitializerRHI
{
	ERasterizerFillMode FillMode;
	ERasterizerCullMode CullMode;
	float DepthBias;
	float SlopeScaleDepthBias;
	bool bAllowMSAA;
	bool bEnableLineAA;
};




struct PipelineState{
public:
    VertexDecl vertexDecl;
    ShaderState shaderState;
};


