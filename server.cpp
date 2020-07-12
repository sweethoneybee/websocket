#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <list>
#define PORT 9999

void sendMsg(const int &);
void recvMsg(const int &);

std::mutex sm;
std::list<std::thread> sends;
std::list<std::thread> recvs;
int maxUser = 5;
int main(void)
{
  int server_sock, client_sock;
  struct sockaddr_in server_addr, client_addr;
  char buffer[1024];
  int len, addr_len, recv_len;

  // TCP/IP에서는 SOCK_STREAM을 UDP/IP에서는 SOCK_DGRAM을 사용함
  // server socket 생성
  if ((server_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("socket error");
    return 1;
  }

  // server socket에 대한 설정
  memset(&server_addr, 0x00, sizeof(server_addr));
  server_addr.sin_family = AF_INET;                // IPv4 protocol
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // accept all 32bit IPv4 ip addresses
  server_addr.sin_port = htons(PORT);              // use PORT 9999

  // 커널에 server_sock 등록해서 사용할 수 있게 하기
  if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    perror("bind error");
    return 1;
  }

  if (listen(server_sock, 5) < 0)
  {
    perror("listen error");
    return 1;
  }

  addr_len = sizeof(client_addr);

  printf("wating for client...\n");

  if ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len)) < 0)
  {
    perror("connect error!\n");
    return 1;
  }
  printf("client ip: %s\n", inet_ntoa(client_addr.sin_addr));

  std::thread t1(sendMsg, client_sock);
  std::thread t2(recvMsg, client_sock);

  t1.join();
  t2.join();
  close(client_sock);

  close(server_sock);
  return 0;
}

void sendMsg(const int &client_sock)
{
  std::string inputMsg;
  char sendBuffer[1024];

  memset(sendBuffer, 0, sizeof(char) * 1024);
  printf("YOU>> ");

  while (inputMsg.compare("exit") != 0)
  {
    std::getline(std::cin, inputMsg);
    int strLen = inputMsg.length() >= 1024 ? 1023 : inputMsg.length();

    for (int i = 0; i < strLen; i++)
      sendBuffer[i] = inputMsg[i];
    sendBuffer[strlen(sendBuffer)] = '\0';

    if (send(client_sock, sendBuffer, strlen(sendBuffer) + 1, 0) < 0)
    {
      printf("server closed!\n");
      close(client_sock);
      return;
    }
    memset(sendBuffer, 0, sizeof(char) * 1024);
  }

  memset(sendBuffer, 0, sizeof(char) * 1024);
  send(client_sock, "server closed!\0", 14, 0);
  close(client_sock);
  return;
}

void recvMsg(const int &client_sock)
{
  int recv_len;
  char buffer[1024];
  memset(buffer, 0, 1024);
  while ((recv_len = recv(client_sock, buffer, 1024, 0)) > 0)
  {
    // 안전을 위해 받는 쪽, 보내는 쪽 둘다 \0 처리를 해줌.
    buffer[recv_len] = '\0';
    if (strcmp(buffer, "exit") == 0)
    {
      printf("disconnect...\n");
      close(client_sock);
      break;
    }
    printf("\nSTRANGER: %s\n", buffer);
    memset(buffer, 0, 1024);
  }

  return;
}
