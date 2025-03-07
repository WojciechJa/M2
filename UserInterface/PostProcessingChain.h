﻿#pragma once
#include "StdAfx.h"
#ifndef DISABLE_POST_PROCESSING
#include "PostProcessingEffect.h"
#include "../eterLib/GrpDevice.h"

class CPostProcessingChain
{
public:
	CPostProcessingChain();
	~CPostProcessingChain();

	void ReleaseResources();
	bool CreateResources();

	bool BeginScene();
	bool EndScene();

	bool Render();

	CPostProcessingEffect AddEffect(const char* c_pszFileName);
	bool AddEffect(const CPostProcessingEffect& c_kEffect);
	bool RemoveEffect(const CPostProcessingEffect& c_kEffect);

	std::list<CPostProcessingEffect>& GetEffects();

protected:
	std::list<CPostProcessingEffect> m_kEffects;
	IDirect3DSurface9* m_lpDepthStencilSurface;
	IDirect3DTexture9* m_lpSourceTexture;
	IDirect3DSurface9* m_lpSourceTextureSurface;
	IDirect3DTexture9* m_lpLastPass;
	IDirect3DTexture9* m_lpTarget;
	IDirect3DSurface9* m_lpTargetSurface;
	IDirect3DSurface9* m_lpBackupTargetSurface;
	IDirect3DSurface9* m_lpDepthStencilBackup;
	IDirect3DSurface9* m_lpLastPassSurface;
	IDirect3DVertexBuffer9* m_lpScreenQuad;
	CGraphicDevice m_grpDevice;
};
#endif