#pragma once
#include "Base.h"

BEGIN(Engine)

struct ENGINE_DLL NODE_DATA
{
	string			                    szName;
	_uint                               iNumChildren;

	_float4x4                           TransformationMatrix;

	vector<shared_ptr<NODE_DATA>>       pChildren;

	HRESULT Make_NodeData(aiNode* In_pAINode);

public:
	void Bake_Binary(ofstream& os);
	void Load_FromBinary(ifstream& is);

	void Debug_NodeLog(ostream& os);

};

END