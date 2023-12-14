#pragma once

#include "Base.h"
#include "Model.h" // ��ǥ�� ��¼��.. �����ε尡 ��¼�� error : cpp�� �ƴ϶� ����� ����� ���Խ��Ѽ� �ذ�

/* Ư�� �ִϸ��̼�(���, �ȱ�, �ٱ�, ������, �±�) �� ǥ���ϱ����� �����͵��� ������. */

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
	_float				m_fDuration = { 0.0f }; /* �� �ִϸ��̼��� ��ü ����ϱ����� ��ü ����. */
	_float				m_fTickPerSecond = { 0.f }; /* �ִϸ��̼��� ��� �ӵ� : m_TickPerSecond * fTimeDelta */
	_float				m_fTrackPosition = { 0.f }; /* ���� ����ǰ� �ִ� ��ġ. */

	_uint				m_iNumChannels = { 0 }; /* �� �ִϸ��̼��� ����ϴ� ���� ����. */

	vector<class CChannel*>	m_Channels;
	vector<_uint>			m_CurrentKeyFrames; // ���� Ű�������� ����� ��ü��� ������ �Ǿ �ȵȴ�. ���� ���� �����̳ʿ� ä�� ������ �°� �־�����. (���� ���� �ð��� �ٸ� �ִϸ��̼��� ���� �� �־�� �ϱ� ����)

	_bool					m_isFinished = { false }; // �ִϸ��̼� ��� �Ϸ� ���� (��������)

public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, const CModel::BONES& Bones);
	CAnimation* Clone();
	virtual void Free() override;
};

END