#pragma once

#include "Base.h"
#include "Model.h"

/* 이 애니메이션에서 사용하는 뼈의 정보다. */
/* 시간대에 따른 뼈의 상태(Scale, Rotation, Position)를 저장한다.*/

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const aiNodeAnim* pChannel, const CModel::BONES& Bones);
	void Invalidate_TransformationMatrix(_float fCurrentTrackPosition, const CModel::BONES& Bones, _uint* pCurrentKeyFrameIndex);

private:
	_char				m_szName[MAX_PATH] = "";
	_uint				m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>	m_KeyFrames;
	//_uint				m_iCurrentKeyFrameIndex = { 0 };	// 현재 키프레임이 어디있는지 알기위한 인덱스
	// m_iCurrentKeyFrameIndex가 클론간에 공유가 되어 문제가 되니, 밖(애니메이션)에서 vector 컨테이너로 받게 빼주자.
	// m_iCurrentKeyFrameIndex 하나때문에 채널 클래스를 복사 생성하게 되는건 많이 비효율 적이기 때문이다.

	_uint				m_iBoneIndex = { 0 }; // 몇번째 뼈에게 접근할지 정하기 위한 인덱스

public:
	static CChannel* Create(const aiNodeAnim* pChannel, const CModel::BONES& Bones);
	virtual void Free() override;
};

END