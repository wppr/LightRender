#pragma once
#include "miniDX9Render.h"

void ReadConfig();

//ModelToRender helper Func
#include "EditModel.h"


//extern CPieceEdit* g_pPieceEdit;

void SetGlobalSkinShaderParam(IDirect3DDevice9* m_pDevice, ID3DXEffect* m_pSkinEffect);

void EditModelToRenderInfo(CEditModel* model, RenderInfo2& infoList);

void PieceToRenderInfo(CPiece* piece, RenderInfo& info, CRenderMatrix& ModelMatrix, bool CalCloth, CRenderPiece* pRP);