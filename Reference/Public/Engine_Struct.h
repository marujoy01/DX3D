
namespace Engine
{
	typedef struct tagGraphicDesc
	{
		enum WINMODE { WINMODE_FULL, WINMODE_WIN, WINMODE_END };

		WINMODE			eWinMode; //! ��� â��带 ����� ���ΰ�. WINMODE_FULL ��üȭ��, WINMODE_WIN â ȭ��
		HWND			hWnd; //! ��� â�ΰ�
		unsigned int	iBackBufferSizeX; //! ����ۿ��� �� â�� ���λ�����
		unsigned int	iBackBufferSizeY; //! ����ۿ��� �� â�� ���λ�����

	}GRAPHIC_DESC;

	typedef struct
	{
		class CTexture* pMtrlTextures[AI_TEXTURE_TYPE_MAX];
	}MATERIAL_DESC;

	typedef struct ENGINE_DLL tagVertex_Postition // �׺� �޽��� ������ ����ü
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

	/* error LNK1120: 1���� Ȯ���� �� ���� �ܺ� �����Դϴ� : ����Ƽ ���� ���� ����, ENGINE_DLL�ڿ� tag�� �ٿ�����Ѵ�.*/
	typedef struct ENGINE_DLL tagVoertex_Position_Normal_Texcoord_Tangent
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexcoord;
		XMFLOAT3		vTangent;

		static const unsigned int					iNumElements = 4;
		static const D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXMESH;

	/* Animation */	// �ܺ����� error : ENGINE_DLL �ڿ� tag�� �޾Ƶα�
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
		XMFLOAT4	vPosition = { 0.f, 0.f, 0.f, 0.f };	// ������Ʈ�� ��ġ��
		wstring		cModelTag;			// ������Ʈ�� ����� ������Ʈ�� ���� �ױ�
		wstring		cShaderTag;			// ������Ʈ�� ����� ������Ʈ�� ���� �ױ�

	}ANYTHINGOBJ;

	typedef struct tagKeyFrame
	{
		XMFLOAT3		vScale, vPosition;
		XMFLOAT4		vRotation;
		float			fTrackPosition;
	}KEYFRAME;
	
	//// �޽� ������
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


