#ifndef Engine_Typedef_h__
#define Engine_Typedef_h__

namespace Engine
{
	typedef		bool						_bool;

	typedef		signed char					_byte;
	typedef		unsigned char				_ubyte;

	typedef		char						_char;
	typedef		wchar_t						_tchar;
	
	typedef		signed short				_short;
	typedef		unsigned short				_ushort;

	typedef		signed int					_int;
	typedef		unsigned int				_uint;

	typedef		signed long					_long;
	typedef		unsigned long				_ulong;

	typedef		float						_float;
	typedef		double						_double;	

	//
	typedef XMINT2							_int2;

	typedef struct tagInt32
	{
		tagInt32(unsigned int _ix, unsigned int _iy, unsigned int _iz)
			: ix(_ix)
			, iy(_iy)
			, iz(_iz) {	}

		tagInt32()
			: ix(0)
			, iy(0)
			, iz(0) {	}

		unsigned int ix, iy, iz;
	} _uint3;

	typedef XMINT4						_uint4;

	/* 저장용 데이터 타입. */
	typedef		XMFLOAT4X4					_float4x4;

	/* 연산용 데이터 타입. */
	typedef		XMMATRIX					_matrix;
	typedef		FXMMATRIX					_fmatrix;
	typedef		CXMMATRIX					_cmatrix;

	/* 저장용 데이터 타입. */
	typedef		XMFLOAT4					_float4;
	typedef		XMFLOAT3					_float3;
	typedef		XMFLOAT2					_float2;

	/* 연산용 데이터 타입. */
	typedef		XMVECTOR					_vector;
	typedef		FXMVECTOR					_fvector;
	typedef		GXMVECTOR					_gvector;
	typedef		HXMVECTOR					_hvector;
	typedef		CXMVECTOR					_cvector;
	
}

#endif // Engine_Typedef_h__
