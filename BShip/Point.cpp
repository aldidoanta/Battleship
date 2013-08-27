/* NIM/Nama		: 13511031/Aldi Doanta Kurnia */
/* Nama file	: Stack.h*/
/* Topik		: kelas, objek, empat sekawan, const, static, operator= */
/* Tanggal 		: 31 Januari 2013 */
/* Deskripsi	: Membuat file implementasi class Point*/

#include <iostream>
#include "Point.h"
using namespace std;

//GET dan SET
int Point::GetAbsis()
{
	return(X);
}

int Point::GetOrdinat()
{
	return(Y);
}

void Point::SetAbsis(int NewAbsis)
{
	X = NewAbsis;
}

void Point::SetOrdinat(int NewOrdinat)
{
	Y = NewOrdinat;
}

//konstruktor
Point::Point()
{
	X = 0;
	Y = 0;
}

Point::Point(int NewAbsis, int NewOrdinat)
{
	X = NewAbsis;
	Y = NewOrdinat;
}

//copy constructor
Point::Point (const Point& P)
{
	this->X = P.X;
	this->Y = P.Y;
}

//destruktor
Point::~Point()
{
	
}

//operator=
Point& Point::operator=(const Point& P)
{
	X = P.X;
	Y = P.Y;
	return *this;
}

//relasional
int Point::IsEqual(Point P) const
{
	return((X == P.GetAbsis()) && (Y == P.GetOrdinat()));
}
int Point::LT (Point P1,Point P2) const
{
	return ( (P1.GetAbsis() < P2.GetAbsis()) && (P1.GetOrdinat() < P2.GetOrdinat()));
}

int Point::Operator (Point P1) const
{
	return ( (P1.GetAbsis() < X) && (P1.GetOrdinat() < Y));
}

//predikat lain
int Point::IsOrigin() const
{
	return (( X == 0) && (Y == 0));
}

//function member
void Point::Mirror()
{
	X = -X;
	Y = -Y;
}

Point Point::MirrorOf()
{
	int TempX = -X;
	int TempY = -Y;
	return(Point (TempX,TempY));
}

void Point::PrintObj()
{
	cout << "Point = (" << X << "," << Y << ")" << endl;
}
