//File : Grid.cpp

#include "Grid.h"

//default ctor
Grid::Grid(){
	Area.resize(MAX_X);
	for (int i = FIRSTIDX; i<MAX_X; i++){
		Area[i].resize(MAX_Y);
	}
	for (int i = FIRSTIDX; i<MAX_X; i++){
		for (int j = FIRSTIDX; j<MAX_Y; j++){
			//initialize every block
			Area[i][j].IsHit = false;
			Area[i][j].IsOccupied = false;
		}
	}
	CurrPos.SetAbsis(FIRSTIDX+1);
	CurrPos.SetOrdinat(FIRSTIDX+1);
}
//dtor
Grid::~Grid(){
	Area.clear();
	CurrPos.SetAbsis(0);
	CurrPos.SetOrdinat(0);
	//delete[] Area;
}
//cctor
/* Grid::Grid(const Grid& G){
	//Area = new Block[MAX_X][MAX_Y];
	for (int j = FIRSTIDX; j<=MAX_Y; j++){
		for (int i = FIRSTIDX; i<=MAX_X; i++){
			SetHitStatus(i, j, G.GetHitStatus(i,j));
			SetOccupationalStatus(i, j, G.GetOccupationalStatus(i,j));
		}
	}
	CurrPos.SetAbsis(G.GetCurrPos().GetAbsis());
	CurrPos.SetOrdinat(G.GetCurrPos().GetOrdinat());
} */
//operator=
/* Grid& Grid::operator= (const Grid& G){
	for (int j = FIRSTIDX; j<=MAX_Y; j++){
		for (int i = FIRSTIDX; i<=MAX_X; i++){
			SetHitStatus(i, j, G.GetHitStatus(i,j));
			SetOccupationalStatus(i, j, G.GetOccupationalStatus(i,j));
		}
	}
	CurrPos.SetAbsis(G.GetCurrPos().GetAbsis());
	CurrPos.SetOrdinat(G.GetCurrPos().GetOrdinat());
	
	return *this;
} */

//function member
Point Grid::GetCurrPos(){
	return CurrPos;
}
bool Grid::GetHitStatus(int X, int Y){
	return Area[X][Y].IsHit;
}
bool Grid::GetOccupationalStatus(int X, int Y){
	return Area[X][Y].IsOccupied;
}

void Grid::SetCurrPos(Point P){
	CurrPos = P;
}
void Grid::SetHitStatus(int X, int Y, bool NewHitStatus){
	Area[X][Y].IsHit = NewHitStatus;
}
void Grid::SetOccupationalStatus(int X, int Y, bool NewOccupationalStatus){
	Area[X][Y].IsOccupied = NewOccupationalStatus;
}

//Set placed ship's position as occupied
void Grid::OccupyGrid(int XPixel, int YPixel, int BlockLength, char CurrentOri, string CurrentPlayer)
{
	int XBlock;
	int YBlock = (YPixel-60)/40;

	if (CurrentPlayer == "P1")
	{
		XBlock = (XPixel-10)/40;
	}
	else //if (CurrentPlayer == "P2")
	{
		XBlock = (XPixel-534)/40;
	}

	if(CurrentOri == 'H')
	{
		for (int i = XBlock; i < XBlock+BlockLength; i++)
		{
			//set the occupation status
			SetOccupationalStatus(i,YBlock,true);
		}
	}
	else //if(CurrentOri == 'V')
	{
		for (int i = YBlock; i < YBlock+BlockLength; i++)
		{
			//set the occupation status
			SetOccupationalStatus(XBlock,i,true);
		}
	}
}
