main:
	g++ main.cpp common/loadshader.cpp common/texture.cpp common/controls.cpp -o out -lglfw -lGL -lGLEW -std=c++11