//File : Point.h

#ifndef GPOINT_H
#define GPOINT_H

class Point
{
	private:
	//data member
	int X;
	int Y;
	
	public:
	//constructor
	//default
	Point ();
	//user-defined
	Point (int,int);

	//copy constructor
	Point(const Point&);
	
	//destructor
	~Point();
	
	//operator=
	Point& operator= (const Point&);

	//GET and SET
	int GetAbsis();
	int GetOrdinat();
	void SetAbsis(int);
	void SetOrdinat(int);
	
	//Relational
	int IsEqual(Point P) const;
	int LT (Point P1, Point P2) const;
	/* menghasilkan true jika P1 < P2 : absis dan ordinat lebih kecil */
	int Operator (Point P1) const;
	/* menghasilkan true jika P1 < Current_Object : absis dan ordinat lebih kecil */
	//predikat lain
	int IsOrigin() const;
	/* menghasilkan true jika Current_Object berada pada (0,0), selain itu false. */
	
	//function member
	void Mirror ();
	Point MirrorOf();
	void PrintObj();
};
#endif
