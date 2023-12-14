#pragma once

#include "Base.h"
#include "Model.h"

/* �� �ִϸ��̼ǿ��� ����ϴ� ���� ������. */
/* �ð��뿡 ���� ���� ����(Scale, Rotation, Position)�� �����Ѵ�.*/

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
	//_uint				m_iCurrentKeyFrameIndex = { 0 };	// ���� Ű�������� ����ִ��� �˱����� �ε���
	// m_iCurrentKeyFrameIndex�� Ŭ�а��� ������ �Ǿ� ������ �Ǵ�, ��(�ִϸ��̼�)���� vector �����̳ʷ� �ް� ������.
	// m_iCurrentKeyFrameIndex �ϳ������� ä�� Ŭ������ ���� �����ϰ� �Ǵ°� ���� ��ȿ�� ���̱� �����̴�.

	_uint				m_iBoneIndex = { 0 }; // ���° ������ �������� ���ϱ� ���� �ε���

public:
	static CChannel* Create(const aiNodeAnim* pChannel, const CModel::BONES& Bones);
	virtual void Free() override;
};

END