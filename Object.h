#pragma once
#include <iostream>
#include <vector>

using namespace std;

class Object
{
private:
	string name;
	int depth;
	
public:
	Object();
	~Object();
	Object(string name, int depth);
	
	string getName();
	int getDepth();
	

};