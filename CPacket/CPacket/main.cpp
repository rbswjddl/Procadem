#include<iostream>

int main()
{
	char* buffer = new char[100];
	char ch = 10;
	short sh = 342;
	int inum = 90000;
	
	*buffer = 230;
	*(buffer + 1) = sh;
	*((int*)(buffer + 3)) = inum;

	printf("%d\n", *(unsigned char*)buffer);
	printf("%d\n", *((short*)(buffer + 1)));
	printf("%d\n", *((int*)(buffer + 3)));
}