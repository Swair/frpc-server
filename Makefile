

COMMON := core/Reactor.cc core/Socket.cc core/LogWrite.cc core/Proto.cc



CC := g++ -g -Wall -fpermissive -std=c++14

all:
	$(CC) remote_call_client.cc $(COMMON) -Icore -I. -o remote_call_client -lpthread
	$(CC) remote_call_server.cc $(COMMON) -Icore -I. -o remote_call_server -lpthread
