#include "AnimationData.h"
#include "Channel.h"

// 애니메이션 만들기
HRESULT ANIMATION_DATA::Make_AnimationData(aiAnimation* In_pAiAnimation, _float In_fSpeed)
{
	// 애니메이션 이름
	szName = In_pAiAnimation->mName.data;

	// 채널 개수
	iNumChannels = In_pAiAnimation->mNumChannels;

	// 전체 길이
	fDuration = (_float)In_pAiAnimation->mDuration;
	// Tick 속도
	fTickPerSecond = (_float)In_pAiAnimation->mTicksPerSecond * In_fSpeed;

	for (_uint i(0); i < iNumChannels; i++)
	{
		CHANNEL_DATA* pChannelData = new CHANNEL_DATA(); // 스마트 포인터를 동적 할당으로 대체.
		pChannelData->Make_ChannelData(In_pAiAnimation->mChannels[i]);	// Make_ChannelData()를 호출하여 ChannelData 생성
		//Safe_AddRef(pChannelData); // 새로운(동적할당) CHANNEL_DATA 객체를 생성할 때 참조 계수를 1로 초기화 시키자.
		Channel_Datas.push_back(pChannelData);
	}

	return S_OK;
}

// 바이너리 저장하기
void ANIMATION_DATA::Bake_Binary(ofstream& os)
{
	write_typed_data(os, szName.size()); // 데이터를 이진 형식으로 파일에 저장한다.
	os.write(&szName[0], szName.size());

	write_typed_data(os, iNumChannels);
	write_typed_data(os, fDuration);
	write_typed_data(os, fTickPerSecond);

	for (_uint i = 0; i < iNumChannels; i++)
	{
		Channel_Datas[i]->Bake_Binary(os); // 배열에 있는 각 채널들을 순회하며 자기 자신의 Bake_Binary()함수를 호출하여 각 채널의 데이터를 파일에 쓰게한다.
	}
}

// 바이너리 불러오기
void ANIMATION_DATA::Load_FromBinary(ifstream& is)
{
	size_t istringSize;
	read_typed_data(is, istringSize); // 데이터 읽기
	szName.resize(istringSize);		  // 읽은 데이터 사이즈만큼 크기 잡기
	is.read(&szName[0], istringSize); // 0번째 배열에 ifstream파일 읽어주기.

	// 받아온 ifstream 파일 정보들 읽기 
	read_typed_data(is, iNumChannels);	// 채널 개수
	read_typed_data(is, fDuration);		// 총 길이
	read_typed_data(is, fTickPerSecond);// Tick속도

	// TODO : !!! DO NOT ERASE !!!
	// Temporary Macro for changing animation speed.
	// undefined after use.
#define SET_ANIM_SPEED(AnimationName, WantedSpeed)\
    if (!strcmp(szName.c_str(), AnimationName))\
    {\
        fTickPerSecond *= WantedSpeed;\
    }

	// 특정 애니메이션 속도를 조절 할 수 있는 듯 하다.
	//SET_ANIM_SPEED("Joker_TakeExecution_Start", 1.36f)
	//SET_ANIM_SPEED("BossBat_TakeExecution_Start01", 1.006f);

#undef SET_ANIM_SPEED

	for (_uint i(0); i < iNumChannels; i++)
	{
		CHANNEL_DATA* pChannelData = new CHANNEL_DATA(); // 스마트 포인터를 동적 할당으로 대체.
		pChannelData->Load_FromBinary(is);
		Channel_Datas.push_back(pChannelData);
	}
}

// 애니메이션 역재생
void ANIMATION_DATA::Bake_ReverseAnimation(shared_ptr<ANIMATION_DATA>& Out_AnimationData)
{
	Out_AnimationData->szName = szName + "_Reverse";

	Out_AnimationData->iNumChannels = iNumChannels;

	Out_AnimationData->fDuration = fDuration;
	Out_AnimationData->fTickPerSecond = fTickPerSecond;

	for (_uint i = 0; i < iNumChannels; i++)
	{
		CHANNEL_DATA* pChannelData = new CHANNEL_DATA(); // 스마트 포인터를 동적 할당으로 대체.
		Channel_Datas[i]->Bake_ReverseChannel(*pChannelData, fDuration);
		Out_AnimationData->Channel_Datas.push_back(pChannelData);
	}
}
