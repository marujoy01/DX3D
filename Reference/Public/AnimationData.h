#pragma once

#include "Base.h"
#include "ChannelData.h"

BEGIN(Engine)
struct ENGINE_DLL ANIMATION_DATA final : public CBase
{
private:
	 int refCount;
	vector<class CHANNEL_DATA*> Channel_Datas;

public:
	ANIMATION_DATA() : refCount(0) {}

	void AddRef() 
	{
		refCount++;
	}

	void Release() 
	{
		refCount--;
		if (refCount == 0) 
		{
			for (CHANNEL_DATA* channelData : Channel_Datas) 
			{
				//channelData->Release();
			}
			delete this;
		}
	}

public:
	string szName;
	_uint  iNumChannels = 0;
	_float fDuration = 0.f;
	_float fTickPerSecond = 0.f;

	HRESULT Make_AnimationData(aiAnimation* In_pAiAnimation, _float In_fSpeed = 1.f);

public:
	void Bake_Binary(ofstream& os);
	void Load_FromBinary(ifstream& is);
	void Bake_ReverseAnimation(shared_ptr<ANIMATION_DATA>& Out_AnimationData);
};
END