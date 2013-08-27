//File : Ship.cpp

#include "Ship.h"
#include <iostream>
#include <string>
using namespace std;

//default ctor
Ship::Ship(){
	Name = "DummyShip";
	Length = 1;
	Health = Length;
	Part.resize(Length);
	Point P = Point(0,0);
	Part[FIRSTIDX].Position = P;
	Part[FIRSTIDX].IsHit = false;
}
//user-defined ctor
Ship::Ship(string NewName,int NewLength){
	Name = NewName;
	Length = NewLength;
	Health = Length;
	Part.resize(Length);
}
//cctor
Ship::Ship(const Ship& S){
	this->Name = S.Name;
	this->Length = S.Length;
	this->Health = S.Health;
	Part.resize(Length);
	for(int i = FIRSTIDX; i<Length; i++){
		Part[i].Position = S.Part[i].Position;
		Part[i].IsHit = S.Part[i].IsHit;
	}
}
//dtor
Ship::~Ship(){
	Name = "";
	Length = 0;
	Health = 0;
}
//operator=
Ship& Ship::operator=(const Ship& S){
	Name = S.Name;
	Length = S.Length;
	Health = S.Health;
	Part.resize(Length);
	for(int i = FIRSTIDX; i<Length; i++){
		Part[i].Position = S.Part[i].Position;
		Part[i].IsHit = S.Part[i].IsHit;
	}
	return *this;
}

//function member
string Ship::GetName(){
	return Name;
}
int Ship::GetLength(){
	return Length;
}
int Ship::GetHealth(){
	return Health;
}
Point Ship::GetPosition(int Idx){
	return Part[Idx].Position;
}
bool Ship::GetHitStatus(int Idx){
	return Part[Idx].IsHit;
}

void Ship::SetName(string NewName){
	Name = NewName;
}
void Ship::SetLength(int NewLength){
	Length = NewLength;
}
void Ship::SetHealth(int NewHealth){
	Health = NewHealth;
}
void Ship::SetPosition(int Idx, Point NewPosition){
	Part[Idx].Position = NewPosition;
}
void Ship::SetHitStatus(int Idx, bool NewHitStatus){
	Part[Idx].IsHit = NewHitStatus;
}

//write the position of every part of the ship
void Ship::SetPartPosition(int XPixel, int YPixel, char CurrentOri, string CurrentPlayer)
{
	int XBlock;
	int YBlock = (YPixel-60)/40;
	Point P(0,0);

	//convert the position in pixel to position in grid
	if (CurrentPlayer == "P1")
	{
		XBlock = (XPixel-10)/40;
	}
	else //if (CurrentPlayer == "P2")
	{
		XBlock = (XPixel-534)/40;
	}

	P.SetAbsis(XBlock);
	P.SetOrdinat(YBlock);

	if(CurrentOri == 'H')
	{
		for (int i=FIRSTIDX; i<Length; i++)
		{
			SetPosition(i,P);
			SetHitStatus(i,false);
			XBlock++;
			P.SetAbsis(XBlock);
		}
	}
	else //if(CurrentOri == 'V')
	{
		for (int i=FIRSTIDX; i<Length; i++)
		{
			SetPosition(i,P);
			SetHitStatus(i,false);
			YBlock++;
			P.SetOrdinat(YBlock);
		}
	}
	
}

void Ship::HitPart(Point P){
	bool Found = false;
	int Idx = FIRSTIDX;

	while ((!(Found)) && (Idx < GetLength()))
	{
		if(P.IsEqual(GetPosition(Idx)))
		{
			//set the part as hit
			SetHitStatus(Idx,true);
			//reduce the ship's health
			Health--;
			Found = true;
		}
		Idx++;
	}
	//(Found) or (Idx == GetLength())
}