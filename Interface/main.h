#include <filesystem>
#include <string>
#include <SFML/Graphics.hpp>

#include "enums.h"

using std::string;

#if __linux__
	#define slash '/'
#else
	#define slash '\\'
#endif

//Menus side
void buildMenus(sf::Font _font);
void showMenu(int selected, bool toggle);
int checkOpen();

//Root/main
void startGame(string rule_file, string save_file);
void quitGame(bool save);
void changeCardset(string file);
void setGameSize(int width, int height);

//Random helpers
int bet(int min, int value, int max);