#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <time.h>
#define WIN32_LEAN_AND_MEAN 
#pragma comment(lib, "Ws2_32.lib")
typedef unsigned long long u64;
u64 GetMicroCounter();
#define BUF_SIZE 3000

int send_simul(SOCKET s, const char* buf, int len, int flags)
{
	srand((unsigned)time(NULL));
	len = 0;
	if (rand() % 20 != 1)
	{
		len = send(s, buf, len, flags);
	}
	return len;
}
int main(int argc, char **argv) {
	u64 start, end;
	WSADATA wsaData;
	struct sockaddr_in server_addr;
	SOCKET s;



	WSAStartup(MAKEWORD(2, 2), &wsaData);

	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Socket Creat Error.\n");
		exit(1);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("1.224.240.122");
	server_addr.sin_port = htons(5000);

	if (connect(s, (SOCKADDR *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
		printf("Socket Connection Error.\n");
		exit(1);
	}

	printf("File Send Start");

	int totalBufferNum;
	int BufferNum;
	int sendBytes;
	long totalSendBytes;
	long file_size;
	char buf[BUF_SIZE];
	char buf2[BUF_SIZE];
	FILE *fp;
	fp = fopen("android2.mp4", "rb");
	if (fp == NULL) {
		printf("File not Exist");
		exit(1);
	}
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	totalBufferNum = file_size / sizeof(buf) + 1;
	fseek(fp, 0, SEEK_SET);
	BufferNum = 0;
	totalSendBytes = 0;

	_snprintf(buf, sizeof(buf), "%d", file_size);
	sendBytes = send(s, buf, sizeof(buf), 0);

	start = GetMicroCounter();
	/*

	*/
	while ((sendBytes = fread(buf, sizeof(char), sizeof(buf), fp))>0) {

		while (send_simul(s, buf, sendBytes, 0) == 0)
		{
			//Packet Loss 발생 다시 보냄
		}

		BufferNum++;
		totalSendBytes += sendBytes;
		printf("In progress: %d/%dByte(s) [%d%%]\n", totalSendBytes, file_size, ((BufferNum * 100) / totalBufferNum));
	}
	end = GetMicroCounter();
	printf("time: %d second(s)", end - start);

	closesocket(s);
	WSACleanup();
	getchar();
	return 0;
}

u64 GetMicroCounter()
{
	u64 Counter;

#if defined(_WIN32)
	u64 Frequency;
	QueryPerformanceFrequency((LARGE_INTEGER *)&Frequency);
	QueryPerformanceCounter((LARGE_INTEGER *)&Counter);
	Counter = 1000000 * Counter / Frequency;
#elif defined(__linux__) 
	struct timeval t;
	gettimeofday(&t, 0);
	Counter = 1000000 * t.tv_sec + t.tv_usec;
#endif

	return Counter;
}