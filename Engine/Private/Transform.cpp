#include "Transform.h"
#include "Shader.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
	,m_WorldMatrix(rhs.m_WorldMatrix)
	,m_fRotationPerSec(rhs.m_fRotationPerSec)
	,m_fSpeedPerSec(rhs.m_fSpeedPerSec)
{
}

HRESULT CTransform::Initialize_Prototype(_float fSpeedPerSec, _float fRotationPerSec, _vector vPosition)
{
	m_fSpeedPerSec = fSpeedPerSec;
	m_fRotationPerSec = fRotationPerSec;
	m_vPosition = vPosition;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	// Set_State(STATE_POSITION, m_vPosition);

	//_matrix			matTmp;
	/*
	matTmp = XMLoadFloat4x4(&m_WorldMatrix);*/


	/*_float3		vTmp = _float3(0.f, 0.f, 0.f);
	_float4		vTmp2 = _float4(0.f, 0.f, 0.f, 0.f);

	_vector		vTmp1 = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector		vTmp3 = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	XMVector3Normalize(vTmp1);

	_float		fDot = XMVectorGetX(XMVector3Dot(vTmp1, vTmp3));

	_float		fLength = XMVectorGetX(XMVector3Length(vTmp1));

	XMStoreFloat3(&vTmp, vTmp1);
	XMStoreFloat4(&vTmp2, vTmp1);*/

	return S_OK;
}

void CTransform::Set_Scaling(_float fScaleX, _float fScaleY, _float fScaleZ)
{
	//TODO 월드행렬 각 행의 길이는 각 x,y,z의 스케일과 같다.
	//! 그래서 기존의 행을 가져와서 스케일과 곱연산을 해주면 스케일이 달라지게 되는 원리
	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(CTransform::STATE_RIGHT)) * fScaleX);
	Set_State(STATE_UP,    XMVector3Normalize(Get_State(CTransform::STATE_UP)) * fScaleY);
	Set_State(STATE_LOOK,  XMVector3Normalize(Get_State(CTransform::STATE_LOOK)) * fScaleZ);
}

void CTransform::Go_Straight(_float fTimeDelta)
{
	//TODO 방향벡터를 만들어서 가게하면된다.

	//! 내 위치 벡터와 룩 벡터를 만들고. 바라보는 방향으로 가게 하는 것.
	_vector vPostion = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);

	//! 룩 벡터를 정규화하지 않았다면 바로 그 위치로 이동되버린다. 그래서 정규화 시킨 후에 바라보는 방향을 시간값에 비례한 속도로 이동시키는 것.
	vPostion += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	//! 위에서 연산을 끝낸 벡터를 실제 월드행렬의 위치벡터에게 적용시킨다. 
	Set_State(STATE_POSITION, vPostion);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vRight = Get_State(STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vRight = Get_State(STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	//TODO 방향벡터를 만들어서 가게하면된다.

		//! 내 위치 벡터와 룩 벡터를 만들고. 바라보는 방향으로 가게 하는 것.
	_vector vPostion = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);

	//! 룩 벡터를 정규화하지 않았다면 바로 그 위치로 이동되버린다. 그래서 정규화 시킨 후에 바라보는 방향을 시간값에 비례한 속도로 이동시키는 것.
	vPostion -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	//! 위에서 연산을 끝낸 벡터를 실제 월드행렬의 위치벡터에게 적용시킨다. 
	Set_State(STATE_POSITION, vPostion);
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	//TODO 임의의축으로 회전하게 하는 함수이다.
	
	_vector vRight = Get_State(STATE_RIGHT);
	_vector vUp	   = Get_State(STATE_UP);
	_vector vLook  = Get_State(STATE_LOOK);

	//! 코드를 보면 기존에 방향벡터들을 가져와서 회전시킨것. 스케일 값에 변화는 주지않고 회전만 시켰으니 스케일곱을 수행하지 않는다.

	_matrix	RotationMatrix = XMMatrixRotationAxis(vAxis,m_fRotationPerSec * fTimeDelta);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix)); 
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	//TODO 특정 각도로 바라보게 하는 함수이다.
	//! 기존 각도에 누적을 시키면 안된다. 스케일값을 고려한 항등행렬을 만들어서 특정각도를 주자.
	
	_float3 vScale = Get_Scaled();

	_vector vRight	= XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector vUp		= XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector vLook	= XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_matrix	RotationMatrix = XMMatrixRotationAxis(vAxis,fRadian);
	
	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Go_Target(_fvector vTargetPos, _float fTimeDelta, _float fSpare)
{
	//TODO 타겟 위치에 도달할때 까지 추적

	_vector vPosition	= Get_State(STATE_POSITION);
	_vector	vDir		= vTargetPos - vPosition;

	_float fDistance = XMVectorGetX(XMVector3Length(vDir));

	if(fDistance >= fSpare)
		vPosition += XMVector3Normalize(vDir) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);
}

void CTransform::Look_At(_fvector vTargetPos)
{
	_float3		vScale = Get_Scaled(); //! 기존 스케일값
	
	_vector		vPosition = Get_State(STATE_POSITION);
	
	_vector		vLook = XMVector3Normalize(vTargetPos - vPosition) * vScale.z;
	_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;
	_vector		vUp = XMVector3Normalize(XMVector3Cross(vLook,vRight)) * vScale.y;

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

void CTransform::Look_At_OnLand(_fvector vTargetPos)
{
	_float3		vScale = Get_Scaled();

	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = vTargetPos - vPosition;

	_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;

	vLook = XMVector3Normalize(XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f))) * vScale.z;

	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);
}

// Transform 저장
void CTransform::Write_Json(json& Out_Json)
{
	Out_Json.clear(); // 일단 앞서 저장하는 것 없이 Transform만 저장하니 클리어하고 저장시켜주자.

	Out_Json.emplace("Transform", m_WorldMatrix.m); // 자기 자신의 월드 매트릭스 저장
}

// Transform 불러오기
void CTransform::Load_FromJson(const json& In_Json)
{
	// "Transform"으로 못찾으면 그냥 나가기.
	if (In_Json.find("Transform") == In_Json.end())
	{
		return;
	}

	// 모두 순회하며 파일에 저장돼있던 월드 행렬 자기 자신에게 받기.
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m_WorldMatrix.m[i][j] = In_Json["Transform"][i][j];
		}
	}
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _float fSpeedPerSec, _float fRotationPerSec, _vector vPosition)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(fSpeedPerSec, fRotationPerSec, vPosition)))
	{
		MSG_BOX("Failed to Created : CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CTransform::Clone(void* pArg)
{
	CTransform* pInstance = new CTransform(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTransform::Free()
{
	int i = 0;
	__super::Free();
}
