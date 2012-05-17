// TestEncrypt.cpp : Defines the entry point for the console application.
//
#include <stdlib.h>
#include "stdafx.h"
#include "IEncrypt.h"

int _tmain(int argc, _TCHAR* argv[])
{
	IEncrypt* pEncrypt = CreateEncrypt();
	if(NULL != pEncrypt)
	{
		pEncrypt->SetEncryptKey("1234567890123456", 16);
		char* inData = "1234567890";
		char  encDataAES[17] = {0};
		char  encDataRC6[17] = {0};
		char  decDataAES[17] = {0};
		char  decDataRC6[17] = {0};

		size_t nSize = 0;
		
		pEncrypt->Encrypt(ENUM_ENCRYPT_AES, inData, 10, encDataAES, nSize);
		pEncrypt->Encrypt(ENUM_ENCRYPT_RC6, inData, 10, encDataRC6, nSize);

		pEncrypt->Decrypt(ENUM_ENCRYPT_AES, encDataAES, 16, decDataAES, nSize);
		pEncrypt->Decrypt(ENUM_ENCRYPT_RC6, encDataRC6, 16, decDataRC6, nSize);


		DestroyEncrypt(pEncrypt);
	}
		
	system("Pause");
	return 0;
}

