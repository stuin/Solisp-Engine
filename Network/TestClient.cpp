#include "../Gameplay/game.h"

#include <brynet/net/TcpService.hpp>
#include <brynet/net/AsyncConnector.hpp>
#include <brynet/net/wrapper/ConnectionBuilder.hpp>

#include <iostream>

using std::string;
using std::cout;
using Solisp::MovePacket;
using namespace brynet;
using namespace brynet::net;
using namespace brynet::base;
using DataCallback = std::function<size_t(const char* buffer, size_t len)>;

struct SETUP {
	unc players = 0;
	unsigned int seed;
	unsigned int ruleSize;
};

Solisp::Game game;
unsigned int move_id;
bool move_updating = false;

int main(int argc, char const *argv[]) {
	string ip = "127.0.0.1";
	if(argc > 1)
		ip = argv[1];

	int port = atoi("1234");
	auto service = TcpService::Create();
    service->startWorkerThread(port);

    auto connector = AsyncConnector::Create();
    connector->startWorkerThread();

    auto enterCallback = [](const TcpConnection::Ptr& session) {
    	auto baseCallback = [session](const char* buffer, size_t len) mutable {
    		if(move_updating) {
				struct MovePacket *move = (struct MovePacket *) buffer;
	    		cout << "  " << move->id << ": " << move->count << " cards\n";

	    		if(move->id >= move_id)
	    			move_updating = false;
    		} else {
    		char c = buffer[0];
	    		switch (c) {
	    			case 'c':
	    				cout << "New player connected\n";
	    				break;
	    			case 'd':
	    				cout << "Player disconnected\n";
	    				break;
	    			case 'm':
	    				memcpy(&move_id, buffer + 1, 4);
	    				cout << "Receiving moves:\n";
	    				move_updating = true;
	    				break;
	    		}
	    	}
	    	return len;
    	};

    	cout << "Connection successfull\n";
        session->setDataCallback([session, &baseCallback](const char* buffer, size_t len) {
            struct SETUP *setup = (struct SETUP *) buffer;
            const char *rules = buffer + sizeof setup;
            cout << rules;

            session->setDataCallback(baseCallback);
            return len;
        });
    };

    auto failedCallback = []() {
        cout << "Connection failed\n";
    };

    wrapper::ConnectionBuilder connectionBuilder;
    connectionBuilder.configureService(service)
        .configureConnector(connector)
        .configureConnectionOptions({
            brynet::net::AddSocketOption::AddEnterCallback(enterCallback),
            brynet::net::AddSocketOption::WithMaxRecvBufferSize(1024 * 1024)
        }
    );

    try {
        connectionBuilder.configureConnectOptions({
            ConnectOption::WithAddr(ip, port),
            ConnectOption::WithTimeout(std::chrono::seconds(10)),
            ConnectOption::WithFailedCallback(failedCallback),
            ConnectOption::AddProcessTcpSocketCallback([](TcpSocket& socket) {
                socket.setNodelay();
            })
        }).asyncConnect();
    }
    catch (std::runtime_error& e) {
        std::cout << "error:" << e.what() << std::endl;
    }

    while(true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

	return 0;
}