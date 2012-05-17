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
		s, TO_NETWORK_16(s), TO_LOCAL_16(s));

	printf("TEST LONG=%ld BIGENDIAN=%d LITTLEENDIAN=%d\r\n",
		l, TO_NETWORK_32(l), TO_LOCAL_32(l));

	printf("TEST LONGLONG=%lld BIGENDIAN=%lld LITTLEENDIAN=%lld\r\n",
		ll, TO_NETWORK_64(ll), TO_LOCAL_64(ll));

	system("Pause");

	return 0;
}
