#pragma once

#include "Base.h"

BEGIN(Engine)
struct ENGINE_DLL CHANNEL_DATA
{
	string                              szNodeName;
	_uint                               iNumScalingKeys;
	_uint                               iNumRotationKeys;
	_uint                               iNumPositionKeys;
	_uint                               iNumKeyframes;

	vector<KEYFRAME>                    tKeyFrames;
	//vector<KEY_DATA>                    tScalingKeys;
	//vector<ROTATIONKEY_DATA>            tRotationKeys;
	//vector<KEY_DATA>                    tPositionKeys;

	HRESULT Make_ChannelData(aiNodeAnim* In_pChannel);

public:
	void Bake_Binary(ofstream& os);
	void Load_FromBinary(ifstream& is);
	void Bake_ReverseChannel(CHANNEL_DATA& Out_ChannelData, const _float In_fDuration);
};
END