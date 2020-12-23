#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

#include "Button.hpp"

class FolderMenu : public SubMenu {
private:
	string dir;
	string ext;
	function<string(string)> name;
	function<clickptr(string, Node*)> func;

	fs::file_time_type edited;

public:
	FolderMenu(string dir, string ext, function<string(string)> name,
			function<clickptr(string, Node*)> func, Node *parent)
			: SubMenu(sf::Vector2i(400, 1090), parent) {
		setPosition(310, 0);

		//Save values to populate menu
		this->dir = dir;
		this->ext = ext;
		this->name = name;
		this->func = func;
	}

	void reload() {
		if(fs::last_write_time(dir) == edited)
			return;

		//Reset list and mark time
		clearContents();
		edited = fs::last_write_time(dir);

		//Set up game list
		int y = 0;
		for(auto &entry : fs::directory_iterator(dir)) {
			if(entry.is_regular_file() && entry.path().extension().string() == ext) {
				string file = entry.path().relative_path().string();
				addButton(name(file), y += 70, 300, this, func(file, getParent()));
			}
		}
	}
};