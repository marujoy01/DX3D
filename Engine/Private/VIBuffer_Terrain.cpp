#include "VIBuffer_Terrain.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const wstring& strHeightMapFilePath)
{
	_ulong	dwByte = 0;
	HANDLE  hFile = CreateFile(strHeightMapFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if(0 == hFile)
		return E_FAIL;

	//TODO 비트맵 파일의 3단계 구조
	//! 1. BITMAPFILEHEADER
	//! 2. BITMAPINFOHEADER
	//! 3. PIXEL
	
	//! 우린 높이맵.bmp로 부터 픽셀값 가져올건데 그럴려면 1,2번 과정 읽어내야 이제 픽셀 읽을수있다.

	BITMAPFILEHEADER fh; //! 생성된 날짜, 수정된 날짜 등을 갖고있는 놈

	//! 어디서 읽을건데?, 읽은거 어디로 가져올건데?, 읽은거 가져올 공간의 사이즈는 얼만데?, 읽어올 바이트는 어딘데?, 마지막건 필요없어
	ReadFile(hFile, &fh, sizeof fh, &dwByte, nullptr);

	BITMAPINFOHEADER ih; //! 파일의 포맷, 이미지의 가로세로사이즈(픽셀의 개수) 등을 갖고있는 놈

	ReadFile(hFile, &ih, sizeof ih, &dwByte, nullptr);

	_ulong*		pPixel = new _ulong[ih.biWidth * ih.biHeight]; //! 픽셀은 당연히 가로사이즈 * 세로사이즈
	ReadFile(hFile, pPixel, sizeof(_ulong) * ih.biWidth * ih.biHeight, &dwByte, nullptr);
	
	CloseHandle(hFile);

	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;

	m_iNumVertexBuffers = 1; //! 나중에는 여러개의 버퍼를 사용할수 있어서. 현재는 1개만 사용
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ; //! 픽셀로 정점 찍을거라메
	m_iStride = sizeof(VTXNORTEX); //! 지형은 빛연산할꺼야 그러니까 정점 정보에 NORMAL 포함시켜

	m_iNumIndices = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2 * 3; //! 인덱스 개수 몇개야?
	//! * 2는 왜 하는거야? : 사각형 하나그릴때 삼각형 두개 필요하잖아
	//! * 3은? : 삼각형 하나의 인덱스 3개 쓰잖아.

	//TODO 인덱스 사이즈에 대한 설명
	//! 먼저 인덱스의 사이즈는 2 바이트와 4 바이트 둘 중에 하나만 사용이 가능하다.
	//! 2 바이트인 경우에는 unsigned short의 사이즈, 4 바이트인 경우에는 unsigned int의 사이즈를 말한다.
	//! 즉, 정점의 개수가 unsigned short가 가질 수 있는 최대 값인 65535를 넘는다면 사이즈를 4로 줘야하고, 아니라면 2로 준다는 얘기
	
	m_iIndexStride = m_iNumVertices >= 65535 ? 4 : 2; //! 정점개수 65535 넘으면 4바이트로 잡고 아니면 2로 쓴다는 삼항조건문

	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


#pragma region VERTEX_BUFFER 

	VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];

	for (size_t i = 0; i < m_iNumVerticesZ; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX; j++)
		{
			_uint	iIndex = i * m_iNumVerticesX + j; //! i가 0일땐 0,1,2,3,4,5 1일땐 정점X개수, 정점X개수+1, 정점X개수+2, 정점X개수+3, 정점X개수+4

			//TODO 0x000000ff의 이유
			//! 높이맵은 그레이스케일로 되어있다. 회색이라는 것은 RGB가 동일하다는 얘기이다. 회색으로 높이를 표현했다. y의 높이값을 채워주기위해서
			//! 픽셀의 rgb중 아무거나 한개를 가져오면 됐는데, 시프트연산을 하지 않기위해서 B의 해당하는 0x000000ff를 사용하는 것

			pVertices[iIndex].vPosition = _float3(j, (pPixel[iIndex] & 0x000000ff) / 10.f, i); //! x 니까 j, y는 픽셀로부터, z니까 i
			pVertices[iIndex].vNormal = _float3(0.0f, 0.0f, 0.f);
			
			//TODO UV좌표로 i와 j를 쓰고싶었어
			//! uv좌표는 0 ~ 1이다. i와 j는 정점이 1025 X 1025라고 쳤을 때 각각 1025이다. 이것을 0~1사이 값으로 바꿔서 써야했다.
			//! 그래서 j / (m_iNumVerticeX - 1.0f) 식으로 나눴떤거고 1이아니라 1.0f 식으로 준 이유는 0~1사이 값으로 줘야하기때문에 소수가 날아가면 안됐다.
			
			pVertices[iIndex].vTexcoord = _float2(j / (m_iNumVerticesX - 1.0f), i / (m_iNumVerticesZ - 1.0f));
		}
	}

	Safe_Delete_Array(pPixel);

#pragma endregion

#pragma region INDEX_BUFFER
//TODO 인덱스 버퍼의 대한 정보를 넣어주는 행위 끝
	
	

	_uint* pIndices = new _uint[m_iNumIndices]; //! 정점 개수가 65535 넘으니까 

	_uint  iNumIndices = 0;

	//TODO -1 왜 해?
	//! 더 하면 잣대

	for (size_t i = 0; i < m_iNumVerticesZ - 1; i++)
	{
		for (size_t j = 0; j < m_iNumVerticesX - 1; j++)
		{
			_uint	iIndex = i * m_iNumVerticesX + j; //! 아까랑 똑같아~

			_uint iIndices[4] =
			{
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector		vSourDir, vDestDir, vNormal;

			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[1];
			pIndices[iNumIndices++] = iIndices[2];

			//TODO 삼각형 세개 됐으니까 노말 구해줄거야
			//! 방향 벡터 두개 구해서 외적할꺼야. 0에서 1바라보는 방향벡터 만들고, 1에서 2바라보는 방향벡터 만드는거야
			//! dx11 함수들은 벡터를 리턴하는 걸 잊지마

			vSourDir = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDestDir = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			vNormal = XMVector3Cross(vSourDir, vDestDir);

			//TODO 위에서 노말 벡터 구했잖아.
			//! 이제 노말 벡터를 정점의 정보로 넣어줘야지. 하나의 노말벡터를 누적할때마다 노말라이즈 해주는 게 키포인트야
			//! 수직벡터들의 평균을 구할 때의 노말라이즈 방법이 성립하려면 수직벡터들의 길이가 모두 같을때야. 보통 빛연산을 할때 정점 노말벡터들의 길이를 1로 맞춰놓아. 길이가 같아
			//! 정리하자면, 그래서 우리는 노말라이즈 방법을 사용해서 정확한 여러면이 겹치는 정점의 평균 노말벡터를 구할 수 있는 거야.
			//! 만약, 길이가 틀렸다면 직접 나누는 작업을 했어야 했을거야.
			
			vNormal = XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal;
			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMVector3Normalize(vNormal));

			vNormal = XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal;
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMVector3Normalize(vNormal));

			vNormal = XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal;
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMVector3Normalize(vNormal));

			//TODO 사각형은 삼각형 두개로 구성되있잖아 두번째 삼각형도 해줘야지
			pIndices[iNumIndices++] = iIndices[0];
			pIndices[iNumIndices++] = iIndices[2];
			pIndices[iNumIndices++] = iIndices[3];

			vSourDir = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDestDir = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
			vNormal = XMVector3Cross(vSourDir, vDestDir);

			vNormal = XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal;
			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMVector3Normalize(vNormal));

			vNormal = XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal;
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMVector3Normalize(vNormal));

			vNormal = XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal;
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMVector3Normalize(vNormal));
		}
	}
#pragma endregion

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc); //! 초기화

	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices; //! 할당할 크기를 말한다.
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/; //! 정적 버퍼를 사용한다
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //! 정점버퍼 정보 채울 거다
	m_BufferDesc.CPUAccessFlags = 0; //! 동적 버퍼일 경우에만 유효한 값이다. 정적버퍼니 0으로 채운다
	m_BufferDesc.MiscFlags = 0; //! 위와 같다
	m_BufferDesc.StructureByteStride = m_iStride; //! 정점 크기 단위로 읽을거다.

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT /*D3D11_USAGE_DYNAMIC*/;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; //! 인덱스 버퍼 정보 채울꺼야
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_SubResourceData, sizeof m_SubResourceData);
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
	Safe_Delete_Array(pVertices);



	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* pArg)
{
	return S_OK;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strHeightMapFilePath)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(strHeightMapFilePath)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void* pArg)
{
	CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();
}
