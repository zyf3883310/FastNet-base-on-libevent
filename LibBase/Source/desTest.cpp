#include <iostream>
#include "des.h"
using namespace std;

//int DECRYPT(u_char *key, unsigned char *s, unsigned char *d, unsigned short len);
//int ENCRYPT(u_char *key, unsigned char *s, unsigned char *d, unsigned short len);


int main()
{
	char key[10] = "key";
	char input[20] = "aaa";
	char output[20] = { 0 }; // 加密输出
	char output2[20] = { 0 };  //解密输出
	
	//以下加密，你把output发给服务端就可以了啊。
	int len = ENCRYPT((u_char *)key, (u_char *)input, (u_char *)output, strlen(input));
	



	//服务端收到的output，解密到output2中处理
	DECRYPT((u_char *)key, (u_char *)output2, (u_char *)output, len);
	cout << "解密:" << output2 << endl;
	

	cin.get();
	return 0;
}