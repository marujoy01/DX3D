#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

/* 
	� ��ü���� �� �� �ִ�.
	��ü���� ����ؾ��� ��ɵ��� ���⿡ ���Ը����.
	� ��ü�� ������ ����ϰ� �ʹ�. -> �� ��ü�� ����ִ� ����� ���
	ex : CAnything_Object->���ϴ±��();
	CAnything_ObjectŬ������ �뷮 ������ ����.
	�����ϱ� ���� �����Ҷ� ����Ʈ�� �����Ұ��̴�. (�׷� ���ϴ� �༮���� ���ϴ� ��� ��밡��)
	ex : List = CAnything_Object ����
	List[���õȳ༮]->���ϴ±��
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
	/* ������ü�� �����Ѵ�. */
	static CAnything_Object* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eLevel);

	/* �纻��ü�� �����Ѵ�. */
	virtual CGameObject* Clone(void* pArg) override;

	virtual void Free() override;
};

END