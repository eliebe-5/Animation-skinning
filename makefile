CC=g++
CFLAGS=-I. -lGLEW -lGL -lglfw -ltinyxml -lglut
DBFLAG=-g
ENGINE= engine/config.cc engine/core/app.cc engine/render/window.cc


labb1:
	$(CC) -std=c++0x -o Labb1 Projects/Labb1/code/main.cc Projects/Labb1/code/exampleapp.cc Projects/Labb1/code/model.cc Projects/Labb1/code/animation.cc Projects/Labb1/code/TextureResource.cc $(ENGINE) $(CFLAGS)

labb1-db:
	$(CC) -std=c++0x -o Labb1 Projects/Labb1/code/main.cc Projects/Labb1/code/exampleapp.cc Projects/Labb1/code/model.cc Projects/Labb1/code/animation.cc Projects/Labb1/code/TextureResource.cc $(ENGINE) $(CFLAGS) $(DBFLAG)

