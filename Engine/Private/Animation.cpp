#include "..\Public\Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

// ���� ������ �߰�
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

	m_CurrentKeyFrames.resize(m_iNumChannels); // ���� Ű������ ���� �����̳ʸ� ä�� ������ �°� resize�� ����� ��� 0���� �ʱ�ȭ ��Ű��

	/* �� �ִϸ��̼ǿ��� ����ϱ����� ��(aiNodeAnim,ä��)�� ������ �����. */
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		// ä���� ����������.
		CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], Bones); // ä���� �����Ҷ� �� �������� �Ѱ�����
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel); // ������ ä���� ���� �����̳ʿ� �������. �׷��� �����а� ������ ����ϴϱ�
	}

	return S_OK;
}

//														���� ����, �ð� ��,			�� ����
void CAnimation::Invalidate_TransformationMatrix(_bool isLoop, _float fTimeDelta, const CModel::BONES& Bones)
{
	m_fTrackPosition += m_fTickPerSecond * fTimeDelta;

	// Ʈ���������� �෹�̼Ǻ��� Ŀ���� ��� (������ �ִϸ��̼Ǳ��� �� ������ ���)
	if (m_fTrackPosition >= m_fDuration)
	{
		// �Ϸ� true
		m_isFinished = true;
		m_fTrackPosition = m_fDuration; // Ʈ���������� �෹�̼����� �������ش�. (������ �ִϸ��̼�)

		if (true == isLoop) // ���� ���� ����� ��Ű�� ������ true�� ���� ��쿣
		{
			m_fTrackPosition = 0.0f; // Ʈ�� �������� 0���� �ǵ�����. (Ʈ���������� 0�̸� ������ 0�� �°� �������� �ڵ带 ä�ο��� �����ص״�)
			m_isFinished = false; // �ִϸ��̼��� ���� ������ ���� �ʾҴٰ� �˷��ش�.
		}
	}

	/* �� �ִϸ��̼��� �̿��ϴ� ��ü ���� ���¸� m_fTrackPosition �ð��� �´� ���·� �����ϴ�.*/
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_Channels[i]->Invalidate_TransformationMatrix(m_fTrackPosition, Bones, &m_CurrentKeyFrames[i]); // �������� ���� ��������.
		//													������ �� ���� Ű�������� ä�ο� �ִ� �Լ��� �Ѱ�����.
	}
}

CAnimation* CAnimation::Create(const aiAnimation* pAIAnimation, const CModel::BONES& Bones) // ������ �� ���� �޾ƿ���
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAIAnimation, Bones))) // ���� ���� �̴ϼȶ���� �Ѱ�����
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this); // �ǵ帮�°� ���� �׳� ������� ������.
}

void CAnimation::Free()
{
	// ä���� ����ִ� �ִϸ��̼��� ��� ��ȸ�ϸ� ����������.
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}