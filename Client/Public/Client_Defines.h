#pragma once

namespace Client 
{
	static const unsigned int	g_iWinSizeX = 1280;	//! 윈도우 창 가로 사이즈 전역
	static const unsigned int	g_iWinSizeY = 720;	//! 윈도우 창 세로 사이즈 전역

	enum LEVEL { LEVEL_STATIC, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_LOADING, LEVEL_TOOL, LEVEL_END }; //! 레벨 열거체
}

using namespace Client;


extern HWND g_hWnd;	//! 전역 윈도우 핸들 선언을 위한 extern
extern HINSTANCE g_hInst; //! 전역 인스턴스 선언을 위한 extern

//! HINSTANCE 핸들은 메모리 상에 올라가 있는 시작 주소 값을 갖고 있다. 
//! 보통 리소스를 로드하는 함수에서 이 핸들 값을 많이 참조하게 된다. 이유는
//! 메모리 상에 올라가 있는 실행 모듈(EXE, DLL 등등) HINSTANCE 가 가리키는 주소에 올라가 있는 실행모듈에서 그 리소스를 읽어 오라고 지정 해주는 것.
//! 이렇게 여러 함수에서 HINSTANCE 핸들을 필요로 하기때문에, 전역 변수에 넣어두는 것이다.

//! HINSTANCE는 실행파일 형태로 껍데기에 불과한 프로그램이 메모리에 실제로 구현된 실체를 뜻한다.
//! 만약, 여러 프로그램이 실행 중일 때, 이들의 각각을 프로그램 인스턴스라고 하고, 
//! 실행되는 프로그램마다 고유한 값을 갖고 실행중인 프로그램들을 구분하기 위한 식별값으로 인스턴스 핸들을 의미.
 

//TODO HWND는 어떠한 한 프로그램은 여러 개의 윈도우(창)들을 가질 수 있는데, 이들의 각각을 구분하기위한 식별 값을 말한다.
//TODO 프로그램의 윈도우 핸들 중, 대표적인 것이 부모 윈도우의 핸들이다. 따라서 대부분의 윈도우 프로그래밍의 작업과정 중 HWND 값을 통해 얻은 윈도우 핸들을 이용한다.
