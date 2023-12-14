#pragma once

#include "Base.h"
#include "Model.h" // 비표준 어쩌고.. 오버로드가 어쩌고 error : cpp가 아니라 헤더에 헤더를 포함시켜서 해결

/* 특정 애니메이션(대기, 걷기, 뛰기, 때리기, 맞기) 을 표현하기위한 데이터들을 가진다. */

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, const CModel::BONES& Bones);
	void Invalidate_TransformationMatrix(_bool isLoop, _float fTimeDelta, const CModel::BONES& Bones);

private:
	_char				m_szName[MAX_PATH] = "";
	_float				m_fDuration = { 0.0f }; /* 내 애니메이션을 전체 재생하기위한 전체 길이. */
	_float				m_fTickPerSecond = { 0.f }; /* 애니메이션의 재생 속도 : m_TickPerSecond * fTimeDelta */
	_float				m_fTrackPosition = { 0.f }; /* 현재 재생되고 있는 위치. */

	_uint				m_iNumChannels = { 0 }; /* 이 애니메이션이 사용하는 뼈의 갯수. */

	vector<class CChannel*>	m_Channels;
	vector<_uint>			m_CurrentKeyFrames; // 현재 키프레임은 복사된 객체들과 공유가 되어선 안된다. 따라서 벡터 컨테이너에 채널 개수에 맞게 넣어주자. (각자 현재 시간에 다른 애니메이션을 돌릴 수 있어야 하기 때문)

	_bool					m_isFinished = { false }; // 애니메이션 재생 완료 여부 (끝났는지)

public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, const CModel::BONES& Bones);
	CAnimation* Clone();
	virtual void Free() override;
};

END