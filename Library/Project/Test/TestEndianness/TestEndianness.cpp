// TestEndianness.cpp : Defines the entry point for the console application.
//

#include <stdlib.h>
#include "stdafx.h"
#include "..\\..\\..\\Include\\Endianness.h"


int _tmain(int argc, _TCHAR* argv[])
{
	short s = 1234;
	long  l = 12345678;
	long long ll = 123456789012345;

	printf("TEST SHORT=%d BIGENDIAN=%d LITTLEENDIAN=%d\r\n",
		s, BIG_ENDIAN16(s), LITTLE_ENDIAN16(s));

	printf("TEST LONG=%ld BIGENDIAN=%d LITTLEENDIAN=%d\r\n",
		l, BIG_ENDIAN32(l), LITTLE_ENDIAN32(l));

	printf("TEST LONGLONG=%lld BIGENDIAN=%lld LITTLEENDIAN=%lld\r\n",
		ll, BIG_ENDIAN64(ll), LITTLE_ENDIAN64(ll));

	system("Pause");

	return 0;
}
