#pragma once

#pragma warning(disable : 4251)

#include <d3d11.h>
#include <DirectXMath.h>
#include "Effects11/d3dx11effect.h"
#include "DirectXTK/DDSTextureLoader.h" //TODO DDS 텍스처 로드전용
#include "DirectXTK/WICTextureLoader.h" //TODO 윈도우 텍스처 로드전용 ( bmp, jpg, png 등등 )

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

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



