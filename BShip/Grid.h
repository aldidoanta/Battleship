// File : Grid.h
#ifndef GRID_H
#define GRID_H

//#include <string>
#include "Point.h"
#include <vector>
#include <iostream>
#include <string>
//constants for the area of the grid
#define MAX_X 11
#define MAX_Y 11
//use 0 as first index
#define FIRSTIDX 0
using namespace std;

//grid represents the "battlefield" of battleships
class Grid{
	private:
	//data member
	struct Block{
		//determines whether the block has been hit
		bool IsHit;
		//determines whether the block contains a part of a ship
		bool IsOccupied;
	};

	vector< vector<Block> > Area;

	//current position of the pointer
	Point CurrPos;
	
	public:
	//default ctor
	Grid();
	//no user-defined ctor
	//dtor
	~Grid();
	//cctor
	// Grid(const Grid&);
	//operator=
	// Grid& operator= (const Grid&);
	
	//function member
	Point GetCurrPos();
	bool GetHitStatus(int, int);
	bool GetOccupationalStatus(int, int);
	
	void SetCurrPos(Point);
	void SetHitStatus(int, int, bool);
	void SetOccupationalStatus(int, int,  bool);

	//Set placed ship's position as occupied
	void OccupyGrid(int, int, int, char, string);
};

#endif
