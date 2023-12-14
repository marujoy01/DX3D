#include "PipeLine.h"
#include "GameInstance.h"

CPipeLine::CPipeLine()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

void CPipeLine::Set_Transform(D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	XMStoreFloat4x4(&m_Transform[eState], TransformMatrix);
}

void CPipeLine::Set_Transform(D3DTRANSFORMSTATE eState, _float4x4 TransformMatrix)
{
	m_Transform[eState] = TransformMatrix;
}

_matrix CPipeLine::Get_TransformMatrix(D3DTRANSFORMSTATE eState)
{
	return XMLoadFloat4x4(&m_Transform[eState]);
}

_float4x4 CPipeLine::Get_TransformFloat4x4(D3DTRANSFORMSTATE eState)
{
	return m_Transform[eState];
}

_matrix CPipeLine::Get_TransformMatrixInverse(D3DTRANSFORMSTATE eState)
{
	return XMLoadFloat4x4(&m_Transform_Inverse[eState]);
}

_float4x4 CPipeLine::Get_TransformFloat4x4Inverse(D3DTRANSFORMSTATE eState)
{
	return m_Transform_Inverse[eState];
}

_float4 CPipeLine::Get_CamPosition()
{
	return m_vCamPosition;
}

RAY CPipeLine::Get_MouseRay_World(const _uint& In_ViewPortWidth, const _uint& In_ViewPortHeight)
{
	
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(m_pGameInstance->Get_GraphicDesc().hWnd, &ptMouse);

	_vector vProjMouse(XMVectorSet(ptMouse.x / (In_ViewPortWidth * 0.5f) - 1.f, ptMouse.y / -(In_ViewPortHeight * 0.5f) + 1.f, 0.f, 0.f));

	_matrix mProjCameaInv(Get_TransformMatrix(D3DTS_PROJ));
	mProjCameaInv = XMMatrixInverse(nullptr, mProjCameaInv);

	_vector vViewMouse(XMVector3TransformCoord(vProjMouse, mProjCameaInv));

	_vector vRayDir(vViewMouse);
	_vector vRayPos(XMVectorSet(0.f, 0.f, 0.f, 1.f));

	_matrix	ViewCameraInv(Get_TransformMatrix(D3DTS_VIEW));
	ViewCameraInv = XMMatrixInverse(nullptr, ViewCameraInv);

	RAY MouseRay;
	ZeroMemory(&MouseRay, sizeof(RAY));

	XMStoreFloat3(&MouseRay.vDirection, XMVector3Normalize(XMVector3TransformNormal(vRayDir, ViewCameraInv)));
	XMStoreFloat4(&MouseRay.vOrigin, XMVector3TransformCoord(vRayPos, ViewCameraInv));

	MouseRay.fLength = 1000000.0f;

	return MouseRay;
}

HRESULT CPipeLine::Initialize()
{
	//! 루프 돌아서 항등행렬로 초기화
	for (size_t i = 0; i < D3DTS_END; i++)
	{
		XMStoreFloat4x4(&m_Transform[i], XMMatrixIdentity());
		XMStoreFloat4x4(&m_Transform_Inverse[i], XMMatrixIdentity());
	}
	
	return S_OK;
}

void CPipeLine::Tick()
{
	//! 매 틱마다 카메라의 역행렬(월드행렬) 멤버변수 채워주기
	for (size_t i = 0; i < D3DTS_END; i++)
	{
		XMStoreFloat4x4(&m_Transform_Inverse[i], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_Transform[i])));
	}

	//! 위에서 구한 카메라의 월드행렬의 위치에 해당하는 4행을 위치 멤버변수에 셋팅해준다.
	memcpy(&m_vCamPosition,&m_Transform_Inverse[D3DTS_VIEW].m[3], sizeof(_float4));
}

CPipeLine* CPipeLine::Create()
{
	CPipeLine* pInstance = new CPipeLine;

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CPipeLine");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPipeLine::Free()
{
	Safe_Release(m_pGameInstance);
}
