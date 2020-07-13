## C++를 이용한 웹소켓 예제

#### server

TCP 소켓을 만들고 커널에 정보를 등록한 다음 접속을 기다린다(listen).  
이후 누군가 접속하면 accept 함수를 통해 소켓 정보를 저장하고  
send, recv 함수를 통해 정보를 전달하고 받는다.  
send, recv 역할을 쓰레드로 나누어서 구현했다.

#### client.cpp

서버에 비해 간단하다.  
TCP 소켓을 만들고 서버의 ip주소, 포트번호, 전송계층의 방식을 저장하고  
connect 함수를 통해 서버의 소켓에 연결을 시도한다.  
연결에 성공하면 역시 send, recv 함수를 통해 정보를 전달하고 받으며  
두 개의 쓰레드로 send, recv를 하게 하였다.

#### 공통

통신이 끝나면 close를 통해 소켓을 닫아주어야 한다.