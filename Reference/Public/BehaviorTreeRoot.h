#pragma once

#include "Base.h"

BEGIN(Engine)
class CBTRoot
{
private:
	CBTRoot();
	virtual ~CBTRoot() = default;

public:
	HRESULT		Initialize();
	HRESULT		Add_Node();

public:
	static CBTRoot*	Create();

private:
	vector<class CBTNode*>    m_Nodes;
};
END

