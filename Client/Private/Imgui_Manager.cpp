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

	// 할당
	m_arrProj = new _float[16];
	m_arrView = new _float[16];

	if(!ImGui_ImplWin32_Init(g_hWnd))
		return E_FAIL;

	if(!ImGui_ImplDX11_Init(m_pDevice, m_pContext))
		return E_FAIL;

#pragma region 원본 테그
	/* 애니메이션 */
	m_vecProtoList.push_back(TEXT("Prototype_Component_Fiona"));
	//m_vecProtoList.push_back(TEXT("Prototype_Component_AntWorker"));
	//m_vecNonProtoList.push_back(TEXT("Prototype_Component_Model_Burgle"));
	//m_vecNonProtoList.push_back(TEXT("Prototype_Component_Model_Firefly"));
	//m_vecNonProtoList.push_back(TEXT("Prototype_Component_Model_Ladybug"));

	/* 논애니메이션 */
	m_vecNonProtoList.push_back(TEXT("Prototype_Component_Model_Mannequin"));
	m_vecNonProtoList.push_back(TEXT("Prototype_Component_Model_CardBoardBox"));
	//m_vecProtoList.push_back(TEXT("Prototype_Component_Model_Garden"));
#pragma endregion

#pragma region 레이어
	// 생성할 곳 지정 (테그)
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
	
	ImGui::Begin(u8"메인 툴", &m_bMainTool, ImGuiWindowFlags_AlwaysAutoResize);

	if (ImGui::BeginMenu(u8"툴"))
	{
		ImGui::MenuItem(u8"맵툴", NULL, &m_bMapTool);
		//ImGui::MenuItem(u8"이펙트툴", NULL, &m_bEffectTool);
		//ImGui::MenuItem(u8"오브젝트툴", NULL, &m_bObjectTool);
		//ImGui::MenuItem(u8"카메라툴", NULL, &m_bCameraTool);

		ImGui::EndMenu();
	}

#pragma region 다이얼로그
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
	//반환할 char* 변수 선언
	char* pStr;
	//입력받은 wchar_t 변수의 길이를 구함
	int strSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	//char* 메모리 할당
	pStr = new char[strSize];
	//형 변환
	WideCharToMultiByte(CP_ACP, 0, str, -1, pStr, strSize, 0, 0);
	return pStr;
}//인터벌 정점과의 간격 사이

wchar_t* CImgui_Manager::ConvertCtoWC(const char* str)
{
	//wchar_t형 변수 선언
	wchar_t* pStr;
	//멀티 바이트 크기 계산 길이 반환
	int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);
	//wchar_t 메모리 할당
	pStr = new WCHAR[strSize];
	//형 변환
	MultiByteToWideChar(CP_ACP, 0, str, (int)strlen(str) + 1, pStr, strSize);
	return pStr;
}

void CImgui_Manager::Create_Object()
{
	/* 세팅할 값이 있다면 아래서 생성 하기전에 여기서 세팅 */
	/* 구조체를 만들고 값을 채워 pArg로 인자값을 넘겨주자. */
	/* pArg를 받은 오브젝트가 받은 값으로 알맞게 생성될것이다. */

	ANYTHINGOBJ tObjInfo; // 여기까진 값 잘들어옴

	tObjInfo.cModelTag = m_strModelTag;
	tObjInfo.cShaderTag = m_strShaderTag;
	tObjInfo.vPosition = m_vPosition;

	class CGameObject* pGameObject;

	//								( 레벨,  레이어테그,프로토타입테그,구조체, CGameObject)			
	m_pGameInstance->Add_CloneObject(LEVEL_TOOL, m_strLayerTag, m_strObjectTag, &tObjInfo, &pGameObject);

	m_vecCloneList.push_back(pGameObject); // 오브젝트 생성시 리스트에 추가
}

void CImgui_Manager::ShowMapTool()
{
	ImGui::Begin(u8"맵툴");
	if (ImGui::BeginTabBar("##MapTabBar"))
	{
		//TODO 지형 탭 시작
		if (ImGui::BeginTabItem(u8"지형"))
		{
			ImGui::Text(u8"지형 만들기");

			ImGui::InputFloat(u8"입력 X : ", &fX);
			//ImGui::InputFloat(u8"입력 Y : ", &fY);
			ImGui::InputFloat(u8"입력 Z : ", &fZ);

			m_Info.vPosition.x = fX;	// 가로
			m_Info.vPosition.y = 1.f;	// 높이
			m_Info.vPosition.z = fZ;	// 세로

			if (ImGui::Button(u8"지형 생성"))
			{
				if (m_pDynamic_Terrain != nullptr) // 삭제
				{ 
					m_pDynamic_Terrain->Delete_Component(TEXT("Com_VIBuffer"));
				}
				//								(	레벨	,		레이어테그		,				프로토타입 테그					, 구조체,					터레인							)			
				m_pGameInstance->Add_CloneObject(LEVEL_TOOL, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Dynamic_Terrain"), &m_Info, reinterpret_cast<CGameObject**>(&m_pDynamic_Terrain));
			}
			
			ImGui::EndTabItem();
		}
		//TODO 지형 탭 종료

		//! 드로우 탭 시작
		if (ImGui::BeginTabItem(u8"드로우"))
		{
			ImGui::Text(u8"드로우");

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

		//! 모델 탭 시작
		if (ImGui::BeginTabItem(u8"모델"))
		{
			Update_View_Proj();

			if(m_pPickObject)
			Use_ImGuizmo();
			wstring strTest;
			// 오브젝트를 담고있는 map 컨테이너를 들고와서 테그를 지정하게 만들자.
			m_mapGameObject = m_pGameInstance->Get_GameObject();

			/* 리스트 박스 */

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
				str2.assign(m_strModelTag.begin(), m_strModelTag.end());// 무슨 오류가 남
				if (ImGui::Selectable((str + "." + str2).c_str(), i == m_iCurClone_ObjIndex)) {
					m_iCurClone_ObjIndex = i;
					m_strCurClone_ObjTag = (str + "." + str2);
				}
			}
			ImGui::EndListBox();

			/* 원본 리스트 */
			/* m_vecObjectList 클론 리스트 */

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
						// 위치 (마우스 픽킹 위치) 멤버 변수로 받는다.
						m_pDynamic_Terrain->PickingGround(&m_vPosition);

						// 생성할 곳 지정 (테그)
						m_strLayerTag = TEXT("Layer_Monster");

						m_strObjectTag = TEXT("Prototype_GameObject_Anything");

						Create_Object(); // 멤버 변수로 채워줄거라 인자값을 받을 필요가 없다.
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

#pragma region 다이얼로그
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

					//							  호출할 녀석							보고싶은 확장자들
					ImGuiFileDialog::Instance()->OpenDialog("DialogFileKey", "Choose File", szFilters,
						".", 1, nullptr, ImGuiFileDialogFlags_Modal);
					// 4번째 인자로 패스 경로를 넣으면 시작경로로 지정
				}
				break;
			}
			case Client::CImgui_Manager::DLOAD:
			{
				if (!m_imEmbed_Open.IsOpened())
				{

					const _char* szFilters = "Models (*.fbx, *.mdl){.fbx,.mdl},FBX (*.fbx){.fbx},Binary (*.mdl){.mdl},All files{.*}";

					//							  호출할 녀석							보고싶은 확장자들
					ImGuiFileDialog::Instance()->OpenDialog("DialogFileKey", "Choose File", szFilters,
						".", 1, nullptr, ImGuiFileDialogFlags_Modal);
					// 4번째 인자로 패스 경로를 넣으면 시작경로로 지정
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

			ImGui::SameLine(); // 이전거와 다음거를 같은 줄에 놓겠다.
			ImGui::EndTabItem();
	}
	//! 모델 탭 종료
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
		//TODO 오브젝트1 탭 시작
		if (ImGui::BeginTabItem(u8"NONE1"))
		{
			ImGui::Text(u8"NONE1");
			ImGui::EndTabItem();
		}
		//TODO 오브젝트1 탭 종료

		//! 오브젝트2 탭 시작
		if (ImGui::BeginTabItem(u8"NONE1"))
		{
			ImGui::Text(u8"NONE1");
			ImGui::EndTabItem();
		}
		//! 오브젝트2 탭 종료
		ImGui::EndTabBar();
	}
	ImGui::End();
}

void CImgui_Manager::ShowCameraTool()
{
	ImGui::Begin(u8"NONE2");
	if (ImGui::BeginTabBar("##CameraTabBar"))
	{
		//TODO 베지어 탭 시작
		if (ImGui::BeginTabItem(u8"베지어"))
		{
			ImGui::Text(u8"베지어");
			ImGui::EndTabItem();
		}
		//TODO 베지어 탭 종료

		//! 메뉴얼 탭 시작
		if (ImGui::BeginTabItem(u8"메뉴얼"))
		{
			ImGui::Text(u8"메뉴얼");
			ImGui::EndTabItem();
		}
		//! 메뉴얼 탭 종료
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





	// 피킹된 오브젝트의 월드 반환
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
	_float4x4 _View4x4 = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW); // 뷰 스페이스 반환

	_float arrView[] = { _View4x4._11,_View4x4._12,_View4x4._13,_View4x4._14,
				  _View4x4._21,_View4x4._22,_View4x4._23,_View4x4._24,
				  _View4x4._31,_View4x4._32,_View4x4._33,_View4x4._34,
				  _View4x4._41,_View4x4._42,_View4x4._43,_View4x4._44 };

	memcpy(m_arrView, &arrView, sizeof(arrView));

	_float4x4 _Proj4x4 = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ); // 월드 스페이스 반환

	_float arrProj[] = { _Proj4x4._11,_Proj4x4._12,_Proj4x4._13,_Proj4x4._14,
					  _Proj4x4._21,_Proj4x4._22,_Proj4x4._23,_Proj4x4._24,
					  _Proj4x4._31,_Proj4x4._32,_Proj4x4._33,_Proj4x4._34,
					  _Proj4x4._41,_Proj4x4._42,_Proj4x4._43,_Proj4x4._44 };

	memcpy(m_arrProj, &arrProj, sizeof(arrProj));

}

void CImgui_Manager::LoadGui()
{
	// display								이 녀석을 출력할 호출 키
	if (ImGuiFileDialog::Instance()->Display("DialogFileKey"))
	{
		std::string fileFullPathName;
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			fileFullPathName = ImGuiFileDialog::Instance()->GetFilePathName(); // file, path, name 다 합친거
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

// 저장하기									  저장할 경로
void CImgui_Manager::Write_Json(const string& In_szPath)
{
	json NewJson; // 값을 담을 json타입 변수 선언.

	// 클론 리스트에 저장된 만큼 모두 순회한다.
	for (_uint i = 0; i < m_vecCloneList.size(); ++i)
	{
		m_vecCloneList[i]->Write_Json(NewJson); // 순회하며 자기 자신의 Write_Json을 모두 호출한다.
	}

	// 위에서 모두 호출하여 저장하는 작업이 끝났으면, 값을 담아온 json형식의 변수를 저장시켜주자.
	if (FAILED(CJson_Utility::Save_Json(In_szPath.c_str(), NewJson)))
		DEBUG_ASSERT;
}

// 불러오기
void CImgui_Manager::Load_FromJson(const string& In_szPath)
{

}