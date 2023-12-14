#include "VIBuffer_Rect.h"

CVIBuffer_Rect::CVIBuffer_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Rect::CVIBuffer_Rect(const CVIBuffer_Rect& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Rect::Initialize_Prototype()
{
	m_iNumVertexBuffers = 1; //! 나중에는 여러개의 버퍼를 사용할수 있어서. 현재는 1개만 사용
	m_iNumVertices = 4; //! 사각형이니까 정점의 개수는 4개
	m_iStride = sizeof(VTXPOSTEX); //! 정점 한개의 크기는 한 정점이 가지고 있어야할 정보의 사이즈(구조체)

	m_iNumIndices = 6; //! 사각형을 삼각형으로 바꿀꺼야. [0,1,2], [0,2,3] 총 6개야

	//TODO 인덱스 사이즈에 대한 설명
	//! 먼저 인덱스의 사이즈는 2 바이트와 4 바이트 둘 중에 하나만 사용이 가능하다.
	//! 2 바이트인 경우에는 unsigned short의 사이즈, 4 바이트인 경우에는 unsigned int의 사이즈를 말한다.
	//! 즉, 정점의 개수가 unsigned short가 가질 수 있는 최대 값인 65535를 넘는다면 사이즈를 4로 줘야하고, 아니라면 2로 준다는 얘기
	
	m_iIndexStride = 2; //! 여기서는 정점의 개수가 65535를 넘지 않을 것이니 2를 준 것이다.

	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


#pragma region VERTEX_BUFFER 
//TODO 정점 버퍼의 대한 정보를 넣어주는 행위

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc); //! 초기화

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices; //! 할당할 크기를 말한다.
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/; //! 정적 버퍼를 사용한다
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //! 정점버퍼 정보 채울 거다
	m_BufferDesc.CPUAccessFlags = 0; //! 동적 버퍼일 경우에만 유효한 값이다. 정적버퍼니 0으로 채운다
	m_BufferDesc.MiscFlags = 0; //! 위와 같다

	//todo StructureByteStride : 덩어리로 할당된 공간의 데이터를 읽을때 어떤 단위로 읽을 것이냐
	m_BufferDesc.StructureByteStride = m_iStride; //! 정점 크기 단위로 읽을거다.

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);


	//TODO 정점에 관한 이야기
	//! 정점을 바꾼다는 것은 실제 버퍼의 배열에게 접근해서 원본 데이터의 정점을 바꿨을 때를 말한다.
	//! 우리가 렌더링 파이프라인을 하는 과정은, 원본 정점의 데이터를 복사해서 그걸 변환시켜서 사용하는 개념.
	//! 즉, 렌더링 파이프라인은 정점을 바꾸는 행위가 아니라, 변환해서 사용하는 개념


	//TODO 오해방지
	//! 바로 아래에서 VTXPOSTEX 구조체를 할당하는 행위는 m_SubResourceData.pSysMem의 값을 채우려고 하는 행위
	//! 절대 버퍼를 만들었다는 행위가 아니라는 것.
	//! 그래서 값을 채워주고 71번 라인에서 진짜로 버퍼를 생성 한뒤 필요없어졌으니 삭제하는 것이다.

	VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];

	//TODO 정점들의 정보를 1 기준으로 셋팅하는 이유
	//! 로컬 정점의 중앙은 0,0 이다. 
	//! 센터값을 1기준으로 맞춰놓으면 나중에 스케일 값을 줄때 배수를 생각할 필요없이 원하는 값을 줄 수 있다
	//! ex) m_pTransform->Set_Scale(15.f,15.f,15.f), 만약 1기준이 아니라 2기준이었다면 스케일은 30이 되었을 것. 
	//! 1기준이라 스케일은 15가 되는 것이다.
	//! 즉, -0.5, 0, +0.5. 중점 기준으로 양 쪽의 총 길이를 1로 만든것이다.
	

	pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f); //! 왼쪽 위
	pVertices[0].vTexcoord = _float2(0.0f, 0.f); //! 왼쪽 위

	//TODO vTexcoord = UV좌표계 왼쪽 위 = 0,0 오른쪽 아래 = 1, 1
	pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexcoord = _float2(1.0f, 0.f); //! 오른쪽 위

	pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexcoord = _float2(1.0f, 1.0f); //! 오른쪽 아래

	pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexcoord = _float2(0.0f, 1.0f);//! 왼쪽 아래

	m_SubResourceData.pSysMem = pVertices;

	//! pVertices에 할당하여 채워놨던 정점들의 정보를 ID3D11Buffer로 할당한 공간에 복사하여 채워넣는다.
	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

//TODO 정점 버퍼의 대한 정보를 넣어주는 행위 끝
#pragma endregion

#pragma region INDEX_BUFFER
//TODO 인덱스 버퍼의 대한 정보를 넣어주는 행위 끝
	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; //! 인덱스 버퍼 정보 채울꺼야
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);

	_ushort* pIndices = new _ushort[m_iNumIndices]; //! 정점 개수가 65535 안넘으니까 _ushort

	pIndices[0] = 0; //! 사각형 왼쪽 위부터 시계방향 기준으로 첫번째 삼각형 
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 0; //! 사각형 왼쪽 위부터 시계방향 기준으로 두번째 삼각형 
	pIndices[4] = 2;
	pIndices[5] = 3;

	//! 사각형 완성

	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

//TODO 인덱스 버퍼의 대한 정보를 넣어주는 행위 끝
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Rect::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_Rect* CVIBuffer_Rect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Rect* pInstance = new CVIBuffer_Rect(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Rect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Rect::Clone(void* pArg)
{
	CVIBuffer_Rect* pInstance = new CVIBuffer_Rect(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Rect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Rect::Free()
{
	__super::Free();
}
