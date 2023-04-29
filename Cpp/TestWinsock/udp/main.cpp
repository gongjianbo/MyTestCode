#include <iostream>
#include <stdio.h>
#include <memory.h>
#include <string.h>
// WIN32_LEAN_AND_MEAN 宏防止 Windows.h 包含 Winsock.h
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "Ws2_32.lib")

int main()
{
	// 调用 Winsock 函数之前初始化 WSADATA
	WSADATA wsa_data;
	int ret;
	// WSAStartup 启动 WS2_32 dll 的使用，2.2 是请求的 Winsock 版本
	ret = ::WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (ret != 0) {
		std::cout << "WSAStartup error: " << ret << std::endl;
		return -1;
	}

	// family:
	// AF_INET IPV4
	// AF_INET6 ipv6
	// AF_UNSPEC 协议无关
	// socktype:
	// SOCK_STREAM 流
	// SOCK_DGRAM 数据报
	// protocol:
	// IPPROTO_TCP TCP
	// IPPROTO_UDP UDP
	SOCKET udp_socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (udp_socket == SOCKET_ERROR) {
		std::cout << "init socket error: " << std::endl;
		return -1;
	}

	// 本程序作为 client
	SOCKADDR_IN client_addr;
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = ::htons(8848);
	//addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	client_addr.sin_addr.S_un.S_addr = ::htonl(INADDR_ANY);

	int opt_size = 1024 * 1024;
	int opt_len = sizeof(opt_size);
	// SO_RCVBUF 接收缓冲区大小
	::setsockopt(udp_socket, SOL_SOCKET, SO_RCVBUF, (const char*)&opt_size, opt_len);
	ret = ::getsockopt(udp_socket, SOL_SOCKET, SO_RCVBUF, (char*)&opt_size, &opt_len);
	if (ret == 0) {
		std::cout << "SOL_SOCKET SO_RCVBUF " << opt_size << std::endl;
	}
	ret = ::getsockopt(udp_socket, SOL_SOCKET, SO_MAX_MSG_SIZE, (char*)&opt_size, &opt_len);
	if (ret == 0) {
		std::cout << "SOL_SOCKET SO_MAX_MSG_SIZE " << opt_size << std::endl;
	}
	// 此值为0，会将套接字设置为阻塞(默认的)；为1设置为非阻塞
	//unsigned long blocking = 0;
	//ret = ::ioctlsocket(udp_socket, FIONBIO, &blocking);
	//if (ret != 0) {
	//	std::cout << "set block error:" << ret << std::endl;
	//	return -1;
	//}
	// winsock 不是用 timeval 结构体设置超时，直接设置毫秒数
	int time_out = 3000;
	// SO_SNDTIMEO 发送超时
	// SO_RCVTIMEO 接收超时
	ret = setsockopt(udp_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&time_out, sizeof(time_out));
	if (ret != 0) {
		std::cout << "set timeout error:" << ret << std::endl;
		return -1;
	}

	ret = ::bind(udp_socket, (SOCKADDR*)&client_addr, sizeof(SOCKADDR_IN));
	if (ret != 0) {
		std::cout << "bind socket error: " << ret << std::endl;
		return -1;
	}

	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = ::htons(12315);
	//server_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	// inet_addr 仅适用于 IPV4，inet_pton 支持 IPV4 和 IPV6
	::inet_pton(AF_INET, "127.0.0.1", (void*)&server_addr.sin_addr.s_addr);
	int addr_len = sizeof(SOCKADDR_IN);
	char send_buf[] = "hello udp!";
	char recv_buf[1024] = { 0 };
	// sendto 发送数据报
	// 如果 socket 未绑定，系统会分配唯一值并标记为绑定
	// 不要超过基础子网的最大数据包大小，可以使用 get/setsocketopt 读写 SO_MAX_MSG_SIZE，默认 64k
	// 如果数据太长而无法通过基础协议进行原子传递，会返回 WSAEMSGSIZE 错误，并且不会传输任何数据
	ret = ::sendto(udp_socket, send_buf, sizeof(send_buf), 0, (SOCKADDR*)&server_addr, addr_len);
	if (ret < 0) {
		std::cout << "send data error: " << ret << std::endl;
		return -1;
	}
	// recvfrom 接收数据报
	// 读取已连接或未连接的套接字上的传入数据，并捕获发送数据的地址
	// 如果数据报大于指定的缓冲区，对于 UDP 会丢弃多余数据，并返回 WSAEMSGSIZE 错误
	// 当 socket 缓冲区满时，新来的数据报无法进入接收缓冲区，此数据报丢弃
	// 如果先 sendoto，但是远端还没绑定，此时有 icmp 错误，recvfrom 会直接返回，lasterror 为 WSAECONNRESET
	ret = ::recvfrom(udp_socket, recv_buf, sizeof(recv_buf), 0, (SOCKADDR*)&server_addr, &addr_len);
	if (ret > 0) {
		std::cout << "recv: " << recv_buf << std::endl;
	}
	else {
		// WSAGetLastError 获取错误码
		// WSAETIMEDOUT 10060 超时
		// WSAECONNRESET 10054 远端可能没绑定
		std::cout << "recv data error: " << ret << " " << WSAGetLastError() << std::endl;
		return -2;
	}
	::closesocket(udp_socket);

	// WSACleanup 终止 WS2_32 dll 的使用
	::WSACleanup();

	std::cout << "enter anykey to close.";
	(void)getchar();
	return 0;
}
