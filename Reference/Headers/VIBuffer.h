#pragma once

#include "Component.h"

//TODO VIBuffer를 다시 부모클래스로 만드는 이유
//! 앞으로 특정 형태 클래스들을 구성할 부모클래스로 사용할건데
//! 특정 형태의 예를 들자면 Rect, Cube, Terrain, Model 등이 있다
//! VI <-- Vertex 와 Index의 약자를 둔건데, 모든 특정 형태 클래스들은 당연히 정점이 필요하다.
//! 어처피 정점버퍼와 인덱스버퍼를 만들거라면 부모클래스에서 공통적으로 수행할 수 있으니까가 이유이다.

BEGIN(Engine)

//TODO VIBuffer = Vertex + Index + Buffer(메모리 공간)
class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render();

public:
	//TODO 정점, 인덱스, 인스턴스 버퍼들을 생성한다.
	HRESULT Bind_VIBuffers();
	HRESULT Create_Buffer(_Inout_ ID3D11Buffer * *ppBuffer);
	//! 아래 주석에서 설명하듯 버퍼타입이 동일하기에 Create_Buffer 함수 하나로 한번에 처리가 가능해진다.

protected:
	//TODO DX9과 다르게 정점버퍼와 인덱스버퍼가 사용하는 버퍼타입이 ID3D11Buffer로 동일한 것을 알 수 있다
	ID3D11Buffer* m_pVB = { nullptr }; //! 버텍스 버퍼 ( 정점 구조체들의 집합 )
	ID3D11Buffer* m_pIB = { nullptr }; //! 인덱스 버퍼 ( 그리는 순서에 따른 정점의 인덱스 집합 == 정수 집합

	//TODO 지금 내가 만들려고 하는 버퍼의 속성을 설정한다.
	//! D3D11_BUFFER_DESC : 정점버퍼인지 인덱스버퍼인지 , 정적 버퍼인지 동적 버퍼인지 등의 값을
	//! 자식 개체에서 채워 준 후 부모의 Create_Buffer() 함수를 호출하는 식으로 할 것.
	D3D11_BUFFER_DESC			m_BufferDesc; 

	//! D3D11_SUBRESOURCE_DATA : 아래의 void* pSystem 에게 초기값을 원하는 값을 줘서 사용할 것
	//! 마찬가지로 자식개체에서 채워 줌
	
	//! typedef struct D3D11_SUBRESOURCE_DATA
	//! {
	//! 	const void* pSysMem;
	//! 	UINT SysMemPitch;
	//! 	UINT SysMemSlicePitch;
	//! } 	D3D11_SUBRESOURCE_DATA;
	
	
	D3D11_SUBRESOURCE_DATA		m_SubResourceData;

protected:
	_uint						m_iNumVertices = { 0 }; //! 정점 개수
	_uint						m_iStride = { 0 }; //! 정점 하나의 크기
	_uint						m_iNumVertexBuffers = { 0 };

	_uint						m_iNumIndices = { 0 }; //! 인덱스 개수
	_uint						m_iIndexStride = { 0 }; //! 인덱스 하나의 크기
	DXGI_FORMAT					m_eIndexFormat = {  };
	D3D11_PRIMITIVE_TOPOLOGY	m_eTopology = { };

	

public:
	virtual CComponent* Clone(void* pArg) = 0; //! 복사를 강제한다
	virtual void Free() override;
};

END

