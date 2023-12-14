#pragma once
#include "Base.h"

BEGIN(Engine)

struct ENGINE_DLL MATERIAL_DATA
{
	string                              szTextureName[AI_TEXTURE_TYPE_MAX];

	HRESULT Make_NodeData(aiMaterial* In_pMaterial);

public:
	void Bake_Binary(ofstream& os);
	void Load_FromBinary(ifstream& is);

};

END