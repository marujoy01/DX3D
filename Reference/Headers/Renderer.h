#pragma once
#include "Base.h"

//! 화면에 그려져야할 객체들을 그려야할 순서대로 모아놓는다.
//! 모아놓은 순서대로 객체들의 렌더함수를 호출한다. (렌더 콜)

BEGIN(Engine)

class CRenderer final : public CBase
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_NONLIGHT, RENDER_NONBLEND, RENDER_BLEND, RENDER_UI, RENDER_END };
	

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderGroup(RENDERGROUP eGroupID, class CGameObject* pGameObject);
	HRESULT Draw_RenderGroup();

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	list<class CGameObject*>	m_RenderObjects[RENDER_END]; //! 가장 아래에서 자료구조

private:
	HRESULT Render_Priority();
	HRESULT Render_NonLight();
	HRESULT Render_NonBlend();
	HRESULT Render_Blend();
	HRESULT Render_UI();

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END



//TODO enum RENDERGROUP { RENDER_PRIORITY, RENDER_NONLIGHT, RENDER_NONBLEND, RENDER_BLEND, RENDER_UI, RENDER_END };
//! 렌더그룹의 순서

//! 스카이박스를 먼저 그려야 하는 이유
/*
   큐브(스카이 박스)는 카메라에 위치로 따라다니면서 셋팅된다. 
   만약 스카이박스를 먼저 그리지 않았다면 깊이테스트에 의해서 스카이박스가 스카이박스의 거리보다 멀리 있는 플레이어 등등의 객체들을 전부 덮어버리는 현상이 일어난다.
   깊이 테스트는 이미 그려진 픽셀보다 깊이 값이 더 작을때만 그려지기 때문이다.

   해결점은 객체들이 스카이박스와 깊이 테스트를 수행하지 않는 상황을 만들어야 했다.
    그래서 스카이박스를 먼저 그리고 깊이버퍼에서 깊이를 기록하지 못하게만든 것. ( ZWRITE를 FALSE ).
*/

//! 논블렌드 그룹을 블렌드그룹보다 먼저 그려야 하는 이유
/*
	논블렌드 그룹은 불투명하게 그릴 객체들을 모아놓는 개념, 당연히 블렌드그룹은 반대되는 개념. 블렌드 그룹은 알파소팅을 수행한다.
	불투명하게 그릴 객체들을 먼저 그려서 블렌드그룹에있는 애들과 픽셀의색상값을 잘 섞어주게 만들어주기 위해서다.
*/

//! 알파블렌딩을 하는 그룹이 알파소팅을 수행하는 이유
/*
	알파블렌딩은 말 그대로 먼저 그려진 픽셀과 색상을 섞는다는 행위를 하는 것인데,
	여기서 먼저 그려진 픽셀의 개념을 주기위해서 알파소팅을 수행하는 것이다.
*/

//! UI 그룹을 마지막에 그려야 하는 이유
/*
	모든 UI는 다른 것에 덮여지면 안된다. 하지만 이 점은 이미 해결 되고 있었다.
	UI는 로컬스페이스 상에서 z값을 0으로 정해놓기 때문. 깊이 버퍼는 0 부터 1사이의 값을 가지고. 우리가 설정할 카메라의 Near 값을 0.2로 설정할 것이기 때문에
	다른 객체가 절대 UI를 덮어서 그릴 일은 없었다.

	그런데 마지막에 그려야하는 이유의 핵심은 모든 UI는 2D UI가 아닐수 있기 때문이다. 3차원 으로 그려지는 UI도 있을 수 있기 때문이다.
	3차원의 UI더라도 모든 걸 덮어서 그렸어야 했다. NPC에게 받을 수 있는 퀘스트가 있다는 표시를 해주는 UI가 있다고 했을 때의 갑자기 안개가쳐서 UI가 가려지는 상황은 있어선 안된다는 것.
	그래서 UI를 마지막에 그리면서 깊이비교를 안할게 라는 작업을 수행하는 것이다.
*/

//TODO 자료구조에 대한 이야기

//! 오브젝트 매니저에서 가지고있는 객체들의 주소를 가져와서 공유하는 컨테이너를 리스트로 선택한 이유
/*
	렌더러의 역할을 잘 생각해보자. 렌더러는 탐색할 일이 없고 오직 중간 삽입과 삭제만 빈번하게 일어난다.
	만약 벡터를 사용 했다면, 중간 삽입 삭제에서 좋지 않은 성능을 보이기도 하고, 미리 벡터의 카파시티를 지정 해놓고 사용해야 하는 경우가 생긴다.
	만약 지정 해놓고 사용한다면 엔진 프로젝트는 클라이언트 프로젝트에게 종속되는 관계가 되버린다. 그러면 카파시티를 9999로 줘버리면 되지 않겠느냐? 
	그럴 바에는 리스트 를 사용하는 게 더 낫지 않겠냐고 종결.
*/

//! 맵 컨테이너의 대한 이야기
/*
	맵 컨테이너는 이진트리 자료구조를 가지고 있다. ( 비선형 ) 레드블랙 알고리즘에 의해서 루트 노드 기준으로
	반으로 나눈 후, 양쪽의 개수가 비슷하게 만들어질 수 있도록 회전을 하며 불필요한 탐색을 최소화 하는 컨테이너이다.

	리스트로 선택한 이유에서 말했듯 렌더러는 탐색할일이 없다. 맵에서 삽입과 삭제를 빈번하게 일어나게 한다면 양쪽의 개수가 비슷하게 만들어지게 하는 것(정렬)을 자주 수행하는 것과 다름이 없다.
	그래서 결국 리스트로 사용한 것.

	그리고 오브젝트 매니저에서 사용한 맵 컨테이너 들을 살펴보면 맵 컨테이너에 저장할 데이터가 방대하지 않은 경우에만 사용하고 만약 방대하게 사용될 것 같다면 맵 컨테이너가 밸류 값으로 다른 컨테이너를 
	가지게 해서 탐색할 데이터가 많지 않게끔 만들어주는 것을 볼 수 있다.
*/