//------------------------------------------------------------------------------
// main.cc
// (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "engine/config.h"
#include "Projects/Labb1/code/exampleapp.h"

int
main(int argc, char** argv)
{

    glutInit(&argc, argv);
    
	Example::ExampleApp app;
	if (app.Open())
	{
		app.Run();
		app.Close();
	}
	app.Exit();
	
}
