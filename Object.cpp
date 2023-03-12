#include "Object.h"


Object::Object()
{
	name = "";
	depth = 0;
}

Object::~Object()
{
	
}

Object::Object(string name, int depth)
{
	this->name = name;
	this->depth = depth;
}

string Object::getName()
{
	return this->name;
}

int Object::getDepth()
{
	return this->depth;
}

