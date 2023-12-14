#include "AnimationData.h"
#include "Channel.h"

// �ִϸ��̼� �����
HRESULT ANIMATION_DATA::Make_AnimationData(aiAnimation* In_pAiAnimation, _float In_fSpeed)
{
	// �ִϸ��̼� �̸�
	szName = In_pAiAnimation->mName.data;

	// ä�� ����
	iNumChannels = In_pAiAnimation->mNumChannels;

	// ��ü ����
	fDuration = (_float)In_pAiAnimation->mDuration;
	// Tick �ӵ�
	fTickPerSecond = (_float)In_pAiAnimation->mTicksPerSecond * In_fSpeed;

	for (_uint i(0); i < iNumChannels; i++)
	{
		CHANNEL_DATA* pChannelData = new CHANNEL_DATA(); // ����Ʈ �����͸� ���� �Ҵ����� ��ü.
		pChannelData->Make_ChannelData(In_pAiAnimation->mChannels[i]);	// Make_ChannelData()�� ȣ���Ͽ� ChannelData ����
		//Safe_AddRef(pChannelData); // ���ο�(�����Ҵ�) CHANNEL_DATA ��ü�� ������ �� ���� ����� 1�� �ʱ�ȭ ��Ű��.
		Channel_Datas.push_back(pChannelData);
	}

	return S_OK;
}

// ���̳ʸ� �����ϱ�
void ANIMATION_DATA::Bake_Binary(ofstream& os)
{
	write_typed_data(os, szName.size()); // �����͸� ���� �������� ���Ͽ� �����Ѵ�.
	os.write(&szName[0], szName.size());

	write_typed_data(os, iNumChannels);
	write_typed_data(os, fDuration);
	write_typed_data(os, fTickPerSecond);

	for (_uint i = 0; i < iNumChannels; i++)
	{
		Channel_Datas[i]->Bake_Binary(os); // �迭�� �ִ� �� ä�ε��� ��ȸ�ϸ� �ڱ� �ڽ��� Bake_Binary()�Լ��� ȣ���Ͽ� �� ä���� �����͸� ���Ͽ� �����Ѵ�.
	}
}

// ���̳ʸ� �ҷ�����
void ANIMATION_DATA::Load_FromBinary(ifstream& is)
{
	size_t istringSize;
	read_typed_data(is, istringSize); // ������ �б�
	szName.resize(istringSize);		  // ���� ������ �����ŭ ũ�� ���
	is.read(&szName[0], istringSize); // 0��° �迭�� ifstream���� �о��ֱ�.

	// �޾ƿ� ifstream ���� ������ �б� 
	read_typed_data(is, iNumChannels);	// ä�� ����
	read_typed_data(is, fDuration);		// �� ����
	read_typed_data(is, fTickPerSecond);// Tick�ӵ�

	// TODO : !!! DO NOT ERASE !!!
	// Temporary Macro for changing animation speed.
	// undefined after use.
#define SET_ANIM_SPEED(AnimationName, WantedSpeed)\
    if (!strcmp(szName.c_str(), AnimationName))\
    {\
        fTickPerSecond *= WantedSpeed;\
    }

	// Ư�� �ִϸ��̼� �ӵ��� ���� �� �� �ִ� �� �ϴ�.
	//SET_ANIM_SPEED("Joker_TakeExecution_Start", 1.36f)
	//SET_ANIM_SPEED("BossBat_TakeExecution_Start01", 1.006f);

#undef SET_ANIM_SPEED

	for (_uint i(0); i < iNumChannels; i++)
	{
		CHANNEL_DATA* pChannelData = new CHANNEL_DATA(); // ����Ʈ �����͸� ���� �Ҵ����� ��ü.
		pChannelData->Load_FromBinary(is);
		Channel_Datas.push_back(pChannelData);
	}
}

// �ִϸ��̼� �����
void ANIMATION_DATA::Bake_ReverseAnimation(shared_ptr<ANIMATION_DATA>& Out_AnimationData)
{
	Out_AnimationData->szName = szName + "_Reverse";

	Out_AnimationData->iNumChannels = iNumChannels;

	Out_AnimationData->fDuration = fDuration;
	Out_AnimationData->fTickPerSecond = fTickPerSecond;

	for (_uint i = 0; i < iNumChannels; i++)
	{
		CHANNEL_DATA* pChannelData = new CHANNEL_DATA(); // ����Ʈ �����͸� ���� �Ҵ����� ��ü.
		Channel_Datas[i]->Bake_ReverseChannel(*pChannelData, fDuration);
		Out_AnimationData->Channel_Datas.push_back(pChannelData);
	}
}
