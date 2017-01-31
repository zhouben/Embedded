/***************************************************************\
*
* Implement Fabonacci by Matrix method.

�⻹�������ķ������������һ��ʱ�临�Ӷ���O(logn)�ķ������ڽ������ַ���֮ǰ���Ƚ���һ����ѧ
��ʽ��
{f(n), f(n-1), f(n-1), f(n-2)} ={1, 1, 1,0}n-1
(ע��{f(n+1), f(n), f(n), f(n-1)}��ʾһ�������ھ����е�һ�е�һ����f(n+1)����һ�еڶ�����f(n)���ڶ�
�е�һ����f(n)���ڶ��еڶ�����f(n-1)��)
���������ʽ��Ҫ���f(n)������ֻ��Ҫ��þ���{1, 1, 1,0}��n-1 �η�����Ϊ����{1, 1, 1,0}��n-1 �η���
����ĵ�һ�е�һ�о���f(n)�������ѧ��ʽ����ѧ���ɷ�����֤��������Ȥ�����Ѳ����Լ�֤��һ�¡�
���ڵ�����ת��Ϊ�����{1, 1, 1, 0}�ĳ˷�������򵥵ڴ�0 ��ʼѭ����n �η�����Ҫn �����㣬������ǰ
��ķ���Ҫ�졣�����ǿ��Կ��ǳ˷����������ʣ�

    / an/2 *an/2 n Ϊż��ʱ
an=
    \ a(n-1)/2*a(n-1)/2 n Ϊ����ʱ

Ҫ���n �η������������n/2 �η����ٰ�n/2 �Ľ��ƽ��һ�¡��������n �η������⿴��һ�������⣬
����n/2 ����һ����С�����⡣���ְѴ�����ֽ��һ������С�����˼·���ǳ�֮Ϊ���η���������n
�η���ֻ��Ҫlogn �������ˡ�

\***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "fibonacci.h"

void matrix_multiply( struct Matrix2By2 *M0, struct Matrix2By2 *M1, struct Matrix2By2 *Result )
{
	Result->m00 = M0->m00 * M1->m00 + M0->m01 * M1->m10;
	Result->m01 = M0->m00 * M1->m01 + M0->m01 * M1->m11;
	Result->m10 = M0->m10 * M1->m00 + M0->m11 * M1->m10;
	Result->m11 = M0->m10 * M1->m01 + M0->m11 * M1->m11;

}
struct Matrix2By2 atom = { 1, 1, 1, 0 };

struct Matrix2By2 fibonacci_seq_quick( long long N )
{
    struct Matrix2By2 m0;
    struct Matrix2By2 m1;
    struct Matrix2By2 result;
    if ( N == 1 )
    {
		return atom;
    }else{
        if ( ( N & 1 ) == 1 )
        {
            m0 = fibonacci_seq_quick( ( N - 1 ) / 2 );
            m1 = fibonacci_seq_quick( ( N + 1 ) / 2 );
        }else{
            m0 = fibonacci_seq_quick( N / 2 );
            m1 = m0;
        }
        matrix_multiply( &m0, &m1, &result );
        return result;
    }

}

char* fibonacci_get_file_name()
{
    return __BASE_FILE__;
}
