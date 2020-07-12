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

#define PORT 9999

void sendMsg(const int &);
void recvMsg(const int &);

int main(void)
{
  int client_sock;
  struct sockaddr_in server_addr;

  const char *msg = "hello! world!";
  int recv_len;

  if ((client_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("socket error");
    return 1;
  }

  memset(&server_addr, 0x00, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  server_addr.sin_port = htons(PORT);

  if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    perror("connect error!");
    return 1;
  }

  printf("connect to server successfully!\n");
  printf("server ip: %s\n", inet_ntoa(server_addr.sin_addr));
  printf("type 'exit' to disconnect\n");

  std::thread t1(sendMsg, client_sock);
  std::thread t2(recvMsg, client_sock);

  t1.join();
  t2.join();

  close(client_sock);
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
      return;
    }
    memset(sendBuffer, 0, sizeof(char) * 1024);
  }
  return;
}

void recvMsg(const int &client_sock)
{
  int recv_len;
  char recvBuffer[1024];
  memset(recvBuffer, 0, 1024);

  while ((recv_len = recv(client_sock, recvBuffer, 1024, 0)) > 0)
  {
    recvBuffer[recv_len] = '\0';
    printf("\nSTRANGER: %s", recvBuffer);
    memset(recvBuffer, 0, sizeof(char) * 1024);
  }

  return;
}