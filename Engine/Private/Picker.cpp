#include "..\Public\Picker.h"

IMPLEMENT_SINGLETON(CPicker) // 싱글톤

CPicker::CPicker()
{
}

CPicker::~CPicker()
{
}

HRESULT CPicker::Initialize(HWND _hWnd)
{
	// 파이프라인 생성
	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	return S_OK;
}

// 매 프레임마다 마우스 위치 좌표를 구하기위해 Tick으로 구한다.
void CPicker::Tick()
{
	POINT	ptCursor;	// 마우스 좌표를 담을 그릇
	RECT	rcClient;	// 클라이언트
	GetCursorPos(&ptCursor); // 마우스 좌표
	ScreenToClient(m_hWnd, &ptCursor);	// 뷰 포트
	GetClientRect(m_hWnd, &rcClient);	// 화면(클라이언트)

	// 화면에서의 마우스 위치 좌표를 계산한다.
	//_float3		vCursor(2.f * ptCursor.x / rcClient.right - 1.f, -2.f * ptCursor.y / rcClient.bottom + 1.f, 0.f);
	_vector		vCursor = { 2.f * ptCursor.x / rcClient.right - 1.f,
							-2.f * ptCursor.y / rcClient.bottom + 1.f,
							0.f };

	// 파이프라인에 저장돼있던 뷰, 투영을 받아온다.
	_matrix	mView = m_pPipeLine->Get_TransformMatrixInverse(m_pPipeLine->D3DTS_VIEW);
	_matrix	mProj = m_pPipeLine->Get_TransformMatrixInverse(m_pPipeLine->D3DTS_PROJ);

	// XMVector3TransformCoord(vCurs, mProj); 왜 안됐는지 모름, 갑자기 됨
	_vector		vWorld = { 0.f, 0.f, 0.f };

	// 월드상의 광선 위치, 방향을 저장한다.
	m_vRay[IDX(PICKER::WORLD)] = _float3(XMVector3Normalize(XMVector3TransformNormal(XMVector3TransformCoord(vCursor, mProj), mView)).m128_f32); // 커서를 뷰 스페이스까지 내리고, 월드 스페이스랑 곱해서 월드까지 내려준다.
	m_vOrg[IDX(PICKER::WORLD)] = _float3(XMVector3TransformCoord(vWorld, mView).m128_f32); // 월드 스페이스와 임의 값 0, 0, 0을 곱해 월드로 내린다.

	// 여기서는 값만 넘겨준다.
}

void CPicker::Render()
{

}

//						(계산된 값을 반환받을 녀석, 인덱스A, 인덱스B, 인덱스C, 월드)
_bool CPicker::Intersect(_Out_ _float3& _vOut, const _float3 _vA, const _float3 _vB, const _float3 _vC, _In_opt_ const _float4x4 _mWorld)
{
	_vOut = _float3(0.f, 0.f, 0.f); // 기본값

	_matrix	mUnit = XMMatrixIdentity();

	_matrix mWorld = XMLoadFloat4x4(&_mWorld);

	_vector vOrg = XMLoadFloat3(&m_vOrg[IDX(PICKER::WORLD)]);

	_vector vRay = XMLoadFloat3(&m_vRay[IDX(PICKER::WORLD)]);



	// 받은 월드 행렬 인자값이 어떤 값인지 판단하고 반환한다. -> 받은 월드 인자값이 g_mUnit과 같다면 WORLD, 아니라면 LOCAL 반환
	PICKER eSystem = XMMatrixIsIdentity(mUnit) ? PICKER::WORLD : PICKER::LOCAL;
	
	// 만약 받은 값이 월드 행렬일 경우
	//if (PICKER::WORLD == eSystem)
	{ // 로컬까지 내려서 광선의 위치와 방향을 저장준다.
		XMStoreFloat3(&m_vOrg[IDX(PICKER::LOCAL)], XMVector3TransformCoord(vOrg, mWorld));
		XMStoreFloat3(&m_vRay[IDX(PICKER::LOCAL)], XMVector3TransformCoord(vRay, mWorld));
	}

	//		거리
	_float fDist;

	_vector vA = XMLoadFloat3(&_vA);
	_vector vB = XMLoadFloat3(&_vB);
	_vector vC = XMLoadFloat3(&_vC);

	// Intersects : 우리가 직접 했던 마우스 광선과 충돌체의 거리계산을 알아서 구하고 반환해준다.
	// 충돌을 안했으면 0을 리턴, 충돌을 했으면 값(fDist)과 함께 1을 리턴한다. -> 조건문 안에 바로 넣어 사용가능.
	if (DirectX::TriangleTests::Intersects(vOrg, vRay, vA, vB, vC, fDist))
	{
		// 결과물 = 광선 위치 + 방향 * 거리(길이)
		_vector vOut = vOrg + vRay * fDist;

		XMStoreFloat3(&_vOut, vOut);

		//if (PICKER::WORLD == eSystem)
		{
			XMStoreFloat3(&_vOut, XMVector3TransformCoord(XMLoadFloat3(&_vOut), mWorld));
		}

		return true;
	}

	return false;
}
