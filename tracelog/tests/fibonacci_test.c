#include <stdio.h>
#include "fibonacci.h"
#include "fibonacci_test.h"
#include "trace_log.h"

void TestFibonacciSeq()
{
    struct Matrix2By2 m;
    long long result;
	long long n;
	for( n = 3; n < 10; n++)
	{
		m = fibonacci_seq_quick( n-1 );
		result = m.m00;
		printf("The fibonacci number of %llu = %llu\n", n, result );
	}
    TRACE_LOG(&m, sizeof(struct Matrix2By2), Matrix2By2, m00, "print Matrix2By2");
}
