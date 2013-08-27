// File : Ship.h
#ifndef SHIP_H
#define SHIP_H

#include <string>
#include <vector>
#include "Point.h"

//use 0 as first index
#define FIRSTIDX 0

using namespace std;

//the ship
class Ship{
	private:
	//data member
	string Name;
	int Length;
	int Health;
	struct Block{
		Point Position;
		//determines whether the block has been hit
		bool IsHit;
	};
	vector <Block> Part;
	
	public:
	//default ctor
	Ship();
	//user-defined ctor
	Ship(string,int);
	//cctor
	Ship(const Ship&);
	//dtor
	~Ship();
	//operator=
	Ship& operator=(const Ship&);
	
	//function member
	string GetName();
	int GetLength();
	int GetHealth();
	Point GetPosition(int);
	bool GetHitStatus(int);
	
	
	void SetName(string);
	void SetLength(int);
	void SetHealth(int);
	void SetPosition(int, Point);
	void SetHitStatus(int, bool);

	//Writes the position of every part of the ship
	void SetPartPosition(int, int, char, string);

	//Checks if Point P matches the one of the part's position
	//If it matches, set the part as hit
	void HitPart(Point);
};

#endif
