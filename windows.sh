files="Interface/main.cpp Interface/Menus/MainMenu.cpp Interface/Skyrmion/GridMaker.cpp Interface/Skyrmion/Node.cpp Interface/Skyrmion/UpdateList.cpp Gameplay/builder.cpp Gameplay/card.cpp Gameplay/game.cpp Lisp/Solisp/src/lisp.cpp Lisp/Solisp/src/reader.cpp Lisp/Solisp/src/library.cpp Lisp/card_env.cpp Lisp/card_library.cpp Lisp/game_library.cpp"
res="res/*.o res/faces/*.o"
options="-pthread -O3 -std=c++17 -Wall -fpermissive -mwindows -static -DSFML_STATIC"
libs="-lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lwinmm -lgdi32 -lopengl32 -lfreetype"

version=$(cat version)
output="builds/Solitaire-$version"

if [ -e "$output" ]; then
	echo "Error: version $version already exists"
else
	cmd="x86_64-w64-mingw32-g++ $files $res -o $output/solitaire.exe $options $libs"

	mkdir $output
	echo $cmd
	$cmd

	#cp -r res $output
	cp -r Games $output
	mkdir "$output/saves"

	rm "$output/Games/Pyramid.solisp"
	rm "$output/Games/Gridcannon.solisp"

	zip -r "$output.zip" "$output"
fi