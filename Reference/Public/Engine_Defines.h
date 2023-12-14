#pragma once

#pragma warning(disable : 4251)

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "Effects11/d3dx11effect.h"
#include "DirectXTK/DDSTextureLoader.h" //TODO DDS 텍스처 로드전용
#include "DirectXTK/WICTextureLoader.h" //TODO 윈도우 텍스처 로드전용 ( bmp, jpg, png 등등 )

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#pragma region 어심프(모델)
#include "Assimp\scene.h"
#include "assimp\postprocess.h"
#include "assimp\Importer.hpp"
#pragma endregion

#pragma region RapidJson
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/pointer.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/reader.h"
#include "rapidjson/filereadstream.h"
#include <locale>
#include <codecvt>
using namespace rapidjson;

#include "commdlg.h"
#include "shlwapi.h"
#pragma endregion

#include <d3dcompiler.h> //TODO 셰이더 컴파일용

using namespace DirectX; //! XMFLOAT등등의 자료형을 사용하기위함.

#define  RAPIDJSON_HAS_STDSTRING 1
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
using namespace rapidjson;

#include "BrainTree/BrainTree.h"

namespace BT = BrainTree;

using namespace BT;

#include <unordered_map>
#include <algorithm>
#include <vector>
#include <list>
#include <map>
#include <typeinfo>
using namespace std;

#pragma region 전역 변수

	const XMMATRIX		g_mUnit = XMMatrixIdentity(); // 단위 행렬 여부(UnitMatrix)

#pragma endregion
#include <iostream>
#include <fstream>
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"

#include "Engine_Typedef.h"

namespace Engine
{
	static const wchar_t*	g_pTransformTag = TEXT("Com_Transform");
}

using namespace Engine;

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include "dxgidebug.h"

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif

#ifdef _DEBUG
#define DEBUG_ASSERT assert(false)
#endif

#ifndef _DEBUG
#define DEBUG_ASSERT void(0);
#endif // _DEBUG
