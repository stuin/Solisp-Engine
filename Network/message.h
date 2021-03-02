#define _LINUX true
#define MAX_PACKET 4096
#define RULE_CACHE "rules.cache"

#include <clsocket/PassiveSocket.h>
#include <iostream>
#include <queue>

using std::cout;
using std::string;
using Solisp::MovePacket;
using Solisp::Move;
using Solisp::Game;
using Solisp::Hand;
using p = const uint8*;

struct Message {
	char type;
	unc slot;
	unsigned int count;
};