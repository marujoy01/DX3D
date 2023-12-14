
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
}


