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
void showMenu(int selected, bool toggle=false);
int checkOpen();

//Main side
void startGame(string rule_file, string save_file="");
void changeCardset(string file);
void quit(bool save);
void setGameSize(int width, int height);

//Random helpers
int bet(int min, int value, int max);