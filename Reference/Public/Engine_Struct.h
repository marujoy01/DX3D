
namespace Engine
{
	typedef struct tagGraphicDesc
	{
		enum WINMODE { WINMODE_FULL, WINMODE_WIN, WINMODE_END };

		WINMODE			eWinMode; //! 어떠한 창모드를 사용할 것인가. WINMODE_FULL 전체화면, WINMODE_WIN 창 화면
		HWND			hWnd; //! 어떠한 창인가
		unsigned int	iBackBufferSizeX; //! 백버퍼에게 줄 창의 가로사이즈
		unsigned int	iBackBufferSizeY; //! 백버퍼에게 줄 창의 세로사이즈

	}GRAPHIC_DESC;

	typedef struct
	{
		class CTexture* pMtrlTextures[AI_TEXTURE_TYPE_MAX];
	}MATERIAL_DESC;

	typedef struct ENGINE_DLL tagVertex_Postition // 네비 메쉬용 포지션 구조체
	{
		XMFLOAT3		vPosition;

		static const unsigned int					iNumElements = 1;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXPOS;

	typedef struct tagPassDesc
	{
		ID3D11VertexShader* VertexShader = { nullptr };
		ID3D11PixelShader* PixelShader = { nullptr };
		ID3D11InputLayout* layout = { nullptr };
	}PASS_DESC;

	typedef struct ENGINE_DLL tagVertex_Position_Texcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexcoord;

		static const unsigned int				iNumElements = 2;
		static const D3D11_INPUT_ELEMENT_DESC   Elements[iNumElements];
	}VTXPOSTEX;

	typedef struct ENGINE_DLL tagVertex_Position_Normal_Texcoord
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;

		static const unsigned int					iNumElements = 3;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXNORTEX;

	/* error LNK1120: 1개의 확인할 수 없는 외부 참조입니다 : 유니티 빌드 사용시 에러, ENGINE_DLL뒤에 tag를 붙여줘야한다.*/
	typedef struct ENGINE_DLL tagVoertex_Position_Normal_Texcoord_Tangent
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		static const unsigned int					iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXMESH;

	/* Animation */	// 외부참조 error : ENGINE_DLL 뒤에 tag꼭 달아두기
	typedef struct ENGINE_DLL tagAnimMesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndices;
		XMFLOAT4		vBlendWeights;

		static const unsigned int					iNumElements = 6;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXANIMMESH;

	/* Dynamic Terrain*/
	typedef struct tagVertex_Dynamic_Texture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
		XMFLOAT3		vTangent;
	}VTXDYNAMIC;

	typedef struct tagFaceIndices32
	{
		unsigned long _1, _2, _3;
	}FACEINDICES32;

	/* Ray */
	typedef struct tagRayDesc
	{
		XMFLOAT4	vOrigin;
		XMFLOAT3	vDirection;
		float		fLength;
	}RAY;
	
	/* Anything Object */
	typedef struct tagAnythingObject
	{
		XMFLOAT4	vPosition = { 0.f, 0.f, 0.f, 0.f };	// 오브젝트의 위치값
		wstring		cModelTag;			// 오브젝트가 사용할 컴포넌트의 원본 테그
		wstring		cShaderTag;			// 오브젝트가 사용할 컴포넌트의 원본 테그

	}ANYTHINGOBJ;

	typedef struct tagKeyFrame
	{
		XMFLOAT3		vScale, vPosition;
		XMFLOAT4		vRotation;
		float			fTrackPosition;
	}KEYFRAME;
	
	//// 메쉬 데이터
	//typedef struct tag_MeshVertextPostion
	//{
	//	_float3 vMin;
	//	_float3 vMax;
	//	_float3 vCenter;

	//} MESH_VTX_INFO;

	enum EDIT_MODE
	{
		HEIGHT_FLAT,
		HEIGHT_LERP,
		HEIGHT_SET,
		FILLTER,
		NON,
		EDIT_END
	};
}


