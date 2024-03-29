#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include "file_packet.h"
#include "file_packet.cpp"

int main(){
  //Create a socket
  int listening = socket(AF_INET, SOCK_STREAM, 0);
  if (listening == -1){
    std::cerr << "Did not create the socket~!";
    return -1;
  }

  //bind the socket to a ip and port
  sockaddr_in hint;
  hint.sin_family = AF_INET;
  hint.sin_port = htons(8080); //port is here
  inet_pton(AF_INET, "10.0.0.4", (sockaddr*)&hint.sin_addr); //connects a number to an array of integers.

  if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1){
    std::cerr << "Cannot bind to IP/Port";
    return -2;
  }
  //Make the socket listen
  std::cout << "Listening for Client" << std::endl;
  if (listen(listening, SOMAXCONN) == -1){
    std::cerr << "Cannot listen";
    return -3;
  }

  //receive and send data
  sockaddr_in client;
  socklen_t clientSize;
  char host[NI_MAXHOST];
  char svc[NI_MAXSERV];

  int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

  if (clientSocket == -1){
    std::cerr << "Problem with the client connecting";
  }

  close(listening);

  memset(host, 0, NI_MAXHOST);
  memset(svc, 0, NI_MAXSERV);

  int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);

  if (result){
    std::cout << "Host Connected (can get name)" << std::endl;
  } else {
    inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
    std::cout << "Connected (cannot get name)" << std::endl;
  }
  int bytes = 0;
  int packets = 0;
  char buf[4096];
  File file("tester.o", 18784);
  while (true){
    //Clear buffer
    //wait for message
    int bytesRecv = recv(clientSocket, buf, 4096, 0);
    Packet packet(buf, bytesRecv, false);
    bytes = bytes + bytesRecv;
    if (bytesRecv == -1) {
      std::cerr << "There was a connection issue" << std::endl;
      break;
    }
    if (bytesRecv == 0 ){
      std::cout << "The client disconnected" << std::endl;
      break;
    }
    ++packets;
    file.pushPacket(packet);
  }
  file.packetsToFile("tester.o");
  std::cout << packets << std::endl;
  std::cout << bytes << std::endl;

  close(clientSocket);

  //close the socket
  return 0;
}
