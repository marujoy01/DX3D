#pragma once
//TODO 파이프라인 객체의 역할
//! 인게임 내의 객체들을 그릴 때 공통으로 사용되는 뷰, 투영행렬들을 보관한다.

#include "Base.h"

BEGIN(Engine)

class CPipeLine final : public CBase
{
public:
	enum D3DTRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };

private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	//TODO 저장용, 연산용에 따른 오버로딩
	void Set_Transform(D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix);
	void Set_Transform(D3DTRANSFORMSTATE eState, _float4x4 TransformMatrix);

public:
	_matrix Get_TransformMatrix(D3DTRANSFORMSTATE eState);
	_float4x4 Get_TransformFloat4x4(D3DTRANSFORMSTATE eState);

	_matrix Get_TransformMatrixInverse(D3DTRANSFORMSTATE eState);
	_float4x4 Get_TransformFloat4x4Inverse(D3DTRANSFORMSTATE eState);

	_float4 Get_CamPosition();

public:
	HRESULT Initialize();
	//TODO 틱함수?
	//! 게임인스턴스에 틱엔진 함수에서 호출할 것이다. 매 틱마다 카메라가 가지고있는 뷰, 투영행렬을 Set해줄 것.
	void	Tick();

private:
	//TODO 멤버변수를 배열로 사용한 이유
	//! 배열로 안두었다면 뷰행렬이냐 투영행렬이냐에 따라 조건분기를 했어야했다. 그게 싫었고 밖에서 enum값만 던져주면 값에맞는 행렬을 Get,Set 하고싶었다.
	_float4x4		m_Transform[D3DTS_END];

	//! 4X4타입의 역행렬을 구하는 작업은 생각보다 무겁다. 그래서 필요할때마다 Get_TransformFloat4x4Inverse 함수를 호출하기 보다는 
	//! 애초에 Tick 함수에서 역행렬을 구해서 멤버변수로 저장해두는 형식으로 가는게 좋았다.
	_float4x4		m_Transform_Inverse[D3DTS_END];

	//! 나중에 빛 연산을 하게되면 카메라의 월드 위치를 요구하는 상황이 많이 생긴다. 그때 카메라의 역행렬(카메라의 월드)의 4행(월드위치)를 넘겨주기 위해 마찬가지로 저장해준다.
	_float4			m_vCamPosition;

public:
	static CPipeLine*	Create();
	virtual void		Free() override;
};

END

