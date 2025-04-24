all: prepare_dirs compile_src compile_imgui link

prepare_dirs:
	mkdir -p build

compile_src:
	g++ -std=c++17 -Wall -g -Iinclude -Iimgui -Iimgui/backends -I/usr/local/include -c src/*.cpp
	mv *.o build/

compile_imgui:
	g++ -std=c++17 -Wall -g -Iimgui -Iimgui/backends -I/usr/local/include -c imgui/imgui.cpp -o build/imgui.o
	g++ -std=c++17 -Wall -g -Iimgui -Iimgui/backends -I/usr/local/include -c imgui/imgui_demo.cpp -o build/imgui_demo.o
	g++ -std=c++17 -Wall -g -Iimgui -Iimgui/backends -I/usr/local/include -c imgui/imgui_draw.cpp -o build/imgui_draw.o
	g++ -std=c++17 -Wall -g -Iimgui -Iimgui/backends -I/usr/local/include -c imgui/imgui_tables.cpp -o build/imgui_tables.o
	g++ -std=c++17 -Wall -g -Iimgui -Iimgui/backends -I/usr/local/include -c imgui/imgui_widgets.cpp -o build/imgui_widgets.o
	g++ -std=c++17 -Wall -g -Iimgui -Iimgui/backends -I/usr/local/include -c imgui/backends/imgui_impl_sdl3.cpp -o build/imgui_impl_sdl3.o
	g++ -std=c++17 -Wall -g -Iimgui -Iimgui/backends -I/usr/local/include -c imgui/backends/imgui_impl_sdlrenderer3.cpp -o build/imgui_impl_sdlrenderer3.o

link:
	g++ -std=c++17 -Wall -g build/*.o -o main -lSDL3 -lSDL3_image -lSDL3_mixer -lGL

clean:
	rm -rf build/*.o main

.PHONY: all clean prepare_dirs compile_src compile_imgui link