#ifndef _FIBONACCI_H_
#define _FIBONACCI_H_
struct Matrix2By2
{
	long long m00;
	long long m01;
	long long m10;
	long long m11;
	//Matrix2By2 ( long long m00 = 0, long long m01 = 0, long long m10 = 0, long long m11 = 0 ): m_00 (m00), m_01(m01), m_10 (m10), m_11 (m11)	{	}
};

struct Matrix2By2 fibonacci_seq_quick( long long N );
char* fibonacci_get_file_name();
#endif
