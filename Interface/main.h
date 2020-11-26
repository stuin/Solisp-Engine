#include "enums.h"

using std::string;

//Menus side
void buildMenus();
void showMenu();

//Main side
void startGame(string file);
void changeCardset(string file);
void showWin();

//Random helpers
int bet(int min, int value, int max);