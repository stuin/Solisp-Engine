#include "GameMenu.hpp"

class MainMenu : public DrawNode {
private:
	Node *menus[2];
	SubMenu *buttons[2];
	sf::RectangleShape fade;

public:
	MainMenu() : DrawNode(fade, FADE) {
		//Set up faded background
		fade.setSize(sf::Vector2f(700, 1090));
		fade.setFillColor(sf::Color(0, 0, 0, 50));

		menus[0] = new GameMenu(this);
		buttons[0] = new SubMenu(0, "Solitaire", menus[0], this);
	}

	void hideOthers(int selected) {
		for(int i = 0; i < 1; i++) {
			if(i != selected)
				menus[i]->setHidden(true);
		}
	}
};

void SubMenu::click() {
	menu->setHidden(!menu->isHidden());
	((MainMenu *)getParent())->hideOthers(i);
}

void buildMenus() {
	//Load important resources
	if(!cardset.loadFromFile("res/base_deck.png"))
		throw std::invalid_argument("Card texture not found");
	if(!font.loadFromFile("/usr/share/fonts/TTF/DejaVuSerif.ttf"))
		throw std::invalid_argument("Font file not found");

	MainMenu *menu = new MainMenu();
	UpdateList::addNode(menu);
}