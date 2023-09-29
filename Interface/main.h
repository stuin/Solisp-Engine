#include <filesystem>
#include <functional>
#include <string>
#include <SFML/Graphics.hpp>

#include "enums.h"
#include "Skyrmion/VertexGraph.hpp"

using std::string;
using std::function;

typedef std::function<void(void)> clickptr;

#if __linux__
	#define slash '/'
#else
	#define slash '\\'
#endif

//Menus side
void addActionButton(int tindex, clickptr func);
void showMenu(int i);
bool gameOpen();
void buildMenus();

//Root+main
void startGame(string rule_file, string save_file);
void joinServer(string ip);
void quitGame(bool save);
void restartGame();
void setGameSize(int width, int height);
sf::Texture *getCardset();

//Random helpers
int bet(int min, int value, int max);