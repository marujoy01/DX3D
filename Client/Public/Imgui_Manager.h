#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
//class CVIBuffer_Terrain;
class CVIBuffer_Dynamic_Terrain;
class CTexture;
END

BEGIN(Client)
class CImgui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImgui_Manager);

public:
	enum EDIT_MODEL { NONE_MODEL, CREATE_MODEL, PICKING_MODEL, END_MODEL };
	enum MODEL_TYPE { ANIM, NONANIM, END_MTYPE };

	typedef struct tag_TextureInfo
	{
		weak_ptr<CTexture>       pDiffTex;
		weak_ptr<CTexture>       pNormTex;
		weak_ptr<CTexture>       pORMTex;
		_float                   fDensity = 30.f;
	} TEXTURES_INFO;

private:
	CImgui_Manager();
	virtual ~CImgui_Manager();

public:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void	Tick();
	void	Render();

	_bool   Get_Ready() { return m_bReady;}
	void    Set_Ready(_bool bReady) { m_bReady = bReady; }

	LEVEL   Get_Level_ID() { return m_eLevelID; }
	void	Set_Level_ID(LEVEL eLevel) { m_eLevelID = eLevel; }

	void	EditTransform();
	void	Use_ImGuizmo();
	void	Update_View_Proj();

	void	LoadGui();
	void	Write_Json(const string& In_szPath);
	void	Load_FromJson(const string& In_szPath);

private:
	char*	ConvertWCtoC(const wchar_t* str);
	wchar_t* ConvertCtoWC(const char* str);

private:
	void	Create_Object();

private:
	_bool						m_bReady = false;
	_bool						m_bTerrain = false;

	_bool						m_bMainTool = { true };
	_bool						m_bMapTool, m_bEffectTool, m_bObjectTool, m_bCameraTool = { false };
	
	_bool						m_bMdelCreate = { false };
	_bool						m_bMdelPicking = { false };
	
private:
	LEVEL						m_eLevelID = { LEVEL_END };
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	CVIBuffer_Dynamic_Terrain*	m_pVIBufferCom = { nullptr };

	// 게임인스턴스
	class CGameInstance*	m_pGameInstance = { nullptr };

	// 다이나믹 터레인	
	class CDynamic_Terrain* m_pDynamic_Terrain = { nullptr };

	// 브러쉬 모드
	EDIT_MODE				m_eEditMode;

private:
	// 터레인 구조체
	VTXDYNAMIC m_Info;

private:
#pragma region 모델
	vector<CGameObject*>	m_vecCloneList; // 사본 생성 객체들 저장
	vector<wstring>			m_vecProtoList;		// 애니메이션o 원본
	vector<wstring>			m_vecNonProtoList;	// 애니메이션x 원본

	map<const wstring, class CGameObject*>* m_mapGameObject = { nullptr };

	class CGameObject* m_pPickObject = nullptr;

	_int					m_iCurClone_ObjIndex = 0;
	_int					m_iCurPrototype_ObjIndex = 0;
	string					m_strCurClone_ObjTag; // error 실수로 초기 값을 줬음, 생성시 터짐

	wstring					m_strLayerTag;
	wstring					m_strObjectTag;
	wstring					m_strModelTag;
	wstring					m_strShaderTag;

	_float4					m_vPosition = { 0.f, 0.f, 0.f, 0.f };
	_float4					m_vModelPos = { 0.f, 0.f, 0.f, 0.f };

	_int					m_iHeightRadio = (_int)EDIT_MODE::NON;
	_int					m_iModelRadio = (_int)NONE_MODEL;
	_int					m_iTypeRadio = (_int)ANIM;
	_int					m_iSelectModel = -1;

	_float* m_arrView = nullptr;
	_float* m_arrProj = nullptr;

#pragma endregion

private:
#pragma region Gizmo
	ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	_bool m_bGridMode = false;
	_bool useSnap = false;
	_float snap[3];
#pragma endregion

private:
#pragma region FileDialog
	enum DLOG_MOD { DSAVE, DLOAD, DEND };
	ImGuiFileDialog						m_imEmbed_Open;
	ImGuiFileDialog						m_imEmbed_Export;
	DLOG_MOD							m_eDialogMod = DEND;

#pragma endregion

	json								m_CurrentLevelJson;
#pragma region Save_Load
	typedef		map<string, TEXTURES_INFO>  TEXTURES;
	string                              m_szMeshName = "";
	TEXTURES                            m_pTextureCom;
#pragma endregion

private:
	float fX = 0.f;
	float fY = 0.f;
	float fZ = 0.f;

private:
	void	HelpMarker(const char* desc);
	char*	ConverWStringtoC(const wstring& wstr);

private:
	void	ShowMapTool();
	void	ShowEffectTool();
	void	ShowObjectTool();
	void	ShowCameraTool();

};
END
