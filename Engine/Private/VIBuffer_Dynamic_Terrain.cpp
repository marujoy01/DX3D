#include "..\Public\VIBuffer_Dynamic_Terrain.h"

CVIBuffer_Dynamic_Terrain::CVIBuffer_Dynamic_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Dynamic_Terrain::CVIBuffer_Dynamic_Terrain(const CVIBuffer_Dynamic_Terrain& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Dynamic_Terrain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Dynamic_Terrain::Initialize(void* pArg)
{
	VTXDYNAMIC* pInfo = (VTXDYNAMIC*)pArg;

	m_fInterval = 0.5f;
	m_iNumVerticesX = (_uint)pInfo->vPosition.x;
	m_iNumVerticesZ = (_uint)pInfo->vPosition.z;

	m_iStride = sizeof(VTXDYNAMIC);
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	m_iNumVertexBuffers = 1;

	//!  ���ؽ� ���� ����
	VTXDYNAMIC* pVertices = new VTXDYNAMIC[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXDYNAMIC) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (size_t j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			pVertices[iIndex].vPosition = _float3(_float(j * m_fInterval), 0.f, _float(i * m_fInterval));
			pVertices[iIndex].vTexUV = _float2(j / (m_iNumVerticesX - 1.f), i / (m_iNumVerticesZ - 1.f));
			pVertices[iIndex].vNormal = _float3(0.f, 1.f, 0.f);
			pVertices[iIndex].vTangent = _float3(1.f, 0.f, 0.f);

			m_VertexInfo.push_back(pVertices[iIndex]);
		}
	}
	//!���ؽ� ���� ����

	//! �ε��� ���� ����
	m_iIndexStride = sizeof(FACEINDICES32);
	m_iNumPrimitive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_iNumIndices = m_iNumPrimitive * 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	FACEINDICES32* pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	_ulong  iNumIndices = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_ulong iIndex = i * m_iNumVerticesX + j;

			_ulong iIndices[] =
			{
			   iIndex + m_iNumVerticesX,
			   iIndex + m_iNumVerticesX + 1,
			   iIndex + 1,
			   iIndex
			};

			pIndices[iNumIndices] = { iIndices[0], iIndices[1], iIndices[2] };
			m_Indices.push_back(_uint3(pIndices[iNumIndices]._1, pIndices[iNumIndices]._2, pIndices[iNumIndices]._3));
			++iNumIndices;

			pIndices[iNumIndices] = { iIndices[0], iIndices[2], iIndices[3] };
			m_Indices.push_back(_uint3(pIndices[iNumIndices]._1, pIndices[iNumIndices]._2, pIndices[iNumIndices]._3));
			++iNumIndices;
		}
	}
	//! �ε��� ���� ����


	//! ���� ���� ����
	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.StructureByteStride = m_iStride;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(Create_Buffer(&m_pVB)))
		return E_FAIL;
	//!���� ���� ����

	//!�ε��� ���� ����
	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_BufferDesc.MiscFlags = 0;

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(Create_Buffer(&m_pIB)))
		return E_FAIL;
	//! �ε��� ���� ����

	Safe_Delete_Array(pVertices);
	Safe_Delete_Array(pIndices);

	return S_OK;
}

// ���� ���� Get
_bool CVIBuffer_Dynamic_Terrain::Get_Vertex(_uint _iIndex, VTXDYNAMIC* _pOut)
{
	if (m_VertexInfo.size() <= _iIndex)
		return false;

	*_pOut = m_VertexInfo[_iIndex];

	return true;
}

// �ε��� ���� Get
_bool CVIBuffer_Dynamic_Terrain::Get_Indices(_uint _iIndex, _uint3* _pOut)
{
	if (m_Indices.size() <= _iIndex)
		return false;

	*_pOut = m_Indices[_iIndex];

	return true;
}

void CVIBuffer_Dynamic_Terrain::Tick(_vector _vMousePos, _float _fRadious, _float _fPower, _uint _iMode)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource; // Map�Լ��� �־��ִ� �༮

	// ID3D11DeviceContext::Map() : ���� ���ҽ��� ���Ե� �����Ϳ� ���� �����͸� ��������, GPU �׼����� �ź��Ѵ� = (Unlock)

	/*	Map(���)�� ���� ��
		1. �������̽� ������ : �ٲٷ��� �༮�� �ּ�(2D�ؽ�ó, ����.. ��� ID3D11Resource�� ��ӹ޴� ��� �༮���� �ּ�), �׳� 0, SubResource);
		2. ���� ���ҽ��� �ε��� ��ȣ : ���� �θ��� �������� ��� �ٲܰ��� �������ִ� ���� ��
		3. CPU �б�, ���� ���� ���� : DISCARD �Ǵ� NO_OVERWRITE
			-> DISCARD : �������� ������, ���� ���� ä���.
			-> NO_OVERWRITE : �������� �����ϰ�, �߰������� ���� ä���.
		4. GPU�� ������϶� �����ϴ� �۾� ���� : ä������ ����. 0���� ����
		5. ��� �� : D3D11_MAPPED_SUBRESOURCE ����ü�� ���� ������
	*/
	m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	// ������ ���� �� ����
	//m_iNumVerticesX = LONG(512);
	//m_iNumVerticesZ = LONG(512);

#pragma region ���콺 ��ŷ
	// ���콺�� ��ŷ�� ����� �ε�����ȣ
	_int2	iPickIndex = { _int(_vMousePos.m128_f32[0] / m_fInterval), _int(_vMousePos.m128_f32[2] / m_fInterval) };

	// ?
	_int	iRoundIndx = (_int)(_fRadious / m_fInterval);

	// �ε���
	_int2	iBeginIndex, iEndIndex;
	iBeginIndex.x = (0 > iPickIndex.x - iRoundIndx) ? (0) : (iPickIndex.x - iRoundIndx);
	iBeginIndex.y = (0 > iPickIndex.y - iRoundIndx) ? (0) : (iPickIndex.y - iRoundIndx);

	iEndIndex.x = ((_int)m_iNumVerticesX < iPickIndex.x + iRoundIndx) ? (m_iNumVerticesX) : (iPickIndex.x + iRoundIndx);
	iEndIndex.y = ((_int)m_iNumVerticesZ < iPickIndex.y + iRoundIndx) ? (m_iNumVerticesZ) : (iPickIndex.y + iRoundIndx);
#pragma endregion

	// �ε��� x,y�� ���ۺ��� ������ ��ȸ (����, ����)
	for (_uint iZ(iBeginIndex.y); iZ < (_uint)iEndIndex.y; ++iZ)
	{
		for (_uint iX(iBeginIndex.x); iX < (_uint)iEndIndex.x; ++iX)
		{
			_ulong	iIndex = iZ * m_iNumVerticesX + iX; // �ε��� ��ȣ : iZ�� 0�϶� 0,1,2,3,4,5 1�϶� ����X����, ����X����+1, ����X����+2, ����X����+3, ����X����+4

			_float3 vPos = ((VTXDYNAMIC*)SubResource.pData)[iIndex].vPosition;	// vPos = iIndex��°�� �ȼ� ������ ��ġ�� �޾ƿ´�.
			_float  fLength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vPos) - _vMousePos));	// fLength = ���� ���� ��ġ�� ���콺�� ���� ���̸� ���Ѵ�.

			switch (_iMode) // ��� ����
			{
			case 0: //
			{
				if (_fRadious < fLength)
					continue;

				((VTXDYNAMIC*)SubResource.pData)[iIndex].vPosition.y += _fPower;
				m_VertexInfo[iIndex].vPosition = ((VTXDYNAMIC*)SubResource.pData)[iIndex].vPosition;
			}
			break;

			case 1: //
			{
				if (_fRadious < fLength)
					continue;

				_float fLerpPower = _fPower * (1.f - pow((fLength / _fRadious), 2.f));

				((VTXDYNAMIC*)SubResource.pData)[iIndex].vPosition.y += fLerpPower;
				m_VertexInfo[iIndex].vPosition = ((VTXDYNAMIC*)SubResource.pData)[iIndex].vPosition;
			}
			break;

			case 2: //
			{
				((VTXDYNAMIC*)SubResource.pData)[iIndex].vPosition.y = _fPower;
				m_VertexInfo[iIndex].vPosition = ((VTXDYNAMIC*)SubResource.pData)[iIndex].vPosition;
			}
			break;
			}
		}
	}

	// ��ŷ�� �ε��� x,y�� ���ۺ��� ������ ��ȸ
	for (_uint iZ(iBeginIndex.y); iZ < (_uint)iEndIndex.y; ++iZ)
	{
		for (_uint iX(iBeginIndex.x); iX < (_uint)iEndIndex.x; ++iX)
		{
			_ulong	iIndex = iZ * m_iNumVerticesX + iX;	// �� �ε����� ����

			_long  iAdjacency[] =
			{
				_long(iIndex + m_iNumVerticesX),		// �»��
				_long(iIndex + m_iNumVerticesX + 1),	// ����
				_long(iIndex + 1),						// ���ϴ�
				_long(iIndex)							// ���ϴ�
			};

			#pragma region ���ǹ�
			if (0 == iX)
				iAdjacency[3] = -1;

			if (m_iNumVerticesX - 1 == iX)
				iAdjacency[1] = -1;

			if (0 == iZ)
				iAdjacency[2] = -1;

			if (m_iNumVerticesZ - 1 == iZ)
				iAdjacency[0] = -1;
			#pragma endregion

			// ������ �븻 ������
			_float3 vNorm = m_VertexInfo[iIndex].vNormal;

			// �븻 ���� ���
			_vector vComputeNorm = XMVectorSet(0.f, 0.f, 0.f, 0.f);

			// �븻 ���͸� ��ȸ�ϸ� �����ش�.
			for (_uint i = 0; i < 4; ++i)
			{
				_uint iNext = (3 == i) ? (0) : (i + 1);

				if (0 > iAdjacency[i] || 0 > iAdjacency[iNext])
					continue;

				// ���� ���͸� ���Ѵ�.
				_vector vLine_no1 = XMLoadFloat3(&m_VertexInfo[iAdjacency[i]].vPosition) - XMLoadFloat3(&m_VertexInfo[iIndex].vPosition);
				_vector vLine_no2 = XMLoadFloat3(&m_VertexInfo[iAdjacency[iNext]].vPosition) - XMLoadFloat3(&m_VertexInfo[iIndex].vPosition);
				_vector vLingNorm = XMVector3Normalize(XMVector3Cross(vLine_no1, vLine_no2)); // �� ���� ���͸� ����

				// �븻 ����
				vComputeNorm = XMVector3Normalize(vComputeNorm + vLingNorm);
			}

			// ������ �븻 ����, ���� ������ �ٶ󺸴� �븻 ����
			XMStoreFloat3(&vNorm, vComputeNorm);

			m_VertexInfo[iIndex].vNormal = vNorm;
			((VTXDYNAMIC*)SubResource.pData)[iIndex].vNormal = vNorm;

			if (0 > iAdjacency[1])
				continue;

			// ź��Ʈ ���� ���
			_float3 vTempTangent;
			XMStoreFloat3(&vTempTangent, XMVector3Normalize(XMLoadFloat3(&m_VertexInfo[iAdjacency[1]].vPosition) - XMLoadFloat3(&m_VertexInfo[iIndex].vPosition)));

			m_VertexInfo[iIndex].vTangent = vTempTangent;
			((VTXDYNAMIC*)SubResource.pData)[iIndex].vTangent = vTempTangent;
		}
	}

	m_pContext->Unmap(m_pVB, 0);
}

// float
_bool CVIBuffer_Dynamic_Terrain::Compute_MousePos(RAY _Ray, _matrix _WorldMatrix, _float3* pOut)
{
	_matrix		WorldMatrix = XMMatrixInverse(nullptr, _WorldMatrix);
	_vector		vRayPos, vRayDir;

	vRayPos = XMVector3TransformCoord(XMLoadFloat4(&_Ray.vOrigin), WorldMatrix);
	vRayDir = XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&_Ray.vDirection), WorldMatrix));

	_float		fDist;

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
	{
		_uint3		iIndices = m_Indices[i];
		_vector		vPickedPos;

		_vector	vVec0 = XMLoadFloat3(&m_VertexInfo[iIndices.ix].vPosition);
		_vector	vVec1 = XMLoadFloat3(&m_VertexInfo[iIndices.iy].vPosition);
		_vector	vVec2 = XMLoadFloat3(&m_VertexInfo[iIndices.iz].vPosition);

		if (true == DirectX::TriangleTests::Intersects(vRayPos, vRayDir, vVec0, vVec1, vVec2, fDist))
		{
			vPickedPos = vRayPos + XMVector3Normalize(vRayDir) * fDist;
			XMStoreFloat3(pOut, vPickedPos); // �� �� ����

			return true;
		}
	}

	return false;
}

// vector
_bool CVIBuffer_Dynamic_Terrain::Compute_MousePos_float4(RAY _Ray, _matrix _WorldMatrix, _float4* pOut)
{
	_matrix		WorldMatrix = XMMatrixInverse(nullptr, _WorldMatrix);
	_vector		vRayPos, vRayDir;

	vRayPos = XMVector3TransformCoord(XMLoadFloat4(&_Ray.vOrigin), WorldMatrix);
	vRayDir = XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&_Ray.vDirection), WorldMatrix));

	_float		fDist;

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
	{
		_uint3		iIndices = m_Indices[i];
		_vector		vPickedPos;

		_vector	vVec0 = XMLoadFloat3(&m_VertexInfo[iIndices.ix].vPosition);
		_vector	vVec1 = XMLoadFloat3(&m_VertexInfo[iIndices.iy].vPosition);
		_vector	vVec2 = XMLoadFloat3(&m_VertexInfo[iIndices.iz].vPosition);

		if (true == DirectX::TriangleTests::Intersects(vRayPos, vRayDir, vVec0, vVec1, vVec2, fDist))
		{
			vPickedPos = vRayPos + XMVector3Normalize(vRayDir) * fDist;
			//XMStoreFloat3(pOut, vPickedPos); // �� �� ����
			XMStoreFloat4(pOut, vPickedPos); // �� �� ����

			return true;
		}
	}

	return false;
}

CVIBuffer_Dynamic_Terrain* CVIBuffer_Dynamic_Terrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Dynamic_Terrain* pInstance = new CVIBuffer_Dynamic_Terrain(pDevice, pContext);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Dynamic_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Dynamic_Terrain::Clone(void* pArg) // Ŭ�� ������ �������� ����ü�� �޴´�.
{
	CVIBuffer_Dynamic_Terrain* pInstance = new CVIBuffer_Dynamic_Terrain(*this);

	/* ������ü�� �ʱ�ȭ�Ѵ�.  */
	if (FAILED(pInstance->Initialize(pArg)))	// ���� ������ ���� ����ü �Ѱ��ֱ�
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Dynamic_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Dynamic_Terrain::Free()
{
	__super::Free();
}
