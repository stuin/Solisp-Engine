#include <filesystem>
#include <string>

#include "enums.h"

using std::string;

#if __linux__
	#define slash '/'
#else
	#define slash '\\'
#endif

//Menus side
void buildMenus();
void showMenu();

//Main side
void startGame(string rule_file, string save_file="");
void changeCardset(string file);
void showWin();

//Random helpers
int bet(int min, int value, int max);