#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

/* 
	어떤 물체든지 될 수 있다.
	물체들이 사용해야할 기능들을 여기에 갖게만든다.
	어떤 물체에 뭔가를 사용하고 싶다. -> 그 물체가 들고있는 기능을 사용
	ex : CAnything_Object->원하는기능();
	CAnything_Object클론으로 대량 생산이 가능.
	구분하기 위해 생성할때 리스트에 저장할것이다. (그럼 원하는 녀석으로 원하는 기능 사용가능)
	ex : List = CAnything_Object 생성
	List[선택된녀석]->원하는기능
*/

class CAnything_Object final : public CGameObject
{
	enum TEXTURE { TYPE_DIFFUSE, TYPE_MASK, TYPE_BRUSH, TYPE_END };

private:
	LEVEL				m_eCurrentLevelID = { LEVEL_END };

private:
	CAnything_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAnything_Object(const CAnything_Object& rhs);
	virtual ~CAnything_Object() = default;

public:
	virtual HRESULT Initialize_Prototype(LEVEL eLevel);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	wstring m_wModelTag;
	wstring m_wShaderTag;
	XMFLOAT4 m_vPosition = { 0.f, 0.f, 0.f, 0.f };

	_bool	m_bAnimLoop = false;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	_bool PickingObject(_float4* pOut);

public:
	void Write_Json(json& Out_Json);
	void Load_FromJson(const json& In_Json);

public:
	/* 원형객체를 생성한다. */
	static CAnything_Object* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel);

	/* 사본객체를 생성한다. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};

END