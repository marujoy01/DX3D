#pragma once
#include "Base.h"

BEGIN(Engine)

class CGraphic_Device final : public CBase
{
private:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;

public:
	HRESULT Initialize(const GRAPHIC_DESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext); //! 그래픽 디바이스 초기화
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);//! 백버퍼를 지우기 위한 함수 _float4는 새롭게 typedef 한 자료형으로 XMFLOAT4 자료형을 의미한다.
	HRESULT Clear_DepthStencil_View();//! 깊이버퍼와 스텐실 버퍼를 지우기 위한 함수
	HRESULT Resize(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	HRESULT UseFullScreen(_bool bMode);
	HRESULT Present(); //! 후면 버퍼를 전면버퍼로 교체하는 함수( 미리 그려놓은 백 버퍼를 화면에 직접 보여준다. ) 
	

private:
	ID3D11Device*			m_pDevice = { nullptr }; //! 메모리 할당 ( 정점버퍼, 인덱스버퍼, 텍스쳐로드 ) 컴객체의 생성과 관련된 역할
	ID3D11DeviceContext*	m_pContext = { nullptr }; //! 기능실행 ( 바인딩 작업, 정점버퍼를 SetStreamSource(), SetIndices(), SetTransform(), SetTexture()
	
	IDXGISwapChain*			m_pSwapChain = { nullptr }; //! 후면버퍼와 전면버퍼를 교체해가면서 화면에 보여주는 역할

	ID3D11RenderTargetView*		m_pBackBufferRTV = { nullptr }; //! 렌더타겟(그려질 대상)용으로 사용될 수 있는  텍스쳐타입..
	ID3D11DepthStencilView*		m_pDepthStencilView = { nullptr }; //! 깊이스텐실버퍼(임의로 변경할 픽셀값을 저장할?)로서 사용될 수 있는 타입.

private:
	HRESULT Ready_SwapChain(HWND hWnd, GRAPHIC_DESC::WINMODE eWinMode, _uint iWinCX, _uint iWinCY);
	HRESULT Ready_BackBufferRenderTargetView();
	HRESULT Ready_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY);


public:
	static CGraphic_Device* Create(const GRAPHIC_DESC& GraphicDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	virtual void Free() override;
};

END


//TODO IDirect3DTexture9 -> { ID3D11Texture2D, ID3D11ShaderResourceView, ID3D11RenderTargetView, ID3D11DepthStencilView }
//!ID3D11Texture2D = 텍스쳐를 표현하는 사전객체 타입이다. 사전객체를 만드는 이유 -> 용도에 맞는 실질적으로 사용하기위한 텍스쳐객체를 만들어내기위해.
//!ID3D11ShaderResourceView = 셰이더에 전달될 수 있는 텍스쳐타입.
//!ID3D11RenderTargetView = 렌더타겟용으로 사용될 수 있는  텍스쳐타입.
//!ID3D11DepthStencilView = 깊이스텐실버퍼로서 사용될 수 있는 타입.


//TODO 스왑체인에게 필수적으로 필요한 데이터는 백버퍼가 필요하여. 하여 백버퍼를 생성하기위한 정보를 던져준다.
//! 스왑체인을 만들었다 == 백버퍼(텍스쳐)가 생성된다.
//! 기존 Direct9 프레임워크에서의 Begin_Scene, End_Scene이 사라지고, 또한 한번에 클리어 하지않고, 클리어하는 부분을 나눠서 하고있다.