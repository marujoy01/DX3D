#include "..\Public\Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

// 복사 생성자 추가
CAnimation::CAnimation(const CAnimation& rhs)
	: m_fDuration(rhs.m_fDuration)
	, m_fTickPerSecond(rhs.m_fTickPerSecond)
	, m_fTrackPosition(rhs.m_fTrackPosition)
	, m_iNumChannels(rhs.m_iNumChannels)
	, m_Channels(rhs.m_Channels)
	, m_isFinished(rhs.m_isFinished)
	, m_CurrentKeyFrames(rhs.m_CurrentKeyFrames)
{
	strcpy_s(m_szName, rhs.m_szName);

	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, const CModel::BONES& Bones)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_fDuration = (_float)pAIAnimation->mDuration;
	m_fTickPerSecond = (_float)pAIAnimation->mTicksPerSecond;


	m_iNumChannels = pAIAnimation->mNumChannels;

	m_CurrentKeyFrames.resize(m_iNumChannels); // 현재 키프레임 벡터 컨테이너를 채널 개수에 맞게 resize로 사이즈를 잡고 0으로 초기화 시키자

	/* 이 애니메이션에서 사용하기위한 뼈(aiNodeAnim,채널)의 정보를 만든다. */
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		// 채널을 생성해주자.
		CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], Bones); // 채널을 생성할때 뼈 정보까지 넘겨주자
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel); // 생성된 채널을 벡터 컨테이너에 담아주자. 그래야 만들어둔걸 꺼내서 사용하니까
	}

	return S_OK;
}

//														루프 여부, 시간 값,			뼈 정보
void CAnimation::Invalidate_TransformationMatrix(_bool isLoop, _float fTimeDelta, const CModel::BONES& Bones)
{
	m_fTrackPosition += m_fTickPerSecond * fTimeDelta;

	// 트렉포지션이 듀레이션보다 커졌을 경우 (마지막 애니메이션까지 다 돌았을 경우)
	if (m_fTrackPosition >= m_fDuration)
	{
		// 완료 true
		m_isFinished = true;
		m_fTrackPosition = m_fDuration; // 트렉포지션을 듀레이션으로 고정해준다. (마지막 애니메이션)

		if (true == isLoop) // 만약 무한 재생을 시키려 루프를 true로 줬을 경우엔
		{
			m_fTrackPosition = 0.0f; // 트렉 포지션을 0으로 되돌린다. (트렉포지션이 0이면 동작을 0에 맞게 돌려놓는 코드를 채널에서 구현해뒀다)
			m_isFinished = false; // 애니메이션이 아직 끝나지 되지 않았다고 알려준다.
		}
	}

	/* 내 애니메이션이 이용하는 전체 뼈의 상태를 m_fTrackPosition 시간에 맞는 상태로 갱신하다.*/
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->Invalidate_TransformationMatrix(m_fTrackPosition, Bones, &m_CurrentKeyFrames[i]); // 뼈정보도 같이 보내주자.
		//													밖으로 뺀 현재 키프레임을 채널에 있는 함수에 넘겨주자.
	}
}

CAnimation* CAnimation::Create(const aiAnimation* pAIAnimation, const CModel::BONES& Bones) // 생성할 때 뼈를 받아오자
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAIAnimation, Bones))) // 받은 뼈를 이니셜라이즈에 넘겨주자
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this); // 건드리는거 없이 그냥 복사생성 해주자.
}

void CAnimation::Free()
{
	// 채널을 들고있는 애니메이션이 모두 순회하며 삭제해주자.
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}