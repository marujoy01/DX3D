#include "../Default/stdafx.h"
#include <sstream>

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "ImGuizmo/ImGuizmo.h"
#include "ImGuiFileDialog.h"

#include "Imgui_Manager.h"
#include "GameInstance.h"
#include "Dynamic_Terrain.h"
#include "GameObject.h"
#include "Object_Manager.h"
#include "Anything_Object.h"


IMPLEMENT_SINGLETON(CImgui_Manager)

CImgui_Manager::CImgui_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

CImgui_Manager::~CImgui_Manager()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
	Safe_Delete_Array(m_arrProj);
	Safe_Delete_Array(m_arrView);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

HRESULT CImgui_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	m_pDevice->AddRef();
	m_pContext->AddRef();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	//io.ConfigViewportsNoAutoMerge = true;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	// �Ҵ�
	m_arrProj = new _float[16];
	m_arrView = new _float[16];

	if(!ImGui_ImplWin32_Init(g_hWnd))
		return E_FAIL;

	if(!ImGui_ImplDX11_Init(m_pDevice, m_pContext))
		return E_FAIL;

#pragma region ���� �ױ�
	/* �ִϸ��̼� */
	m_vecProtoList.push_back(TEXT("Prototype_Component_Fiona"));
	//m_vecProtoList.push_back(TEXT("Prototype_Component_AntWorker"));
	//m_vecNonProtoList.push_back(TEXT("Prototype_Component_Model_Burgle"));
	//m_vecNonProtoList.push_back(TEXT("Prototype_Component_Model_Firefly"));
	//m_vecNonProtoList.push_back(TEXT("Prototype_Component_Model_Ladybug"));

	/* ��ִϸ��̼� */
	m_vecNonProtoList.push_back(TEXT("Prototype_Component_Model_Mannequin"));
	m_vecNonProtoList.push_back(TEXT("Prototype_Component_Model_CardBoardBox"));
	//m_vecProtoList.push_back(TEXT("Prototype_Component_Model_Garden"));
#pragma endregion

#pragma region ���̾�
	// ������ �� ���� (�ױ�)
	m_strLayerTag = TEXT("Layer_Monster");
#pragma endregion

	ImGui::StyleColorsDark();
	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesKorean());

	return S_OK;
}

void CImgui_Manager::Tick()
{
	
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuizmo::BeginFrame();


	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoBackground;

	auto& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	const ImVec4 bgColor = ImVec4(0.1, 0.1, 0.1, 0.5);
	colors[ImGuiCol_WindowBg] = bgColor;
	colors[ImGuiCol_ChildBg] = bgColor;
	colors[ImGuiCol_TitleBg] = bgColor;
	
	ImGui::Begin(u8"���� ��", &m_bMainTool, ImGuiWindowFlags_AlwaysAutoResize);

	if (ImGui::BeginMenu(u8"��"))
	{
		ImGui::MenuItem(u8"����", NULL, &m_bMapTool);
		//ImGui::MenuItem(u8"����Ʈ��", NULL, &m_bEffectTool);
		//ImGui::MenuItem(u8"������Ʈ��", NULL, &m_bObjectTool);
		//ImGui::MenuItem(u8"ī�޶���", NULL, &m_bCameraTool);

		ImGui::EndMenu();
	}

#pragma region ���̾�α�
	LoadGui();
#pragma endregion

	if (m_bMapTool)	ShowMapTool();
	//if (m_bEffectTool) ShowEffectTool();
	//if (m_bCameraTool) ShowCameraTool();
	//if (m_bObjectTool) ShowObjectTool();

	ImGui::End();
}

void CImgui_Manager::Render()
{
	ImGui::EndFrame();
	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

char* CImgui_Manager::ConvertWCtoC(const wchar_t* str)
{
	//��ȯ�� char* ���� ����
	char* pStr;
	//�Է¹��� wchar_t ������ ���̸� ����
	int strSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	//char* �޸� �Ҵ�
	pStr = new char[strSize];
	//�� ��ȯ
	WideCharToMultiByte(CP_ACP, 0, str, -1, pStr, strSize, 0, 0);
	return pStr;
}//���͹� �������� ���� ����

wchar_t* CImgui_Manager::ConvertCtoWC(const char* str)
{
	//wchar_t�� ���� ����
	wchar_t* pStr;
	//��Ƽ ����Ʈ ũ�� ��� ���� ��ȯ
	int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);
	//wchar_t �޸� �Ҵ�
	pStr = new WCHAR[strSize];
	//�� ��ȯ
	MultiByteToWideChar(CP_ACP, 0, str, (int)strlen(str) + 1, pStr, strSize);
	return pStr;
}

void CImgui_Manager::Create_Object()
{
	/* ������ ���� �ִٸ� �Ʒ��� ���� �ϱ����� ���⼭ ���� */
	/* ����ü�� ����� ���� ä�� pArg�� ���ڰ��� �Ѱ�����. */
	/* pArg�� ���� ������Ʈ�� ���� ������ �˸°� �����ɰ��̴�. */

	ANYTHINGOBJ tObjInfo; // ������� �� �ߵ���

	tObjInfo.cModelTag = m_strModelTag;
	tObjInfo.cShaderTag = m_strShaderTag;
	tObjInfo.vPosition = m_vPosition;

	class CGameObject* pGameObject;

	//								( ����,  ���̾��ױ�,������Ÿ���ױ�,����ü, CGameObject)			
	m_pGameInstance->Add_CloneObject(LEVEL_TOOL, m_strLayerTag, m_strObjectTag, &tObjInfo, &pGameObject);

	m_vecCloneList.push_back(pGameObject); // ������Ʈ ������ ����Ʈ�� �߰�
}

void CImgui_Manager::ShowMapTool()
{
	ImGui::Begin(u8"����");
	if (ImGui::BeginTabBar("##MapTabBar"))
	{
		//TODO ���� �� ����
		if (ImGui::BeginTabItem(u8"����"))
		{
			ImGui::Text(u8"���� �����");

			ImGui::InputFloat(u8"�Է� X : ", &fX);
			//ImGui::InputFloat(u8"�Է� Y : ", &fY);
			ImGui::InputFloat(u8"�Է� Z : ", &fZ);

			m_Info.vPosition.x = fX;	// ����
			m_Info.vPosition.y = 1.f;	// ����
			m_Info.vPosition.z = fZ;	// ����

			if (ImGui::Button(u8"���� ����"))
			{
				if (m_pDynamic_Terrain != nullptr) // ����
				{ 
					m_pDynamic_Terrain->Delete_Component(TEXT("Com_VIBuffer"));
				}
				//								(	����	,		���̾��ױ�		,				������Ÿ�� �ױ�					, ����ü,					�ͷ���							)			
				m_pGameInstance->Add_CloneObject(LEVEL_TOOL, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Dynamic_Terrain"), &m_Info, reinterpret_cast<CGameObject**>(&m_pDynamic_Terrain));
			}
			
			ImGui::EndTabItem();
		}
		//TODO ���� �� ����

		//! ��ο� �� ����
		if (ImGui::BeginTabItem(u8"��ο�"))
		{
			ImGui::Text(u8"��ο�");

			if (m_pGameInstance->Key_Pressing(DIK_1))
			{
				m_iHeightRadio = (_int)EDIT_MODE::HEIGHT_FLAT;
			}

			if (m_pGameInstance->Key_Pressing(DIK_2))
			{
				m_iHeightRadio = (_int)EDIT_MODE::HEIGHT_LERP;
			}

			if (m_pGameInstance->Key_Pressing(DIK_3))
			{
				m_iHeightRadio = (_int)EDIT_MODE::HEIGHT_SET;
			}

			if (m_pGameInstance->Key_Pressing(DIK_4))
			{
				m_iHeightRadio = (_int)EDIT_MODE::FILLTER;
			}

			if (m_pGameInstance->Key_Pressing(DIK_5))
			{
				m_iHeightRadio = (_int)EDIT_MODE::NON;
			}

			ImGui::RadioButton("HEIGHT_FLAT", &m_iHeightRadio, 0);
			ImGui::RadioButton("HEIGHT_LERP", &m_iHeightRadio, 1);
			ImGui::RadioButton("HEIGHT_SET", &m_iHeightRadio, 2);
			ImGui::RadioButton("FILLTER", &m_iHeightRadio, 3);
			ImGui::RadioButton("NONE", &m_iHeightRadio, 4);

			if (m_pGameInstance->Mouse_Pressing(CInput_Device::DIM_LB))
			{
				switch ((EDIT_MODE)m_iHeightRadio)
				{
					case EDIT_MODE::HEIGHT_FLAT:
					case EDIT_MODE::HEIGHT_LERP:
					case EDIT_MODE::HEIGHT_SET:
					case EDIT_MODE::FILLTER:
					{
						m_pDynamic_Terrain->PickingGround((EDIT_MODE)m_iHeightRadio);
					}
					case EDIT_MODE::NON:
					break;
				}
			}
			ImGui::EndTabItem();
		}

		//! �� �� ����
		if (ImGui::BeginTabItem(u8"��"))
		{
			Update_View_Proj();

			if(m_pPickObject)
			Use_ImGuizmo();
			wstring strTest;
			// ������Ʈ�� ����ִ� map �����̳ʸ� ���ͼ� �ױ׸� �����ϰ� ������.
			m_mapGameObject = m_pGameInstance->Get_GameObject();

			/* ����Ʈ �ڽ� */

			ImGui::RadioButton("Anim Model", &m_iTypeRadio, 0);
			ImGui::RadioButton("NonAnim Model", &m_iTypeRadio, 1);

			switch ((MODEL_TYPE)m_iTypeRadio)
			{
				case ANIM:
				{
					m_strShaderTag = TEXT("Prototype_Component_Shader_AnimModel");
					ImGui::Text("Anim_List");
					ImVec2 vSize = ImVec2(250, 100);
					ImGui::BeginListBox("Model", vSize);
					for (auto& iter : m_vecProtoList) 
					{

						string str;

						str.assign(iter.begin(), iter.end());

						//

						if (ImGui::Selectable(str.c_str(), iter == m_strModelTag))
							m_strModelTag = iter;
					}
					ImGui::EndListBox();
					break;
				}
				case NONANIM:
				{
					m_strShaderTag = TEXT("Prototype_Component_Shader_Model");
					ImGui::Text("NonAnim_List");
					ImVec2 vSize = ImVec2(250, 100);
					ImGui::BeginListBox("Model", vSize);
					for (auto& iter : m_vecNonProtoList) 
					{
						string str;
						str.assign(iter.begin(), iter.end());

						//

						if (ImGui::Selectable(str.c_str(), iter == m_strModelTag))
							m_strModelTag = iter;
					}
					ImGui::EndListBox();
					break;
				}
				case END_MTYPE:
				default:
					break;
			}

			ImVec2 vSize = ImVec2(250, 100);
			ImGui::BeginListBox("Clone", vSize);
			for (_uint i = 0; i < m_vecCloneList.size(); i++)
			{
				string str = to_string(i);
				string str2;
				str2.assign(m_strModelTag.begin(), m_strModelTag.end());// ���� ������ ��
				if (ImGui::Selectable((str + "." + str2).c_str(), i == m_iCurClone_ObjIndex)) {
					m_iCurClone_ObjIndex = i;
					m_strCurClone_ObjTag = (str + "." + str2);
				}
			}
			ImGui::EndListBox();

			/* ���� ����Ʈ */
			/* m_vecObjectList Ŭ�� ����Ʈ */

			if (m_pGameInstance->Key_Pressing(DIK_1))
			{
				m_iModelRadio = (_int)EDIT_MODEL::NONE_MODEL;
			}

			if (m_pGameInstance->Key_Pressing(DIK_2))
			{
				m_iModelRadio = (_int)EDIT_MODEL::CREATE_MODEL;
			}

			if (m_pGameInstance->Key_Pressing(DIK_3))
			{
				m_iModelRadio = (_int)EDIT_MODEL::PICKING_MODEL;
			}

			ImGui::Text("Select Mode");
			ImGui::RadioButton("NONE", &m_iModelRadio, 0);
			ImGui::RadioButton("Model Create", &m_iModelRadio, 1);
			ImGui::RadioButton("Model Picking", &m_iModelRadio, 2);

			switch ((EDIT_MODEL)m_iModelRadio)
			{
				case NONE_MODEL:
					break;
				case CREATE_MODEL:
				{
					if (m_pGameInstance->Mouse_Pressing(CInput_Device::DIM_LB))
					{
						// ��ġ (���콺 ��ŷ ��ġ) ��� ������ �޴´�.
						m_pDynamic_Terrain->PickingGround(&m_vPosition);

						// ������ �� ���� (�ױ�)
						m_strLayerTag = TEXT("Layer_Monster");

						m_strObjectTag = TEXT("Prototype_GameObject_Anything");

						Create_Object(); // ��� ������ ä���ٰŶ� ���ڰ��� ���� �ʿ䰡 ����.
					}
					break;
				}
				case PICKING_MODEL:
				{
					if (m_pGameInstance->Mouse_Pressing(CInput_Device::DIM_LB))
					{
						for (_int i = 0; i < m_vecCloneList.size(); i++)
						{
							if (dynamic_cast<CAnything_Object*>(m_vecCloneList[i])->PickingObject(&m_vModelPos))
							{
								m_iSelectModel = i;
								m_pPickObject = (m_vecCloneList[i]);
								break;
							}

						}
					}
					break;
				}
				default:
					break;
			}

			ImGui::Text("Select Model Index : %d", m_iSelectModel);
			ImGui::Text("Select Model X : %f", m_vModelPos.x);
			ImGui::Text("Select Model Y : %f", m_vModelPos.y);
			ImGui::Text("Select Model Z : %f", m_vModelPos.z);
			ImGui::Text("Select Model W : %f", m_vModelPos.w);

#pragma region ���̾�α�
			if (ImGui::Button("Save"))
				m_eDialogMod = DSAVE;

			if (ImGui::Button("Load"))
				m_eDialogMod = DLOAD;

			switch (m_eDialogMod)
			{
			case Client::CImgui_Manager::DSAVE:
			{
				if (!m_imEmbed_Open.IsOpened())
				{

					const _char* szFilters = "Models (*.fbx, *.mdl){.fbx,.mdl},FBX (*.fbx){.fbx},Binary (*.mdl){.mdl},All files{.*}";

					//							  ȣ���� �༮							������� Ȯ���ڵ�
					ImGuiFileDialog::Instance()->OpenDialog("DialogFileKey", "Choose File", szFilters,
						".", 1, nullptr, ImGuiFileDialogFlags_Modal);
					// 4��° ���ڷ� �н� ��θ� ������ ���۰�η� ����
				}
				break;
			}
			case Client::CImgui_Manager::DLOAD:
			{
				if (!m_imEmbed_Open.IsOpened())
				{

					const _char* szFilters = "Models (*.fbx, *.mdl){.fbx,.mdl},FBX (*.fbx){.fbx},Binary (*.mdl){.mdl},All files{.*}";

					//							  ȣ���� �༮							������� Ȯ���ڵ�
					ImGuiFileDialog::Instance()->OpenDialog("DialogFileKey", "Choose File", szFilters,
						".", 1, nullptr, ImGuiFileDialogFlags_Modal);
					// 4��° ���ڷ� �н� ��θ� ������ ���۰�η� ����
				}
				break;
			}
			case Client::CImgui_Manager::DEND:
				break;
			default:
				break;
			}

		}
#pragma endregion

			ImGui::SameLine(); // �����ſ� �����Ÿ� ���� �ٿ� ���ڴ�.
			ImGui::EndTabItem();
	}
	//! �� �� ����
	ImGui::EndTabBar();

	ImGui::End();
}


void CImgui_Manager::ShowEffectTool()
{
	ImGui::Begin("NONE");
	if (ImGui::BeginTabBar("NONE"))
	{

	}
	ImGui::End();
}

void CImgui_Manager::ShowObjectTool()
{
	ImGui::Begin("NONE1");
	if (ImGui::BeginTabBar("##NONE1"))
	{
		//TODO ������Ʈ1 �� ����
		if (ImGui::BeginTabItem(u8"NONE1"))
		{
			ImGui::Text(u8"NONE1");
			ImGui::EndTabItem();
		}
		//TODO ������Ʈ1 �� ����

		//! ������Ʈ2 �� ����
		if (ImGui::BeginTabItem(u8"NONE1"))
		{
			ImGui::Text(u8"NONE1");
			ImGui::EndTabItem();
		}
		//! ������Ʈ2 �� ����
		ImGui::EndTabBar();
	}
	ImGui::End();
}

void CImgui_Manager::ShowCameraTool()
{
	ImGui::Begin(u8"NONE2");
	if (ImGui::BeginTabBar("##CameraTabBar"))
	{
		//TODO ������ �� ����
		if (ImGui::BeginTabItem(u8"������"))
		{
			ImGui::Text(u8"������");
			ImGui::EndTabItem();
		}
		//TODO ������ �� ����

		//! �޴��� �� ����
		if (ImGui::BeginTabItem(u8"�޴���"))
		{
			ImGui::Text(u8"�޴���");
			ImGui::EndTabItem();
		}
		//! �޴��� �� ����
		ImGui::EndTabBar();
	}
	ImGui::End();
}

void CImgui_Manager::HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::TextUnformatted(desc);
		ImGui::EndTooltip();
	}
}

char* CImgui_Manager::ConverWStringtoC(const wstring& wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);

	char* result = new char[size_needed];

	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, result, size_needed, NULL, NULL);

	return result;
}


void CImgui_Manager::Use_ImGuizmo()
{
	/*==== GridMode ====*/
	//ImGui::Checkbox("Render_Grid_Mode", &m_bGridMode);

	/*==== Set ImGuizmo ====*/
	ImGuizmo::SetOrthographic(false);
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

	if (ImGui::IsKeyPressed(ImGuiKey_T))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(ImGuiKey_R))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(ImGuiKey_E))
		mCurrentGizmoOperation = ImGuizmo::SCALE;

	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;





	// ��ŷ�� ������Ʈ�� ���� ��ȯ
	_float4x4 matWorld = m_pPickObject->Get_TransformCom()->Get_WorldFloat4x4();

	_float arrWorld[] = { matWorld._11,matWorld._12,matWorld._13,matWorld._14,
						  matWorld._21,matWorld._22,matWorld._23,matWorld._24,
						  matWorld._31,matWorld._32,matWorld._33,matWorld._34,
						  matWorld._41,matWorld._42,matWorld._43,matWorld._44 };

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(arrWorld, matrixTranslation, matrixRotation, matrixScale);
	ImGui::DragFloat3("Tr", matrixTranslation);
	ImGui::DragFloat3("Rt", matrixRotation);
	ImGui::DragFloat3("Sc", matrixScale);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, arrWorld);

	
	ImGui::Checkbox("UseSnap", &useSnap);

	ImGui::SameLine();

	

	switch (mCurrentGizmoOperation)
	{
	case ImGuizmo::TRANSLATE:
		ImGui::DragFloat3("Snap", &snap[0]);
		break;
	case ImGuizmo::ROTATE:
		ImGui::DragFloat3("Angle Snap", &snap[0]);
		break;
	case ImGuizmo::SCALE:
		ImGui::DragFloat3("Scale Snap", &snap[0]);
		break;
	}

	//if (m_bGridMode)
	//	CImgui_Manager::GetInstance()->DrawGrid();

	ImGuizmo::Manipulate(m_arrView, m_arrProj, mCurrentGizmoOperation, mCurrentGizmoMode, arrWorld, NULL, useSnap ? &snap[0] : NULL);

	_float4x4 matW = { arrWorld[0],arrWorld[1],arrWorld[2],arrWorld[3],
					arrWorld[4],arrWorld[5],arrWorld[6],arrWorld[7],
					arrWorld[8],arrWorld[9],arrWorld[10],arrWorld[11],
					arrWorld[12],arrWorld[13],arrWorld[14],arrWorld[15] };

	m_pPickObject->Get_TransformCom()->Set_WorldFloat4x4(matW);


	if (ImGuizmo::IsOver())
	{
		int a = 0;
	}
}

void CImgui_Manager::Update_View_Proj()
{
	_float4x4 _View4x4 = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW); // �� �����̽� ��ȯ

	_float arrView[] = { _View4x4._11,_View4x4._12,_View4x4._13,_View4x4._14,
				  _View4x4._21,_View4x4._22,_View4x4._23,_View4x4._24,
				  _View4x4._31,_View4x4._32,_View4x4._33,_View4x4._34,
				  _View4x4._41,_View4x4._42,_View4x4._43,_View4x4._44 };

	memcpy(m_arrView, &arrView, sizeof(arrView));

	_float4x4 _Proj4x4 = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ); // ���� �����̽� ��ȯ

	_float arrProj[] = { _Proj4x4._11,_Proj4x4._12,_Proj4x4._13,_Proj4x4._14,
					  _Proj4x4._21,_Proj4x4._22,_Proj4x4._23,_Proj4x4._24,
					  _Proj4x4._31,_Proj4x4._32,_Proj4x4._33,_Proj4x4._34,
					  _Proj4x4._41,_Proj4x4._42,_Proj4x4._43,_Proj4x4._44 };

	memcpy(m_arrProj, &arrProj, sizeof(arrProj));

}

void CImgui_Manager::LoadGui()
{
	// display								�� �༮�� ����� ȣ�� Ű
	if (ImGuiFileDialog::Instance()->Display("DialogFileKey"))
	{
		std::string fileFullPathName;
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			fileFullPathName = ImGuiFileDialog::Instance()->GetFilePathName(); // file, path, name �� ��ģ��
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();	// file, path
			std::string filter = ImGuiFileDialog::Instance()->GetCurrentFilter();	// filter
			// here convert from string because a string was passed as a userDatas, but it can be what you want
			std::string userDatas;

			ImGui::Text(fileFullPathName.c_str());
			ImGui::Text(filePath.c_str());
			ImGui::Text(filter.c_str());
			ImGui::Text(userDatas.c_str());

			/*MultiByteToWideChar()*/

			if (ImGuiFileDialog::Instance()->GetUserDatas())
				userDatas = std::string((const char*)ImGuiFileDialog::Instance()->GetUserDatas());
			auto selection = ImGuiFileDialog::Instance()->GetSelection(); // multiselection

			switch (m_eDialogMod)
			{
			case Client::CImgui_Manager::DSAVE:
				Write_Json(fileFullPathName);
				break;
			case Client::CImgui_Manager::DLOAD:
				//Load_FromJson();
				break;
			case Client::CImgui_Manager::DEND:
				break;
			default:
				break;
			}
			
		}
		// close
		ImGuiFileDialog::Instance()->Close();
	}
}

// �����ϱ�									  ������ ���
void CImgui_Manager::Write_Json(const string& In_szPath)
{
	json NewJson; // ���� ���� jsonŸ�� ���� ����.

	// Ŭ�� ����Ʈ�� ����� ��ŭ ��� ��ȸ�Ѵ�.
	for (_uint i = 0; i < m_vecCloneList.size(); ++i)
	{
		m_vecCloneList[i]->Write_Json(NewJson); // ��ȸ�ϸ� �ڱ� �ڽ��� Write_Json�� ��� ȣ���Ѵ�.
	}

	// ������ ��� ȣ���Ͽ� �����ϴ� �۾��� ��������, ���� ��ƿ� json������ ������ �����������.
	if (FAILED(CJson_Utility::Save_Json(In_szPath.c_str(), NewJson)))
		DEBUG_ASSERT;
}

// �ҷ�����
void CImgui_Manager::Load_FromJson(const string& In_szPath)
{

}