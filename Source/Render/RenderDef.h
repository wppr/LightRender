#pragma once
#include <d3d11_1.h>
#include <string>
using namespace std;



enum ERasterizerFillMode
{
	FM_Point,
	FM_Wireframe,
	FM_Solid,
};

enum ERasterizerCullMode
{
	CM_None,
	CM_CW,
	CM_CCW,
};


struct VertexInputDesc{
	D3D11_INPUT_ELEMENT_DESC element[16];
};

struct ShaderBindDesc {
};

struct TextureStateDesc {
};

struct RenderTargetDesc {
};

struct SamplerStateDesc {
	D3D11_SAMPLER_DESC sampler[16];
};

struct ShaderConstantDesc {
	string VertexShaderName;
	string PixelShaderName;
};

struct ViewportDesc {
};

struct ScissorDesc {

};

struct BufferBindDesc {

};

struct BufferDesc{
	D3D11_BUFFER_DESC desc;
};

struct BufferObject{

};

class BufferMgr{
	void CreateBuffer(string name, D3D11_BUFFER_DESC desc){

	}
};

struct DrawParamDesc{
	bool IsIndex=true;
	int IndexLocation;
	int BaseVertexLocation;
	int IndexCount;
};
struct RenderParam
{
	VertexInputDesc InputLayout;
	D3D11_RASTERIZER_DESC RasterizeState;
	D3D11_BLEND_DESC	BlendState;
	D3D11_DEPTH_STENCIL_DESC DepthState;
	ShaderBindDesc	ShaderState;
	TextureStateDesc TextureState;
	RenderTargetDesc RenderTargetState;
	ShaderConstantDesc ConstantState;
	BufferBindDesc	BufferState;
	DrawParamDesc	DrawParam;
};
struct FrameData {

};

// side effect
class RenderStrategy{
	void RenderBasic(FrameData& fp){}
};


class Renderer{
	RenderParam CurDeviceState;
	RenderParam NextDeviceState;
	void render(){
		auto scene = 1;
		
	}

	// transform scene to FrameData
};

class TextureMgr{

};

class DXStateMgr{
	RenderParam CurDeviceState;
	void SetRasterizeState(){}
};

class Vector3 {
	Vector3(){}
};
class Quaternion {

};
using Translate = Vector3;
using Scale = Vector3;
using Rotate = Quaternion;

class Transform {
	Translate translate;
	Scale scale;
	Rotate rotate;

	Transform* parrent = NULL;
};