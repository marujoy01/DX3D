/* 함수 
pow : 제곱 함수
min : 최소값 함수
max : 최대값 함수
*/


matrix			g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D		g_Texture;

// 정광원
vector			g_vLightPos = vector(50.f, 10.f, 50.f, 1.f);	// 위치 (조명을 놓고싶은 곳)
float			g_fLightRange = 30.f;							// 범위 (조명을 비추고싶은 범위)

/* 빛 연산 변수 */
vector			g_vLightDir = vector(1.f, -1.f, 1.f, 0.f);		// 빛의 방향 : 객체의 법선 벡터와 연산하여 빛을 받게한다.	
vector			g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);	// 난반사	 : 현실의 태양과 같은 느낌의 난반사이다.

vector			g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);	// 환경반사	 : 현실처럼 직접적인 빛을 받지않아도 어느정도 빛을 받게하기 위한녀석.
vector			g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);	// 반사광	 : 광택이 있는 표면에 빛이 직접 입사될 때 생긴 직접반사를 보여 준다.

// 객체의 정보
vector			g_vMtrlAmbient = vector(0.3f, 0.3f, 0.3f, 1.f);	// Ambient도 빛과 객체에게 각각 존재한다.
vector			g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);
//texture2D		g_DiffuseTexture;	// Diffuse도 빛과 객체에게 각각 존재한다.

// 지형 섞기
texture2D		g_DiffuseTexture[2];
texture2D		g_MaskTexture;

// 브러쉬
texture2D		g_BrushTexture;	// 텍스처
vector			g_vBrushPos;//= vector(30.f, 0.f, 30.f, 1.f);	// 브러쉬를 그릴 위치
float			g_fBrushRange = 1.f;	// 브러쉬의 범위

// 카메라
vector			g_vCamPosition;

sampler DefaultSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
	//Filter = MIN_MAG_MIP_POINT;
	AddressU = wrap;
	AddressV = wrap;
};

//TODO 셰이더가 하는 일
//! 정점의 변환 ( 월드변환, 뷰변환, 투영변환 ) 을 수행한다. ( 뷰행렬의 투영행렬을 곱했다고 투영 스페이스에 있는 것이아니다. 반드시 w나누기 까지 거친 다음에야 투영 스페이스 변환이 됐다고 할 수 있다. )
//! 정점의 구성정보를 추가, 삭제 등의 변경을 수행한다.

struct VS_IN
{
	float3		vPosition : POSITION; //! 위치값을 나타내는 시맨틱, 아직 연산을 해야하는 자료형임을 알림.
	float3		vNormal	  : NORMAL;
	float2		vTexcoord : TEXCOORD0; //! 텍스처의 좌표값을 나타내는 시맨틱, UV 좌표계는 0 ~ 1사이의 값을 가지는 데, 특정 위치에 색상값을 알기위함?
};

struct VS_OUT // OUT과 IN을 맞춰주기 위해 서로 똑같이 추가되어야함
{
	float4		vPosition : SV_POSITION; //! 리턴해주기위한 위치. 렌더링파이프라인을 수행 후 w값이 있는채여야해서 float4로 바뀌었고, SV_POSITION 시맨틱을 줘서 연산이 끝난 자료형이란 것을 셰이더에게 알림
	float4		vNormal	  : NORMAL;
	float2		vTexcoord : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1; // 월드 위치
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0; //! HLSL에서만 사용가능한 구조체 제로메모리
	
	//! 매개인자로 들어온 In은 로컬 영역이다
	//! In.vPosition * 월드 * 뷰 * 투영을 해줘야한다.
	
	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	//TODO mul 함수는 행렬곱이 가능한 모든 행렬들의 곱셈을 수행해준다. 

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);		//! 클라에서 던져준 정점의 위치정보를 행렬곱이 가능하게 행을 맞춰주고 w값이니 1로 초기화 시켜준 행렬과 월드,뷰,투영 곱을 마친 행렬을 곱해준다.
	Out.vNormal = mul(float4(In.vNormal, 0.f), g_WorldMatrix);  /* Out.vNomal은 로컬스페이스의 노멀이다. 빛의 방향벡터는 월드스페이스 이기때문에 연산을 위해 로컬의 노멀을 행렬곱셈을 통해 월드로 올려준다. */
	Out.vTexcoord = In.vTexcoord; //! 텍스쿠드는 변하지 않는다는 것을 알 수 있다.
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix); // + (스펙큘러, 정광원 수업)월드 위치 추가

	return Out;
}

//TODO 순서
//! 정점을 그리는 건 결국 인덱스로 그릴거다.
//! 사각형을 예시로 들자면  정점 4개, 인덱스 6개를 가지고 있다. 정점은 { 0,1,2,3 } 인덱스는 { {0,1,2}, {0,2,3} }을 가지고 있다.
//! 0번째 정점을 꺼내서 VS_IN으로 전달
//! 통과된 정점을 대기
//! 1번째 정점을 꺼내서 VS_IN으로 전달
//! 통과된 정점을 대기
//! 2번째 정점을 꺼내서 VS_IN으로 전달
//! 통과된 정점을 대기
//! 이제 통과된 정점의 개수가 3개가 되었으니 그리는 작업을 시작한다 ( Why? 밖에서 트라이앵글리스트로 그린다고 했으니까 ( 삼각형으로 그린다 ) )
//! 정점 3개에 대해서 w나누기 변환, 뷰포트 변환, 이제 정점 세 개의 안을 막 채운다는 작업흐름이 이어짐.
//! 정점셰이더는 6번 통과
//! 픽셸셰이더는 가로 사이즈 * 세로 사이즈 수행 800 * 600 일 때 = 480,000 번 호출

struct PS_IN // OUT과 IN을 맞춰주기 위해 서로 똑같이 추가되어야함
{
	float4		vPosition : SV_POSITION; //! 왜 PS_IN인데 연산이 끝낫다는 시맨틱으로? 래스터라이즈 과정을 거쳤다고 하면 당연히 픽셀화가 된 이후이니 연산이 끝났다.
	float4		vNormal	  : NORMAL;	// 빛 연산을 위한 노멀 추가
	float2		vTexcoord :	TEXCOORD0;
	float4		vWorldPos : TEXCOORD1; // 월드 위치
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0; //! 다렉9에서는 COLOR라고 했었으나 11에서 바뀌었다. 훨씬 가독성이 좋아졌다고 할 수 있다. 왜? 몇번째 렌더타겟에 그릴꺼냐 라는 직관적이니까
};

//TODO 픽셀 셰이더 : 픽셀의 색!!!!!을 결정한다.
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	
	///* 첫번째 인자의 방식으로 두번째 인자의 위치에 있는 픽셀의 색을 얻어온다. */
	//vector		vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord * 100.0f);

	/* 첫번째 인자의 방식으로 두번째 인자의 위치에 있는 픽셀의 색을 얻어온다. */
	vector		vSourDiffuse = g_DiffuseTexture[0].Sample(DefaultSampler, In.vTexcoord * 100.0f);
	vector		vDestDiffuse = g_DiffuseTexture[1].Sample(DefaultSampler, In.vTexcoord * 100.0f);

	vector		vMask = g_MaskTexture.Sample(DefaultSampler, In.vTexcoord);

	// 브러쉬를 지형과 섞어 그려보자.
	vector		vBrush = vector(0.f, 0.f, 0.f, 0.f);	// 임시 값으로 생성해보자.

	// vector		vBrush = g_BrushTexture.Sample(DefaultSampler, In.vTexcoord); // 위 Diffuse들과 똑같이 해주면된다. (끝에 100을 곱해주면 타일처럼 여러장이 깔린다.)

	// 브러쉬를 어디에 어떤 크기로 생성할건지 조건을 통해 만들어주자.
	if (g_vBrushPos.x - g_fBrushRange < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fBrushRange &&
		g_vBrushPos.z - g_fBrushRange < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fBrushRange)
	{
		float2		vUV; // UV를 새로 잡아줘야한다.

		/*	UV를 새로 만들어주는 이유
			기존 UV로 해버리면 전체 텍스처의 일부 영역만 가져와 그리게된다. (ex : 얼굴중 일부인 눈만 그린다던지..)
			따라서 기존 UV는 그대로 놔두고 새 UV를 통해 그려주는 것이다. 
		*/
		// 계산식
		vUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushRange)) / (2.f * g_fBrushRange);	// 가로
		vUV.y = ((g_vBrushPos.z + g_fBrushRange) - In.vWorldPos.z) / (2.f * g_fBrushRange);	// 세로

		// 브러쉬 채워주기
		vBrush = g_BrushTexture.Sample(DefaultSampler, vUV);
	}

	/*	fShade = 빛 연산을 위해 필요한 요소들
		1. g_vLightDir : 위에서 미리 지정해둔 빛의 방향 
		2. * -1.f	   : 빛의 방향을 반대로 지정해, cos그래프와 동일하게 만들어주는 용도
		3. In.vNormal  : 빛을 받는 객체의 법선벡터
		4. nrmalize	   : 노멀라이즈
		5. dot		   : 위 과정을 모두 거친 법선벡터를 내적해주는 내적함수
		6. max		   : 음수가 나오면 바꿔주게되는 함수
	*/

	//																				위에서 채워준 브러쉬를 디퓨즈에 더해서 그려주기 (배경이 검정색이라 더해주면 알맞게 들어간다.)
	vector		vMtrlDiffuse = vMask * vDestDiffuse + (1.f - vMask) * vSourDiffuse + vBrush;

	//			 명암  = 내적(빛의 방향 * -1.f역벡터, 픽셀의 법선벡터)
	float		fShade = max(dot(normalize(g_vLightDir) * -1.f, normalize(In.vNormal)), 0.f);

	/* 스펙큘러가 보여져야하는 영역에서는 1로, 아닌 영역에서는 0으로 정의되는 스펙큘러의 세기가 필요하다. */
	//		시선 벡터 =   픽셀 위치  -   카메라 위치
	vector		vLook = In.vWorldPos - g_vCamPosition;	// 시선 벡터 (카메라)

	//			반사 벡터 = 빛의 방향과 픽셀 면적의 법선벡터를 넣어 반사벡터를 반환하는 함수
	vector		vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));	// 반사 벡터

	//			스펙큘러  = 제곱함수(최대값(내적(정규화(시선벡터) * -1.f역벡터, 정규화(반사 벡터)), 최대값), 제곱30);
	float		fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

	//			스펙큘러 조절 = 빛 범위 - 빛방향 / 빛 범위
	//float		fAtt = max(g_fLightRange - length(vLightDir), 0.f) / g_fLightRange;
	//float		fAtt = max(g_fLightRange - length(g_vLightDir), 0.f) / g_fLightRange;

	// 최종 색상
	Out.vColor = g_vLightDiffuse * vMtrlDiffuse * fShade * min((fShade + (g_vLightAmbient * g_vMtrlAmbient)), 1.f) // ;확인..
		+ (g_vLightSpecular * g_vMtrlSpecular) * fSpecular; // 명암, 스펙큘러 적용 안되던 이유 : VIBuffer_Terrain에서 노말을 안바꿨었음


	/*	 색상  =	빛의 난반사	 * 물체의 난반사* 최대값 지정(연산된 빛 + (빛의 환경반사 * 물체의 환경반사)), 최대값1); */
	//Out.vColor = g_vLightDiffuse * vMtrlDiffuse * min((fShade + (g_vLightAmbient * g_vMtrlAmbient)), 1.f);

	return Out;
}

// 정광원
PS_OUT PS_MAIN_POINT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	/* 첫번째 인자의 방식으로 두번째 인자의 위치에 있는 픽셀의 색을 얻어온다. */
	vector		vMtrlDiffuse = g_DiffuseTexture[0].Sample(DefaultSampler, In.vTexcoord * 100.0f);

	vector		vLightDir = In.vWorldPos - g_vLightPos;


	float		fAtt = max(g_fLightRange - length(vLightDir), 0.f) / g_fLightRange;

	float		fShade = max(dot(normalize(vLightDir) * -1.f, normalize(In.vNormal)), 0.f);

	/* 스펙큘러가 보여져야하는 영역에서는 1로, 아닌 영역에서는 0으로 정의되는 스펙큘러의 세기가 필요하다. */
	vector		vLook = In.vWorldPos - g_vCamPosition;
	vector		vReflect = reflect(normalize(vLightDir), normalize(In.vNormal));

	float		fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

	Out.vColor = (g_vLightDiffuse * vMtrlDiffuse * min((fShade + (g_vLightAmbient * g_vMtrlAmbient)), 1.f)
		+ (g_vLightSpecular * g_vMtrlSpecular) * fSpecular) * fAtt; // * fAtt를 해줌으로써 빛이 가까우면 1, 멀어지면 0으로 스펙큘러를 조절해준다.

	return Out;
}

//	   (요기)
technique11 DefaultTechnique //! 다렉9 이후로 테크니크뒤에 버전을 붙여줘야함. 우린 다렉11이니 11로 붙여줌
{
	/* 내가 원하는 특정 셰이더들을 그리는 모델에 적용한다. */
	pass Terrain	// 빛 연산 테스트용 터레인 추가
	{
		/* 렌더스테이츠 */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Delete // 정광원 (나중에 지울거라서 Delete로 지어뒀음)
	{
		/* 렌더스테이츠 */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		HullShader = NULL;
		DomainShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_POINT();
	}

	//pass UI
	//{
	//	//! 렌더스테이트가 올수도 있음.
	//	VertexShader = compile vs_5_0 VS_MAIN();
	//	GeometryShader = NULL;
	//	HullShader = NULL;
	//	DomainShader = NULL;
	//	PixelShader = compile ps_5_0 PS_MAIN();
	//}

	///* 위와 다른 형태에 내가 원하는 특정 셰이더들을 그리는 모델에 적용한다. */
	//pass Particle
	//{
	//	VertexShader = compile vs_5_0 VS_MAIN();
	//	PixelShader = compile ps_5_0 PS_MAIN();
	//}
};