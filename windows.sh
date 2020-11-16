files="Interface/main.cpp Interface/Menus/MainMenu.cpp Interface/Skyrmion/GridMaker.cpp Interface/Skyrmion/Node.cpp Interface/Skyrmion/UpdateList.cpp Gameplay/builder.cpp Gameplay/card.cpp Gameplay/game.cpp Lisp/Solisp/src/lisp.cpp Lisp/Solisp/src/reader.cpp Lisp/Solisp/src/library.cpp Lisp/card_env.cpp Lisp/card_library.cpp Lisp/game_library.cpp"
options="-pthread -O3 -g -std=c++17 -Wall -fpermissive -mwindows -static -DSFML_STATIC"
libs="-lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lwinmm -lgdi32 -lopengl32 -lfreetype"

version=$(cat version)
output="builds/Solitaire-$version"

if [ -e "$output"]; then
	echo "Error: version $version already exists"
else
	cmd="x86_64-w64-mingw32-g++ $files -o $output/solitaire.exe $options $libs"

	echo $cmd
	$cmd

	cp -r res $output
	cp -r Games $output

	rm "$output/Games/Pyramid.solisp"
fi