#include <filesystem>
#include <string>
#include <SFML/Graphics.hpp>

#include "enums.h"

using std::string;
using std::function;
typedef function<void(void)> clickptr;

#if __linux__
	#define slash '/'
#else
	#define slash '\\'
#endif

//Menus side
void buildMenus();
void showMenu(int selected, bool toggle);
void addActionButton(int tindex, clickptr func);
int checkOpen();

//Root+main
void startGame(string rule_file, string save_file);
void joinServer(string ip);
void quitGame(bool save);
void setGameSize(int width, int height);
sf::Texture *getCardset();

//Random helpers
int bet(int min, int value, int max);