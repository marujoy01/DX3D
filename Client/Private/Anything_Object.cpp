#include "stdafx.h"
#include "..\Public\Anything_Object.h"

#include "GameInstance.h"

CAnything_Object::CAnything_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{

}

CAnything_Object::CAnything_Object(const CAnything_Object& rhs)
	: CGameObject(rhs)
	, m_eCurrentLevelID(rhs.m_eCurrentLevelID)	// 복사 생성할때 원본이 갖고있는 멤버 변수가 있다면 직접 전해줘야한다. 
												// 이거때문에 레벨을 계속 못받아서 Shader컴포넌트를 만들지 못하고 터짐.
{
}

HRESULT CAnything_Object::Initialize_Prototype(LEVEL eLevel)
{
	// 레벨 구분
	m_eCurrentLevelID = eLevel;

	return S_OK;
}

HRESULT CAnything_Object::Initialize(void* pArg) // 값이 안받아와짐
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	/* 여기서 원하는 값을 세팅 하거나 */
	//MONSTER_DESC* pDesc = (MONSTER_DESC*)pArg;
	//
	//m_pTransformCom->Set_State(vPos)

	ANYTHINGOBJ* pAnyInfo = (ANYTHINGOBJ*)pArg;

	// 위치 값
	m_vPosition = pAnyInfo->vPosition;

	// 모델 컴포넌트 테그
	m_wModelTag = pAnyInfo->cModelTag;
	m_wShaderTag = pAnyInfo->cShaderTag;
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pModelCom->Set_AnimChange(0);

	// 이 녀석의 TransformCom을 이용해 위치를 지정해주자.
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPosition);

	return S_OK;
}

void CAnything_Object::Priority_Tick(_float fTimeDelta)
{



}

void CAnything_Object::Tick(_float fTimeDelta)
{
	// 스페이스바 눌렀는데
	if (m_pGameInstance->Key_Down(DIK_SPACE))
	{
		if (m_bAnimLoop) // 켜져있었으면
		{
			m_bAnimLoop = false; // 끄고
		}
		else // 꺼져있었으면
		{
			m_bAnimLoop = true; // 켜고
		}
	}

	if (m_pGameInstance->Key_Down(DIK_LEFT))
	{
		m_pModelCom->Set_AnimChange(-1);
	}
	if (m_pGameInstance->Key_Down(DIK_RIGHT))
	{
		m_pModelCom->Set_AnimChange(1);
	}


	// 애니메이션을 반복 재생할지 말지 결정하기 위해 bool값을 던져주자.
	m_pModelCom->Play_Animation(fTimeDelta, true); // 뼈들이 갖고있는 애니메이션(Transform)에 맞게 모델을 움직인다.
}

void CAnything_Object::Late_Tick(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this)))
		return;
}

HRESULT CAnything_Object::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i); // 뼈 매트릭스 추가

		m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE);

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

// 이 클래스가 사용할 컴포넌트들을 준비하자.
HRESULT CAnything_Object::Ready_Components()
{// 셰이더 관련 함수와 모델만 추가했다.
	///* For.Com_Shader */	// 셰이더 컴포넌트 추가 
	//if (FAILED(__super::Add_Component(m_eCurrentLevelID, TEXT("Prototype_Component_Shader_Model"), // 에러 복사생성자에 멤버변수를 받지 않아서 레벨이 달랐음
	//	TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	//	return E_FAIL;

	// 이제 애니메이션 모델로 생성하기 위해, 위 셰이더 Model 컴포넌트를 셰이더 AnimModel 컴포넌트로 바꿔주자!
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, m_wShaderTag,
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Model */		// 모델 컴포넌트 추가
	if (FAILED(__super::Add_Component(m_eCurrentLevelID, m_wModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

// 셰이더에 필요한 정보들을 넘겨주자.
HRESULT CAnything_Object::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//	return E_FAIL;
	return S_OK;
}

_bool CAnything_Object::PickingObject(_float4* pOut)
{
	if (m_pModelCom == nullptr)
		return false;

	// 마우스의 월드 스페이스를 받는다.
	RAY MouseRayInWorldSpace = m_pGameInstance->Get_MouseRay_World(g_iWinSizeX, g_iWinSizeY);

	// 받은 마우스 월드 스페이스와 이 녀석의 월드 스페이스를 넘겨주고, 픽킹이 됐다면 픽킹된 값을 받아온다.
 	if (m_pModelCom->Compute_MousePos_float4(MouseRayInWorldSpace, m_pTransformCom->Get_WorldMatrix(), pOut)) // 값 잘 받았음))
		return true;

	return false;
}

void CAnything_Object::Write_Json(json& Out_Json)
{
	__super::Write_Json(Out_Json);

	// 컴포넌트를 순회하며 트랜스폼 컴포넌트를 찾으면.
	auto iter = Out_Json["Component"].find("Transform");

}

void CAnything_Object::Load_FromJson(const json& In_Json)
{
	__super::Load_FromJson(In_Json);

	_float4x4 WorldMatrix;
	// 선언 및 초기화
	ZeroMemory(&WorldMatrix, sizeof(_float4x4));

	// 값 불러오기
	CJson_Utility::Load_JsonFloat4x4(In_Json["Component"]["Transform"], WorldMatrix);

	// 불러온 월드 값을 넣어주자.
	_float4 fWorld = { WorldMatrix._41, WorldMatrix._42, WorldMatrix._43, 1.f };

	// 이 녀석을 받은 월드 값으로 변경해주자.
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, fWorld);
}

CAnything_Object* CAnything_Object::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel)
{
	CAnything_Object* pInstance = new CAnything_Object(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(eLevel)))
	{
		MSG_BOX("Failed to Created : CAnything_Object");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CAnything_Object::Clone(void* pArg)
{
	CAnything_Object* pInstance = new CAnything_Object(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAnything_Object");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnything_Object::Free()
{
	__super::Free();

	/* 컴포넌트들을 다 사용했으면 지워주자. */
	Safe_Release(m_pModelCom);	// 모델
	Safe_Release(m_pShaderCom);	// 셰이더
}

