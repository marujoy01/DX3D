#pragma once

#include "Component.h"

//TODO 트랜스폼 컴객체의 역할
//! 객체들의 월드 상태를 표현하기 위한 데이터를 가진다. ( 월드행렬 )
//! 해당 월드 상에서의 변환을 위한 기능을 가진다.

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	//! 행렬 행 정보 교체
	void Set_State(STATE eState, const _float4& vState)
	{
		memcpy(&m_WorldMatrix.m[eState],&vState, sizeof(_float3));
	}

	void Set_State(STATE eState, _fvector vState)
	{
		m_WorldMatrix.m[eState][0] = XMVectorGetX(vState);
		m_WorldMatrix.m[eState][1] = XMVectorGetY(vState);
		m_WorldMatrix.m[eState][2] = XMVectorGetZ(vState);
		m_WorldMatrix.m[eState][3] = XMVectorGetW(vState);
	}

	_vector Get_State(STATE eState)
	{
		return XMVectorSet
			(
				m_WorldMatrix.m[eState][0],
				m_WorldMatrix.m[eState][1],
				m_WorldMatrix.m[eState][2],
				m_WorldMatrix.m[eState][3]
			);
	}

	_float3 Get_Scaled()
	{
		return _float3
		(
			XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_WorldMatrix).r[STATE_RIGHT])),
			XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_WorldMatrix).r[STATE_UP])),
			XMVectorGetX(XMVector3Length(XMLoadFloat4x4(&m_WorldMatrix).r[STATE_LOOK]))
		);
	}

	_matrix Get_WorldMatrix()
	{
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	_float4x4 Get_WorldFloat4x4()
	{
		return m_WorldMatrix;
	}

	void Set_WorldFloat4x4(_float4x4 _World) 
	{
		XMStoreFloat4x4(&m_WorldMatrix, XMLoadFloat4x4(&_World));
	}

	_matrix Get_WorldMatrixInverse()
	{
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}

	_float4x4 Get_WorldFloat4x4Inverse()
	{
		_float4x4 InverseMatrix;
		XMStoreFloat4x4(&InverseMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

		return InverseMatrix;
	}

	void Set_Scaling(_float fScaleX, _float fScaleY, _float fScaleZ);

public:
	void	Go_Straight(_float fTimeDelta);
	void	Go_Left(_float fTimeDelta);
	void	Go_Right(_float fTimeDelta);
	void	Go_Backward(_float fTimeDelta);
	void	Turn(_fvector vAxis, _float fTimeDelta);
	void	Rotation(_fvector vAxis, _float fRadian);
	void	Go_Target(_fvector vTargetPos, _float fTimeDelta, _float fSpare = 0.1f);
	void	Look_At(_fvector vTargetPos);
	void	Look_At_OnLand(_fvector vTargetPos);

public: // 저장, 불러오기
	virtual void Write_Json(json& Out_Json) override;
	virtual void Load_FromJson(const json& In_Json) override;

public:
	virtual HRESULT Initialize_Prototype(_float fSpeedPerSec, _float fRotationPerSec, _vector vPosition);
	
public:
	HRESULT	Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);

private:
	_float				m_fSpeedPerSec = { 0.0f };
	_float				m_fRotationPerSec = { 0.0f };
	_vector				m_vPosition = { 0.0f, 0.0f, 0.0f, 0.0f };

	_float4x4			m_WorldMatrix;

public:
	static	CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _float fSpeedPerSec, _float fRotationPerSec, _vector vPosition);
	virtual CComponent* Clone(void* pArg) override; //! 복사를 강제한다
	virtual void Free() override;
};

END

