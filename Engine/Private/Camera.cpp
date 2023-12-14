#include "Camera.h"
#include "GameInstance.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice,pContext)
{
}

CCamera::CCamera(const CCamera& rhs)
	: CGameObject(rhs)
{
}

HRESULT CCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Initialize(void* pArg)
{
	if(nullptr == pArg)
		return E_FAIL;
	
	//TODO 부모클래스인 게임오브젝트의 Initialize 함수를 호출했다면 Transform 컴포넌트를 만들었다는 의미와도 같다.
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CAMERA_DESC*	pCameraDesc = (CAMERA_DESC*)pArg;

	//! 위에서 만든 트랜스폼은 항등상태일 것이다. 내가 셋팅하고자 하는 카메라의 상태로 동기화시켜주자.
	//! 정쌤은 처음에 XMLoadFloat3를 주려했다. 하지만 그렇게하면 W값이 0으로 채워지기때문에 안된다 다시 Float4 형태로 바꿔줬다.
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&pCameraDesc->vEye)); //! 카메라의 위치니까 Transform에 POSITION에 바인딩
	m_pTransformCom->Look_At(XMLoadFloat4(&pCameraDesc->vAt)); //! vAt이 카메라가 바라보는 방향이니 Transform의 Look_At 함수 호출해서 바라보게 해주자.
	
	m_fFovy = pCameraDesc->fFovY;
	m_fAspect = pCameraDesc->fAspect;
	m_fNear = pCameraDesc->fNear;
	m_fFar = pCameraDesc->fFar;
	
	//! 32,33번 라인을 보면 vEye,vAt을 멤버변수로 두지않는 이유를 알 수 있다. 한번만 셋팅할거니까 트랜스폼에 주고 트랜스폼으로 제어하기 위해 바인딩 한 것이다.

	//TODO 투영행렬 루프를 돌건데
	//! 카메라 월드 행렬의 역행렬은 카메라의 뷰 행렬이다. 모든 객체는 뷰 행렬(카메라 행렬)을 거쳐야 하는데, 오브젝트매니저에서 카메라 컴포넌트를 탐색 후 
	//! 트랜스폼을 꺼내와서 트랜스폼이 가지고 있는 월드행렬의 역행렬 식으로 접근하는것도 가능은 하겠지만, 매 번, 객체들이 그려질때마다 검색을 해야한다.
	//! 그래서 다른객체들이 쉽게 뷰와 투영행렬을 얻어올 수 있는 작업이 필요했다.
	//! dx9때는 그래픽디바이스에 GetTransform 함수를 호출해서 뷰,투영을 얻어올 수 있었다.
	//! 그 역할을 대신해줄 파이프라인이라는 클래스를 만들 것이다.
	return S_OK;
}

void CCamera::Priority_Tick(_float fTimeDelta)
{
}

void CCamera::Tick(_float fTimeDelta)
{
	//! 파이프라인에게 자기자신(카메라) 뷰행렬 (월드 역행렬), 투영행렬 던져주자
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrixInverse());
	m_pGameInstance->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fFovy, m_fAspect, m_fNear, m_fFar));
}

void CCamera::Late_Tick(_float fTimeDelta)
{
}


void CCamera::Free()
{
	__super::Free();
}
