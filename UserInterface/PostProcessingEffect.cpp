#include "stdafx.h"
#ifndef DISABLE_POST_PROCESSING
#include "../EterLib/StdAfx.h"
#include "../EterLib/StateManager.h"
#include "../EterPack/EterPackManager.h"
#include "PostProcessingEffect.h"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

CPostProcessingEffect::CPostProcessingEffect() : m_pPixelShader(nullptr), m_pConstants(nullptr)
{
}
CPostProcessingEffect::CPostProcessingEffect(const CPostProcessingEffect& c_kOld) : m_pPixelShader(nullptr), m_pConstants(nullptr), m_strName(c_kOld.m_strName), m_kConfig(c_kOld.m_kConfig)
{
	SetShader(c_kOld.m_pPixelShader, c_kOld.m_pConstants);
}
CPostProcessingEffect::CPostProcessingEffect(CPostProcessingEffect&& r_kOld) : m_pConstants(r_kOld.m_pConstants), m_pPixelShader(r_kOld.m_pPixelShader), m_strName(std::move(r_kOld.m_strName)), m_kConfig(std::move(r_kOld.m_kConfig))
{
	r_kOld.m_pPixelShader = nullptr;
	r_kOld.m_pConstants = nullptr;
}
CPostProcessingEffect::~CPostProcessingEffect()
{
	SetShader(nullptr, nullptr);
}

CPostProcessingEffect& CPostProcessingEffect::operator=(const CPostProcessingEffect& c_kOld)
{
	if (&c_kOld == this)
	{
		return *this;
	}

	m_kConfig = c_kOld.m_kConfig;
	SetShader(c_kOld.m_pPixelShader, c_kOld.m_pConstants);
	m_strName = c_kOld.m_strName;

	return *this;
}
CPostProcessingEffect& CPostProcessingEffect::operator=(CPostProcessingEffect&& r_kOld)
{
	if (&r_kOld == this)
	{
		return *this;
	}

	if (m_pPixelShader)
	{
		m_pPixelShader->Release();
	}
	if (m_pConstants)
	{
		m_pConstants->Release();
	}
	m_kConfig = std::move(r_kOld.m_kConfig);
	m_pPixelShader = r_kOld.m_pPixelShader;
	m_pConstants = r_kOld.m_pConstants;
	m_strName = std::move(r_kOld.m_strName);
	r_kOld.m_pPixelShader = nullptr;
	r_kOld.m_pConstants = nullptr;

	return *this;
}

bool CPostProcessingEffect::operator==(const char* c_pszName) const
{
	return !stricmp(m_strName.c_str(), c_pszName);
}
bool CPostProcessingEffect::operator==(const CPostProcessingEffect& c_kOld) const
{
	return !stricmp(m_strName.c_str(), c_kOld.GetName());
}

void CPostProcessingEffect::SetName(const char* c_pszName)
{
	m_strName = c_pszName;
}
const char* CPostProcessingEffect::GetName() const
{
	return m_strName.c_str();
}

bool CPostProcessingEffect::CreateShader(CMappedFile& rkDescriptor)
{
	char* pszBuffer = new char[rkDescriptor.Size() + 1];
	rkDescriptor.Read(pszBuffer, rkDescriptor.Size());
	pszBuffer[rkDescriptor.Size()] = 0;

	std::stringstream kDescriptorConfig;
	kDescriptorConfig << pszBuffer;
	delete [] pszBuffer;

	try
	{
		read_xml(kDescriptorConfig, m_kConfig);
	}
	catch (std::exception& e)
	{
		TraceError("%s", e.what());
		return false;
	}

	m_strName = m_kConfig.get<std::string>("Effect.<xmlattr>.Name", "<shader>");

	CMappedFile kFile;
	LPCVOID pData;
	if (!CEterPackManager::Instance().Get(kFile, m_kConfig.get<std::string>("Effect.<xmlattr>.Source").c_str(), &pData))
	{
		TraceError("CPostProcessingEffect unable to load effect %s", m_kConfig.get<std::string>("Effect.<xmlattr>.Source").c_str());
		return false;
	}

	LPD3DXBUFFER pCode;
	LPD3DXBUFFER pError = nullptr;
	if (FAILED(D3DXCompileShader(reinterpret_cast<const char*>(pData), strlen(reinterpret_cast<const char*>(pData)), nullptr, nullptr, m_kConfig.get<std::string>("Effect.<xmlattr>.Entry", "EffectEntry").c_str(), "ps_2_0", 0, &pCode, &pError, &m_pConstants)))
	{
		char* pszError = new char[pError->GetBufferSize() + 1];
		memcpy(pszError, pError->GetBufferPointer(), pError->GetBufferSize());
		pszError[pError->GetBufferSize()] = 0;
		pError->Release();
		TraceError("CPostProcessingEffect unable to compile effect %s: %s", m_kConfig.get<std::string>("Effect.<xmlattr>.Source").c_str(), pszError);
		delete [] pszError;
		return false;
	}
	if (pError)
	{
		pError->Release();
	}
	if (FAILED(STATEMANAGER.GetDevice()->CreatePixelShader(reinterpret_cast<DWORD*>(pCode->GetBufferPointer()), &m_pPixelShader)))
	{
		TraceError("CPostProcessingEffect unable to create effect instance %s", m_kConfig.get<std::string>("Effect.<xmlattr>.Source").c_str());
		pCode->Release();
		m_pConstants->Release();
		return false;
	}
	pCode->Release();
	return true;
}
void CPostProcessingEffect::SetShader(IDirect3DPixelShader9* lpShader, LPD3DXCONSTANTTABLE lpConstants)
{
	if (m_pPixelShader)
	{
		m_pPixelShader->Release();
	}
	if (m_pConstants)
	{
		m_pConstants->Release();
	}
	m_pPixelShader = lpShader;
	m_pConstants = lpConstants;
	if (m_pPixelShader)
	{
		m_pPixelShader->AddRef();
	}
	if (m_pConstants)
	{
		m_pConstants->AddRef();
	}
}
IDirect3DPixelShader9* CPostProcessingEffect::GetShader()
{
	return m_pPixelShader;
}
LPD3DXCONSTANTTABLE CPostProcessingEffect::GetConstants()
{
	return m_pConstants;
}

bool CPostProcessingEffect::Apply()
{
	try
	{
		BOOST_FOREACH(boost::property_tree::ptree::value_type& vt, m_kConfig.get_child("Effect"))
		{
			if (vt.first == "Bool")
			{
				m_pConstants->SetBool(STATEMANAGER.GetDevice(), vt.second.get<std::string>("<xmlattr>.Name").c_str(), vt.second.get<bool>("<xmlattr>.Value", false));
			}
			else if (vt.first == "BoolArray")
			{
				std::vector<BOOL> kArray;
				BOOST_FOREACH(boost::property_tree::ptree::value_type& v, vt.second)
				{
					if (v.first == "Bool")
					{
						kArray.push_back(v.second.get<bool>("<xmlattr>.Value", false));
					}
				}
				m_pConstants->SetBoolArray(STATEMANAGER.GetDevice(), vt.second.get<std::string>("<xmlattr>.Name").c_str(), &kArray[0], kArray.size());
			}
			else if (vt.first == "Float")
			{
				m_pConstants->SetFloat(STATEMANAGER.GetDevice(), vt.second.get<std::string>("<xmlattr>.Name").c_str(), vt.second.get<float>("<xmlattr>.Value", 0.0f));
			}
			else if (vt.first == "FloatArray")
			{
				std::vector<float> kArray;
				BOOST_FOREACH(boost::property_tree::ptree::value_type& v, vt.second)
				{
					if (v.first == "Float")
					{
						kArray.push_back(v.second.get<float>("<xmlattr>.Value", 0.0f));
					}
				}
				m_pConstants->SetFloatArray(STATEMANAGER.GetDevice(), vt.second.get<std::string>("<xmlattr>.Name").c_str(), &kArray[0], kArray.size());
			}
			else if (vt.first == "Int")
			{
				m_pConstants->SetInt(STATEMANAGER.GetDevice(), vt.second.get<std::string>("<xmlattr>.Name").c_str(), vt.second.get<int>("<xmlattr>.Value", 0));
			}
			else if (vt.first == "IntArray")
			{
				std::vector<int> kArray;
				BOOST_FOREACH(boost::property_tree::ptree::value_type& v, vt.second)
				{
					if (v.first == "Int")
					{
						kArray.push_back(v.second.get<int>("<xmlattr>.Value", 0));
					}
				}
				m_pConstants->SetIntArray(STATEMANAGER.GetDevice(), vt.second.get<std::string>("<xmlattr>.Name").c_str(), &kArray[0], kArray.size());
			}
			else if (vt.first == "Matrix")
			{
				D3DMATRIX mVar;
				mVar.m[0][0] = vt.second.get<float>("<xmlattr>.m0", 0.0f);
				mVar.m[0][1] = vt.second.get<float>("<xmlattr>.m1", 0.0f);
				mVar.m[0][2] = vt.second.get<float>("<xmlattr>.m2", 0.0f);
				mVar.m[0][3] = vt.second.get<float>("<xmlattr>.m3", 0.0f);
				mVar.m[1][0] = vt.second.get<float>("<xmlattr>.m4", 0.0f);
				mVar.m[1][1] = vt.second.get<float>("<xmlattr>.m5", 0.0f);
				mVar.m[1][2] = vt.second.get<float>("<xmlattr>.m6", 0.0f);
				mVar.m[1][3] = vt.second.get<float>("<xmlattr>.m7", 0.0f);
				mVar.m[2][0] = vt.second.get<float>("<xmlattr>.m8", 0.0f);
				mVar.m[2][1] = vt.second.get<float>("<xmlattr>.m9", 0.0f);
				mVar.m[2][2] = vt.second.get<float>("<xmlattr>.m10", 0.0f);
				mVar.m[2][3] = vt.second.get<float>("<xmlattr>.m11", 0.0f);
				mVar.m[3][0] = vt.second.get<float>("<xmlattr>.m12", 0.0f);
				mVar.m[3][1] = vt.second.get<float>("<xmlattr>.m13", 0.0f);
				mVar.m[3][2] = vt.second.get<float>("<xmlattr>.m14", 0.0f);
				mVar.m[3][3] = vt.second.get<float>("<xmlattr>.m15", 0.0f);
				m_pConstants->SetMatrix(STATEMANAGER.GetDevice(), vt.second.get<std::string>("<xmlattr>.Name").c_str(), reinterpret_cast<D3DXMATRIX*>(&mVar));
			}
			else if (vt.first == "MatrixArray")
			{
				std::vector<D3DMATRIX> kArray;
				BOOST_FOREACH(boost::property_tree::ptree::value_type& v, vt.second)
				{
					if (v.first == "Matrix")
					{
						D3DMATRIX mVar;
						mVar.m[0][0] = v.second.get<float>("<xmlattr>.m0", 0.0f);
						mVar.m[0][1] = v.second.get<float>("<xmlattr>.m1", 0.0f);
						mVar.m[0][2] = v.second.get<float>("<xmlattr>.m2", 0.0f);
						mVar.m[0][3] = v.second.get<float>("<xmlattr>.m3", 0.0f);
						mVar.m[1][0] = v.second.get<float>("<xmlattr>.m4", 0.0f);
						mVar.m[1][1] = v.second.get<float>("<xmlattr>.m5", 0.0f);
						mVar.m[1][2] = v.second.get<float>("<xmlattr>.m6", 0.0f);
						mVar.m[1][3] = v.second.get<float>("<xmlattr>.m7", 0.0f);
						mVar.m[2][0] = v.second.get<float>("<xmlattr>.m8", 0.0f);
						mVar.m[2][1] = v.second.get<float>("<xmlattr>.m9", 0.0f);
						mVar.m[2][2] = v.second.get<float>("<xmlattr>.m10", 0.0f);
						mVar.m[2][3] = v.second.get<float>("<xmlattr>.m11", 0.0f);
						mVar.m[3][0] = v.second.get<float>("<xmlattr>.m12", 0.0f);
						mVar.m[3][1] = v.second.get<float>("<xmlattr>.m13", 0.0f);
						mVar.m[3][2] = v.second.get<float>("<xmlattr>.m14", 0.0f);
						mVar.m[3][3] = v.second.get<float>("<xmlattr>.m15", 0.0f);
						kArray.push_back(mVar);
					}
				}
				m_pConstants->SetMatrixArray(STATEMANAGER.GetDevice(), vt.second.get<std::string>("<xmlattr>.Name").c_str(), reinterpret_cast<D3DXMATRIX*>(&kArray[0]), kArray.size());
			}
			else if (vt.first == "MatrixTranspose")
			{
				D3DMATRIX mVar;
				mVar.m[0][0] = vt.second.get<float>("<xmlattr>.m0", 0.0f);
				mVar.m[0][1] = vt.second.get<float>("<xmlattr>.m1", 0.0f);
				mVar.m[0][2] = vt.second.get<float>("<xmlattr>.m2", 0.0f);
				mVar.m[0][3] = vt.second.get<float>("<xmlattr>.m3", 0.0f);
				mVar.m[1][0] = vt.second.get<float>("<xmlattr>.m4", 0.0f);
				mVar.m[1][1] = vt.second.get<float>("<xmlattr>.m5", 0.0f);
				mVar.m[1][2] = vt.second.get<float>("<xmlattr>.m6", 0.0f);
				mVar.m[1][3] = vt.second.get<float>("<xmlattr>.m7", 0.0f);
				mVar.m[2][0] = vt.second.get<float>("<xmlattr>.m8", 0.0f);
				mVar.m[2][1] = vt.second.get<float>("<xmlattr>.m9", 0.0f);
				mVar.m[2][2] = vt.second.get<float>("<xmlattr>.m10", 0.0f);
				mVar.m[2][3] = vt.second.get<float>("<xmlattr>.m11", 0.0f);
				mVar.m[3][0] = vt.second.get<float>("<xmlattr>.m12", 0.0f);
				mVar.m[3][1] = vt.second.get<float>("<xmlattr>.m13", 0.0f);
				mVar.m[3][2] = vt.second.get<float>("<xmlattr>.m14", 0.0f);
				mVar.m[3][3] = vt.second.get<float>("<xmlattr>.m15", 0.0f);
				m_pConstants->SetMatrixTranspose(STATEMANAGER.GetDevice(), vt.second.get<std::string>("<xmlattr>.Name").c_str(), reinterpret_cast<D3DXMATRIX*>(&mVar));
			}
			else if (vt.first == "MatrixTransposeArray")
			{
				std::vector<D3DMATRIX> kArray;
				BOOST_FOREACH(boost::property_tree::ptree::value_type& v, vt.second)
				{
					if (v.first == "Matrix")
					{
						D3DMATRIX mVar;
						mVar.m[0][0] = v.second.get<float>("<xmlattr>.m0", 0.0f);
						mVar.m[0][1] = v.second.get<float>("<xmlattr>.m1", 0.0f);
						mVar.m[0][2] = v.second.get<float>("<xmlattr>.m2", 0.0f);
						mVar.m[0][3] = v.second.get<float>("<xmlattr>.m3", 0.0f);
						mVar.m[1][0] = v.second.get<float>("<xmlattr>.m4", 0.0f);
						mVar.m[1][1] = v.second.get<float>("<xmlattr>.m5", 0.0f);
						mVar.m[1][2] = v.second.get<float>("<xmlattr>.m6", 0.0f);
						mVar.m[1][3] = v.second.get<float>("<xmlattr>.m7", 0.0f);
						mVar.m[2][0] = v.second.get<float>("<xmlattr>.m8", 0.0f);
						mVar.m[2][1] = v.second.get<float>("<xmlattr>.m9", 0.0f);
						mVar.m[2][2] = v.second.get<float>("<xmlattr>.m10", 0.0f);
						mVar.m[2][3] = v.second.get<float>("<xmlattr>.m11", 0.0f);
						mVar.m[3][0] = v.second.get<float>("<xmlattr>.m12", 0.0f);
						mVar.m[3][1] = v.second.get<float>("<xmlattr>.m13", 0.0f);
						mVar.m[3][2] = v.second.get<float>("<xmlattr>.m14", 0.0f);
						mVar.m[3][3] = v.second.get<float>("<xmlattr>.m15", 0.0f);
						kArray.push_back(mVar);
					}
				}
				m_pConstants->SetMatrixTransposeArray(STATEMANAGER.GetDevice(), vt.second.get<std::string>("<xmlattr>.Name").c_str(), reinterpret_cast<D3DXMATRIX*>(&kArray[0]), kArray.size());
			}
			else if (vt.first == "Vector")
			{
				D3DXVECTOR4 vVar;
				vVar.x = vt.second.get<float>("<xmlattr>.x", 0.0f);
				vVar.y = vt.second.get<float>("<xmlattr>.y", 0.0f);
				vVar.z = vt.second.get<float>("<xmlattr>.z", 0.0f);
				vVar.w = vt.second.get<float>("<xmlattr>.w", 0.0f);
				m_pConstants->SetVector(STATEMANAGER.GetDevice(), vt.second.get<std::string>("<xmlattr>.Name").c_str(), &vVar);
			}
			else if (vt.first == "VectorArray")
			{
				std::vector<D3DXVECTOR4> kArray;
				BOOST_FOREACH(boost::property_tree::ptree::value_type& v, vt.second)
				{
					if (v.first == "Matrix")
					{
						D3DXVECTOR4 vVar;
						vVar.x = v.second.get<float>("<xmlattr>.x", 0.0f);
						vVar.y = v.second.get<float>("<xmlattr>.y", 0.0f);
						vVar.z = v.second.get<float>("<xmlattr>.z", 0.0f);
						vVar.w = v.second.get<float>("<xmlattr>.w", 0.0f);
						kArray.push_back(vVar);
					}
				}
				m_pConstants->SetVectorArray(STATEMANAGER.GetDevice(), vt.second.get<std::string>("<xmlattr>.Name").c_str(), &kArray[0], kArray.size());
			}
		}
	}
	catch (std::exception& e)
	{
		TraceError("CPostProcessingEffect::Apply (%s): %s", m_strName.c_str(), e.what());
	}

	STATEMANAGER.GetDevice()->SetPixelShader(m_pPixelShader);

	return true;
}
#endif