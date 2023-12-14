#ifndef Engine_Macro_h__
#define Engine_Macro_h__

// color 매크로
#define D3DCOLOR_ARGB(a,r,g,b) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((b)&0xff)<<16)|(((g)&0xff)<<8)|((r)&0xff)))

// enum 매크로
#define IDX(ENUM)				static_cast<unsigned int>(ENUM)

#ifndef			MSG_BOX
#define			MSG_BOX(_message)		MessageBox(nullptr, TEXT(_message), L"System Message", MB_OK)
#endif

#ifndef			MSG_BOX_INPUT
#define			MSG_BOX_INPUT(_INPUT)		MessageBox(nullptr, _INPUT.c_str(), L"System Message", MB_OK)
#endif

#define			BEGIN(NAMESPACE)		namespace NAMESPACE {
#define			END						}

#define			USING(NAMESPACE)	using namespace NAMESPACE;

#ifdef	ENGINE_EXPORTS
#define ENGINE_DLL		_declspec(dllexport)
#else
#define ENGINE_DLL		_declspec(dllimport)
#endif


/*  Debug Break */
#ifdef _DEBUG
#define DEBUG_BREAK  DebugBreak()
#else
#define DEBUG_BREAK
#endif

/* ================== NULL CHECK ================== */
#define NULL_CHECK( _ptr)   \
   {if( (_ptr) == 0){ DEBUG_BREAK;return;}}

#define NULL_CHECK_RETURN( _ptr, _return)   \
   {if( 0 == (_ptr) ){ DEBUG_BREAK;return _return;}}

#define NULL_CHECK_MSG( _ptr, _message )      \
   {if( (_ptr) == 0){MessageBox(NULL, _message, L"System Message",MB_OK); DEBUG_BREAK;}}

#define NULL_CHECK_RETURN_MSG( _ptr, _return, _message )   \
   {if( (_ptr) == 0){MessageBox(NULL, _message, L"System Message",MB_OK); DEBUG_BREAK;return _return;}}

/* ================== FAILED CHECK8 ================== */
#define FAILED_CHECK(_hr)   if( ((HRESULT)(_hr)) < 0 )   \
   { MessageBoxW(NULL, L"Failed", L"System Error",MB_OK); DEBUG_BREAK; return E_FAIL;}

#define FAILED_CHECK_RETURN(_hr, _return)   if( ((HRESULT)(_hr)) < 0 )      \
   { MessageBoxW(NULL, L"Failed", L"System Error",MB_OK); DEBUG_BREAK; return _return;}

#define FAILED_CHECK_MSG( _hr, _message)   if( ((HRESULT)(_hr)) < 0 )   \
   { MessageBoxW(NULL, _message, L"System Message",MB_OK); DEBUG_BREAK;}

#define FAILED_CHECK_RETURN_MSG( _hr, _return, _message)   if( ((HRESULT)(_hr)) < 0 )   \
   { MessageBoxW(NULL, _message, L"System Message",MB_OK); DEBUG_BREAK;return _return;}

/* ================== COMPARE CHECK ================== */
#define COMPARE_SAME_RETURN(_check1, _check2, _return)\
   {if ((_check1) == (_check2)) { DEBUG_BREAK; return _return; }}

#define COMPARE_DIFF_RETURN(_check1, _check2, _return)\
   {if ((_check1) != (_check2)) { DEBUG_BREAK; return _return; }}

/* ================== FALSE CHECK ================== */
#define FALSE_CHECK( _false)   \
   NULL_CHECK( _false)

#define FALSE_CHECK_RETURN( _false, _return)   \
   NULL_CHECK_RETURN( _false, _return)

#define FALSE_CHECK_MSG( _false, _message )   \
   NULL_CHECK_MSG( _false, _message )

#define FALSE_CHECK_RETURN_MSG( _false, _return, _message )   \
   NULL_CHECK_RETURN_MSG( _false, _return, _message )



#define NO_COPY(CLASSNAME)								\
		private:										\
		CLASSNAME(const CLASSNAME&);					\
		CLASSNAME& operator = (const CLASSNAME&);		

#define DECLARE_SINGLETON(CLASSNAME)					\
		NO_COPY(CLASSNAME)								\
		private:										\
		static CLASSNAME*	m_pInstance;				\
		public:											\
		static CLASSNAME*	GetInstance( void );		\
		static unsigned long DestroyInstance( void );			

#define IMPLEMENT_SINGLETON(CLASSNAME)							\
		CLASSNAME*	CLASSNAME::m_pInstance = nullptr;			\
		CLASSNAME*	CLASSNAME::GetInstance( void )	{			\
			if(NULL == m_pInstance) {							\
				m_pInstance = new CLASSNAME;					\
			}													\
			return m_pInstance;									\
		}														\
		unsigned long CLASSNAME::DestroyInstance( void ) {		\
			unsigned long dwRefCnt = 0;							\
			if(nullptr != m_pInstance)							\
			{													\
				dwRefCnt = m_pInstance->Release();				\
				if(0 == dwRefCnt)								\
					m_pInstance = nullptr;						\
			}													\
			return dwRefCnt;									\
		}


#endif // Engine_Macro_h__
