#include "..\Public\Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

// 어떤 동작들을 취해야하는지 담겨있는 키프레임을 저장해두자.
// 키프레임을 통해 애니메이션이 동작할것이고, 자연스럽게 동작하기 위해 키프레임과 포지션을 이용해 선형보간을 계산할것이다.
// 계산된 보간값은 행열로 조합하여 완성하고
// 완성된 행열은 실제 모델의 뼈에 Transform을 찾아 곱하여 지금 취해야하는 동작을 짓게 만들것이다.
HRESULT CChannel::Initialize(const aiNodeAnim* pChannel, const CModel::BONES& Bones)
{
	// 나중에 디버깅하기 쉽게 이름을 꺼내놓자.
	strcpy_s(m_szName, pChannel->mNodeName.data);

	/* 받아온 뼈 정보로 뼈 인덱스를 골라주자. */
	_uint		iBoneIndex = { 0 };

	auto	iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)
		{
			if (false == strcmp(m_szName, pBone->Get_Name()))
			{
				return true;
			}

			++iBoneIndex;

			return false;
		});

	if (iter == Bones.end())
		return E_FAIL;

	m_iBoneIndex = iBoneIndex; // 고른 뼈 인덱스를 뼈 인덱스 멤버 변수에 저장해두자.

	// 키프레임 개수이다.
	m_iNumKeyFrames = max(pChannel->mNumScalingKeys, pChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(pChannel->mNumPositionKeys, m_iNumKeyFrames);

	_float3		vScale;		// 크기
	_float4		vRotation;	// 회전(자전)
	_float3		vPosition;	// 이동

	for (size_t i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME			KeyFrame = {};

		if (i < pChannel->mNumScalingKeys)
		{
			memcpy(&vScale, &pChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = pChannel->mScalingKeys[i].mTime;
		}
		if (i < pChannel->mNumRotationKeys)
		{
			// memcpy(&vRotation, &pChannel->mRotationKeys[i].mValue, sizeof(_float4));
			vRotation.x = pChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pChannel->mRotationKeys[i].mValue.w;
			KeyFrame.fTrackPosition = pChannel->mRotationKeys[i].mTime;
		}
		if (i < pChannel->mNumPositionKeys)
		{
			memcpy(&vPosition, &pChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTrackPosition = pChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vPosition = vPosition;

		m_KeyFrames.push_back(KeyFrame); // 만든 키프레임을 키프레임 벡터 컨테이너에 저장하자.
	}

	return S_OK;
}

// 애니메이션 선형 보간 (매번 호출하여 현재 애니메이션이 어떤 동작을 취해야하는지 계산한다)
void CChannel::Invalidate_TransformationMatrix(_float fCurrentTrackPosition, const CModel::BONES& Bones, _uint* pCurrentKeyFrameIndex)
{ // 채널을 생성할때 이름을 가진 녀석을 받아와주자.

	// 현재 트렉포지션이 0일 경우 (애니메이션을 반복하기 위해, 혹은 변경해서 처음으로 돌렸을 경우)
	if (0.0f == fCurrentTrackPosition)
		*pCurrentKeyFrameIndex = 0; // 키프레임(동작)을 0으로 맞춰준다.

	_vector		vScale;
	_vector		vRotation;
	_vector		vPosition;

	KEYFRAME	LastKeyFrame = m_KeyFrames.back(); // 마지막번째 키프레임을 받자

	/* 만약 현재 애니메이션 재생이 마지막번째일 경우 마지막 동작으로 유지 */
	if (fCurrentTrackPosition >= LastKeyFrame.fTrackPosition)
	{
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vPosition = XMLoadFloat3(&LastKeyFrame.vPosition);
	}
	/* 현재 애니메이션 재생이 마지막 애니메이션보다 낮을 경우 선형보간을 통해 값을 만들어준다. */
	else
	{
		// if를 while로 바꾼 이유
		// 프레임이 떨어졌을 때 시간값으로 선형보간을 계산하기 때문에 시간값이 크게 들어오게되어, 키프레임을 많이 건너뛰는 현상이 발생한다.
		// 이 문제로 메쉬가 찢어지는 현상이 벌어지는데
		// 이를 방지하기 위해 선형보간에 들어왔을 때 while을 통해 시간값이 크게 들어와 키프레임을 많이 건너뛰어도 건너뛴곳까지 루프를돌아 빠져나오기 때문에 알맞게 재생된다.
		while (fCurrentTrackPosition >= m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition)
				++*pCurrentKeyFrameIndex;

		_float3		vSourScale, vDestScale;
		_float4		vSourRotation, vDestRotation;
		_float3		vSourPosition, vDestPosition;

		vSourScale = m_KeyFrames[*pCurrentKeyFrameIndex].vScale;
		vSourRotation = m_KeyFrames[*pCurrentKeyFrameIndex].vRotation;
		vSourPosition = m_KeyFrames[*pCurrentKeyFrameIndex].vPosition;

		vDestScale = m_KeyFrames[*pCurrentKeyFrameIndex + 1].vScale;
		vDestRotation = m_KeyFrames[*pCurrentKeyFrameIndex + 1].vRotation;
		vDestPosition = m_KeyFrames[*pCurrentKeyFrameIndex + 1].vPosition;

		// Ratio : 선형보간을 해주기 위한 비율을 계산한 녀석.
		// 현재 포지션의 시작 애니메이션, 현재 재생위치, 다음 애니메이션을 계산하여 보간 값의 비율을 계산해준다.
		// 이렇게 계산된 Ratio값을 각각 크기, 회전, 이동에 곱해주면 선형 보간 값이 완성된다.
		_float		fRatio = (fCurrentTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition) /
			(m_KeyFrames[*pCurrentKeyFrameIndex + 1].fTrackPosition - m_KeyFrames[*pCurrentKeyFrameIndex].fTrackPosition);

		// 소스, 데스트, 라티오를 조합해 최종적인 값들을 완성 시켜준다.
		vScale = XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio);
		vPosition = XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio);
		/* 선형 보간 계산을 통해 구해진 녀석들을 각각의 벡터에 담아준다.
		   단, Rotation은 쿼터니언이기 때문에 쿼터니언을 구해주는 함수를 사용한다.*/
	}

	// 진짜 완성된 마지막 값들 3가지를 하나로 조합해 행열로 만들어주는 함수이다.
	_matrix		TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	// 이렇게 만들어진 행열을 이 채널과 같은 이름을 가진 뼈를 찾아서 TransformationMatrix를 곱하여, 현재 시간대에 움직임을 완성 시켜주자.
	// # 그냥 지금 어떤 동작해야하는지 계산했으니, 이제 곱해서 적용해주겠다는 것 #
	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

CChannel* CChannel::Create(const aiNodeAnim* pChannel, const CModel::BONES& Bones)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pChannel, Bones)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChannel::Free()
{
}