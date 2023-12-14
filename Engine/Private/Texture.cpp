#include "Texture.h"
#include "Shader.h"

CTexture::CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice,pContext)
{
}

CTexture::CTexture(CTexture& rhs)
	: CComponent(rhs)
	, m_iNumTextures(rhs.m_iNumTextures)
	, m_SRVs(rhs.m_SRVs)
{
	for(auto& pSRV : m_SRVs)
		Safe_AddRef(pSRV);
}

HRESULT CTexture::Initialize_Prototype(const wstring& strTextureFilePath, _uint iNumTextures)
{
	m_iNumTextures = iNumTextures;

	//TODO 텍스쳐의 개수는 정해져있다. 그러니 vector 컨테이너를 사용한것이고. 그럼 reserve로 잡아줘서 쓸모없는 메모리 공간을 할당하지 않게해주자
	m_SRVs.reserve(m_iNumTextures);

	for (size_t i = 0; i < m_iNumTextures; i++)
	{
		/* ..\정의훈\139\Framework\Client\Bin\Resources\Textures\Default.jpg */
		_tchar		szFullPath[MAX_PATH] = TEXT("");


		//TODO 만약 Default%d 였다면 뒤에 i가 붙고 아닐경우에는 i가 붙지 않는다. 그래서 Default.jpg로 줘도 아무런 일도 수행하지않아서 그대로 사용가능한 것.
		wsprintf(szFullPath, strTextureFilePath.c_str(), i);

		/* D:\ */
		//TODO 경로의 드라이브가 무엇인지 받기위한 문자열 메모리 공간을 스택메모리에 할당했다. 하지만 들어온 경로의 드라이브가 적히지않고 ../ 상대경로가 들어올시에는 빈값이 된다.
		_tchar		szDrive[MAX_PATH] = TEXT("");

		/* 정의훈\139\Framework\Client\Bin\Resources\Textures\ */
		//TODO 드라이브를 제외한 디렉토리 경로를 잘라서 전달해준다.
		_tchar		szDirectory[MAX_PATH] = TEXT("");

		/* Default0 */
		_tchar		szFileName[MAX_PATH] = TEXT("");

		/* .jpg */
		_tchar		szExt[MAX_PATH] = TEXT("");

		//TODO 총 경로인 szFullPath만 입력값으로 주고 나머지는 출력값을 받기위한 공간을 주면된다. 지금은 확장자만 알면 되기에 0 또는 nullptr로 채우고 경로를 담을수 있는 공간을 주었다.
		_wsplitpath_s(szFullPath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);


		//! CreateDDSTextureFromFile 함수를 통해서 경로에있는 텍스처파일을 리소스뷰로 로드시킨다.
		ID3D11ShaderResourceView* pSRV = { nullptr };

		//! 기존에는 ID3D11Texture2D 텍스처 사전객체를 만든 후 m_pDevice->CreateShaderResourceView 하는 식으로 리소스뷰를 만들었어야했다.
		//! 하지만 CreateDDSTextureFromFile 함수의 내부에서 알아서 사전객체와 리소스뷰 까지 만들어줘서 굉장히 좋은 함수다.
		 

		HRESULT			hr = 0;

		//! DDS 확장자의 경우 분기
		if (!lstrcmp(szExt, TEXT(".dds")))
		{
			hr = CreateDDSTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);
		}
		//! 우리가 넣어준 TK(ToolKit)라이브러리는 tga 텍스처를 로드하지 못한다. 사용자에게 알리기 위한 분기
		else if (!lstrcmp(szExt, TEXT(".tga")))
		{
			MSG_BOX("tga 확장자는 로드할 수 없습니다.");
		}
		//! 나머지 윈도우 기본 확장자들 (png, jpg, bmp 등등)은 전부 WIC에 해당한다.
		else
		{
			hr = CreateWICTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);
			
		}

		if (FAILED(hr))
			return E_FAIL;

		m_SRVs.push_back(pSRV);
	}

	return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CTexture::Bind_ShaderResource(CShader* pShader, const _char* pConstantName, _uint iTextureIndex)
{
	return pShader->Bind_SRV(pConstantName, m_SRVs[iTextureIndex]);
}

HRESULT CTexture::Bind_ShaderResourceArray(CShader* pShader, const _char* pConstantName)
{
	//! 배열의 첫번째 주소는 해당 배열의 주소의 해당하기에 이중포인터 인자로 front를 넘겨준 것.
	return pShader->Bind_SRVs(pConstantName, &m_SRVs.front(), m_iNumTextures);
}

//string CTexture::Get_TextureKey() const
//{
//	return m_TextureKey;
//}

CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strTextureFilePath, _uint iNumTextures)
{
	CTexture* pInstance = new CTexture(pDevice, pContext);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize_Prototype(strTextureFilePath, iNumTextures)))
	{
		MSG_BOX("Failed to Created : CTexture");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CTexture::Clone(void* pArg)
{
	CTexture* pInstance = new CTexture(*this);

	/* 원형객체를 초기화한다.  */
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTexture");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for(auto& pSRV : m_SRVs)
		Safe_Release(pSRV);

	m_SRVs.clear();
}
