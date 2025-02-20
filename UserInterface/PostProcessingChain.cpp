#include "stdafx.h"
#ifndef DISABLE_POST_PROCESSING
#include "../EterLib/StdAfx.h"
#include "../EterLib/StateManager.h"
#include "../EterPack/EterPackManager.h"
#include "PostProcessingChain.h"

typedef struct SRTTVertex
{
	D3DXVECTOR4 p;
	D3DXVECTOR2 t;
} TRTTVertex;

CPostProcessingChain::CPostProcessingChain() : m_lpSourceTexture(nullptr), m_lpLastPass(nullptr), m_lpTarget(nullptr), m_lpBackupTargetSurface(nullptr), m_lpSourceTextureSurface(nullptr), m_lpTargetSurface(nullptr), m_lpDepthStencilSurface(nullptr), m_lpDepthStencilBackup(nullptr), m_lpScreenQuad(nullptr), m_lpLastPassSurface(nullptr)
{
}
CPostProcessingChain::~CPostProcessingChain()
{
	ReleaseResources();
}

void CPostProcessingChain::ReleaseResources()
{
	if (m_lpSourceTextureSurface)
	{
		m_lpSourceTextureSurface->Release();
		m_lpSourceTextureSurface = nullptr;
	}
	if (m_lpSourceTexture)
	{
		m_lpSourceTexture->Release();
		m_lpSourceTexture = nullptr;
	}
	if (m_lpLastPassSurface)
	{
		m_lpLastPassSurface->Release();
		m_lpLastPassSurface = nullptr;
	}
	if (m_lpLastPass)
	{
		m_lpLastPass->Release();
		m_lpLastPass = nullptr;
	}
	if (m_lpTarget)
	{
		m_lpTarget->Release();
		m_lpTarget = nullptr;
	}
	if (m_lpTargetSurface)
	{
		m_lpTargetSurface->Release();
		m_lpTarget = nullptr;
	}
	if (m_lpDepthStencilSurface)
	{
		m_lpDepthStencilSurface->Release();
		m_lpDepthStencilSurface = nullptr;
	}
	if (m_lpScreenQuad)
	{
		m_lpScreenQuad->Release();
		m_lpScreenQuad = nullptr;
	}
}
bool CPostProcessingChain::CreateResources()
{
	ReleaseResources();

	D3DSURFACE_DESC kDesc;
	IDirect3DSurface9* pSurface;
	STATEMANAGER.GetDevice()->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface);
	pSurface->GetDesc(&kDesc);
	pSurface->Release();

	if (FAILED(STATEMANAGER.GetDevice()->CreateTexture(kDesc.Width, kDesc.Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F , D3DPOOL_DEFAULT, &m_lpSourceTexture, nullptr)))
	{
		TraceError("CPostProcessingChain unable to create render target");
		return false;
	}
	m_lpSourceTexture->GetSurfaceLevel(0, &m_lpSourceTextureSurface);
	if (FAILED(STATEMANAGER.GetDevice()->CreateTexture(kDesc.Width, kDesc.Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT, &m_lpLastPass, nullptr)))
	{
		TraceError("CPostProcessingChain unable to create pass render target");
		return false;
	}
	m_lpLastPass->GetSurfaceLevel(0, &m_lpLastPassSurface);
	if (FAILED(STATEMANAGER.GetDevice()->CreateTexture(kDesc.Width, kDesc.Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F, D3DPOOL_DEFAULT, &m_lpTarget, nullptr)))
	{
		TraceError("CPostProcessingChain unable to create final render target");
		return false;
	}
	m_lpTarget->GetSurfaceLevel(0, &m_lpTargetSurface);

	STATEMANAGER.GetDevice()->GetDepthStencilSurface(&pSurface);
	pSurface->GetDesc(&kDesc);
	pSurface->Release();
	if (FAILED(STATEMANAGER.GetDevice()->CreateDepthStencilSurface(kDesc.Width, kDesc.Height, D3DFMT_D16, kDesc.MultiSampleType, kDesc.MultiSampleQuality, FALSE, &m_lpDepthStencilSurface, nullptr)))
	{
		TraceError("CPostProcessingChain unable to create depth surface");
		return false;
	}

	if (FAILED(STATEMANAGER.GetDevice()->CreateVertexBuffer(sizeof(TRTTVertex)* 6, 0, D3DFVF_XYZRHW | D3DFVF_TEX1, D3DPOOL_MANAGED, &m_lpScreenQuad, nullptr)))
	{
		TraceError("CPostProcessingChain unable to create screen quad");
		return false;
	}
	
	float fWidth = static_cast<float>(kDesc.Width) - 0.5f;
	float fHeight = static_cast<float>(kDesc.Height) - 0.5f;

	TRTTVertex* pVertices;
	m_lpScreenQuad->Lock(0, 0, reinterpret_cast<void**>(&pVertices), 0);
	pVertices[0].p = D3DXVECTOR4(-0.5f, -0.5f, 0.0f, 1.0f);
	pVertices[0].t = D3DXVECTOR2(0.0f, 0.0f);
	pVertices[1].p = D3DXVECTOR4(-0.5f, fHeight, 0.0f, 1.0f);
	pVertices[1].t = D3DXVECTOR2(0.0f, 1.0f);
	pVertices[2].p = D3DXVECTOR4(fWidth, fHeight, 0.0f, 1.0f);
	pVertices[2].t = D3DXVECTOR2(1.0f, 1.0f);
	pVertices[3].p = D3DXVECTOR4(-0.5f, -0.5f, 0.0f, 1.0f);
	pVertices[3].t = D3DXVECTOR2(0.0f, 0.0f);
	pVertices[4].p = D3DXVECTOR4(fWidth, fHeight, 0.0f, 1.0f);
	pVertices[4].t = D3DXVECTOR2(1.0f, 1.0f);
	pVertices[5].p = D3DXVECTOR4(fWidth, -0.5f, 0.0f, 1.0f);
	pVertices[5].t = D3DXVECTOR2(1.0f, 0.0f);
	m_lpScreenQuad->Unlock();

	AddEffect("shaders/BrightFilter.fxd");
	AddEffect("shaders/VerticalBlur.fxd");
	AddEffect("shaders/HorizontalBlur.fxd");
	AddEffect("shaders/BloomAdd.fxd");
	//AddEffect("shaders/Tint.fxd");

	//AddEffect("shaders/nBloomAdd.fxd");
	//AddEffect("shaders/nBrightFilter.fxd");


	return true;
}

bool CPostProcessingChain::BeginScene()
{
	if (!m_lpSourceTexture || !m_lpLastPass || !m_lpTarget || !m_lpDepthStencilSurface)
	{
		if (!CreateResources())
		{
			return false;
		}
	}

	STATEMANAGER.GetDevice()->GetRenderTarget(0, &m_lpBackupTargetSurface);
	STATEMANAGER.GetDevice()->SetRenderTarget(0, m_lpSourceTextureSurface);
	STATEMANAGER.GetDevice()->GetDepthStencilSurface(&m_lpDepthStencilBackup);
	STATEMANAGER.GetDevice()->SetDepthStencilSurface(m_lpDepthStencilSurface);

	return true;
}

bool CPostProcessingChain::EndScene()
{
	STATEMANAGER.GetDevice()->SetRenderTarget(0, m_lpBackupTargetSurface);
	STATEMANAGER.GetDevice()->SetDepthStencilSurface(m_lpDepthStencilBackup);
	m_lpDepthStencilBackup->Release();
	m_lpDepthStencilBackup = nullptr;
	m_lpBackupTargetSurface->Release();
	m_lpBackupTargetSurface = nullptr;

	D3DXMATRIX mWorld, mView, mProj, mIdentity;
	D3DXMatrixIdentity(&mIdentity);
	STATEMANAGER.GetDevice()->GetTransform(D3DTS_WORLD, &mWorld);
	STATEMANAGER.GetDevice()->GetTransform(D3DTS_VIEW, &mView);
	STATEMANAGER.GetDevice()->GetTransform(D3DTS_PROJECTION, &mProj);
	STATEMANAGER.GetDevice()->SetTransform(D3DTS_WORLD, &mIdentity);
	STATEMANAGER.GetDevice()->SetTransform(D3DTS_VIEW, &mIdentity);
	STATEMANAGER.GetDevice()->SetTransform(D3DTS_PROJECTION, &mIdentity);
	STATEMANAGER.SaveRenderState(D3DRS_ZENABLE, FALSE);
	STATEMANAGER.SaveRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);

	STATEMANAGER.GetDevice()->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0.0f);
	STATEMANAGER.GetDevice()->SetTexture(0, m_lpTarget);
	STATEMANAGER.GetDevice()->SetPixelShader(nullptr);
	STATEMANAGER.GetDevice()->SetVertexShader(nullptr);
	STATEMANAGER.GetDevice()->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
	STATEMANAGER.GetDevice()->SetStreamSource(0, m_lpScreenQuad, 0, sizeof(TRTTVertex));
	STATEMANAGER.GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

	STATEMANAGER.RestoreRenderState(D3DRS_MULTISAMPLEANTIALIAS);
	STATEMANAGER.RestoreRenderState(D3DRS_ZENABLE);
	STATEMANAGER.GetDevice()->SetTransform(D3DTS_WORLD, &mWorld);
	STATEMANAGER.GetDevice()->SetTransform(D3DTS_VIEW, &mView);
	STATEMANAGER.GetDevice()->SetTransform(D3DTS_PROJECTION, &mProj);

	return true;
}

bool CPostProcessingChain::Render()
{
	D3DXMATRIX mWorld, mView, mProj, mIdentity;
	D3DXMatrixIdentity(&mIdentity);
	STATEMANAGER.GetDevice()->GetTransform(D3DTS_WORLD, &mWorld);
	STATEMANAGER.GetDevice()->GetTransform(D3DTS_VIEW, &mView);
	STATEMANAGER.GetDevice()->GetTransform(D3DTS_PROJECTION, &mProj);
	STATEMANAGER.GetDevice()->SetTransform(D3DTS_WORLD, &mIdentity);
	STATEMANAGER.GetDevice()->SetTransform(D3DTS_VIEW, &mIdentity);
	STATEMANAGER.GetDevice()->SetTransform(D3DTS_PROJECTION, &mIdentity);
	STATEMANAGER.GetDevice()->SetRenderTarget(0, m_lpLastPassSurface);
	STATEMANAGER.SaveRenderState(D3DRS_ZENABLE, FALSE);
	STATEMANAGER.SaveRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);

	STATEMANAGER.GetDevice()->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0.0f);
	STATEMANAGER.GetDevice()->SetTexture(0, m_lpSourceTexture);
	STATEMANAGER.GetDevice()->SetPixelShader(nullptr);
	STATEMANAGER.GetDevice()->SetVertexShader(nullptr);
	STATEMANAGER.GetDevice()->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
	STATEMANAGER.GetDevice()->SetStreamSource(0, m_lpScreenQuad, 0, sizeof(TRTTVertex));
	STATEMANAGER.GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

	IDirect3DSurface9* pCurrentTarget = m_lpTargetSurface;

	STATEMANAGER.GetDevice()->SetTexture(1, m_lpSourceTexture);

	for (std::list<CPostProcessingEffect>::iterator it = m_kEffects.begin(); it != m_kEffects.end(); ++it)
	{
		D3DSURFACE_DESC kDesc;
		pCurrentTarget->GetDesc(&kDesc);

		D3DXVECTOR4 vScreenSize = D3DXVECTOR4(kDesc.Width, kDesc.Height, 0.0f, 0.0f);
		(*it).GetConstants()->SetFloatArray(STATEMANAGER.GetDevice(), "ScreenSize", reinterpret_cast<float*>(&vScreenSize), 4);

		if ((*it).Apply())
		{
			STATEMANAGER.GetDevice()->SetRenderTarget(0, pCurrentTarget);
			STATEMANAGER.GetDevice()->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0.0f);
			STATEMANAGER.GetDevice()->SetTexture(0, pCurrentTarget == m_lpLastPassSurface ? m_lpTarget : m_lpLastPass);
			STATEMANAGER.GetDevice()->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
			STATEMANAGER.GetDevice()->SetStreamSource(0, m_lpScreenQuad, 0, sizeof(TRTTVertex));
			STATEMANAGER.GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
			pCurrentTarget = pCurrentTarget == m_lpLastPassSurface ? m_lpTargetSurface : m_lpLastPassSurface;
		}
	}

	STATEMANAGER.GetDevice()->SetTexture(1, nullptr);

	if (pCurrentTarget == m_lpTargetSurface)
	{
		STATEMANAGER.GetDevice()->SetRenderTarget(0, m_lpTargetSurface);
		STATEMANAGER.GetDevice()->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0.0f);
		STATEMANAGER.GetDevice()->SetTexture(0, m_lpLastPass);
		STATEMANAGER.GetDevice()->SetPixelShader(nullptr);
		STATEMANAGER.GetDevice()->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
		STATEMANAGER.GetDevice()->SetStreamSource(0, m_lpScreenQuad, 0, sizeof(TRTTVertex));
		STATEMANAGER.GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
	}

	STATEMANAGER.RestoreRenderState(D3DRS_MULTISAMPLEANTIALIAS);
	STATEMANAGER.RestoreRenderState(D3DRS_ZENABLE);
	STATEMANAGER.GetDevice()->SetTransform(D3DTS_WORLD, &mWorld);
	STATEMANAGER.GetDevice()->SetTransform(D3DTS_VIEW, &mView);
	STATEMANAGER.GetDevice()->SetTransform(D3DTS_PROJECTION, &mProj);

	return true;
}

CPostProcessingEffect CPostProcessingChain::AddEffect(const char* c_pszFileName)
{
	CPostProcessingEffect kEffect;
	CMappedFile kFile;
	LPCVOID pData;
	if (!CEterPackManager::Instance().Get(kFile, c_pszFileName, &pData))
	{
		return kEffect;
	}
	if (!kEffect.CreateShader(kFile))
	{
		return CPostProcessingEffect();
	}
	AddEffect(kEffect);
	return kEffect;
}
bool CPostProcessingChain::AddEffect(const CPostProcessingEffect& c_kEffect)
{
	m_kEffects.push_back(c_kEffect);
	return true;
}
bool CPostProcessingChain::RemoveEffect(const CPostProcessingEffect& c_kEffect)
{
	bool bResult = false;
	for (std::list<CPostProcessingEffect>::iterator it = m_kEffects.begin(); it != m_kEffects.end(); ++it)
	{
		if ((*it) == c_kEffect)
		{
			m_kEffects.erase(it--);
			bResult = true;
		}
	}
	return bResult;
}

std::list<CPostProcessingEffect>& CPostProcessingChain::GetEffects()
{
	return m_kEffects;
}
#endif