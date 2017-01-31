/***************************************************************\
*
* Implement Fabonacci by Matrix method.

这还不是最快的方法。下面介绍一种时间复杂度是O(logn)的方法。在介绍这种方法之前，先介绍一个数学
公式：
{f(n), f(n-1), f(n-1), f(n-2)} ={1, 1, 1,0}n-1
(注：{f(n+1), f(n), f(n), f(n-1)}表示一个矩阵。在矩阵中第一行第一列是f(n+1)，第一行第二列是f(n)，第二
行第一列是f(n)，第二行第二列是f(n-1)。)
有了这个公式，要求得f(n)，我们只需要求得矩阵{1, 1, 1,0}的n-1 次方，因为矩阵{1, 1, 1,0}的n-1 次方的
结果的第一行第一列就是f(n)。这个数学公式用数学归纳法不难证明。感兴趣的朋友不妨自己证明一下。
现在的问题转换为求矩阵{1, 1, 1, 0}的乘方。如果简单第从0 开始循环，n 次方将需要n 次运算，并不比前
面的方法要快。但我们可以考虑乘方的如下性质：

    / an/2 *an/2 n 为偶数时
an=
    \ a(n-1)/2*a(n-1)/2 n 为奇数时

要求得n 次方，我们先求得n/2 次方，再把n/2 的结果平方一下。如果把求n 次方的问题看成一个大问题，
把求n/2 看成一个较小的问题。这种把大问题分解成一个或多个小问题的思路我们称之为分治法。这样求n
次方就只需要logn 次运算了。

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
