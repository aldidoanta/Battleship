//The headers
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "Ship.h"
#include "Grid.h"
#include <string>
#include <iostream>
#include "ThreeMaxLoader.h"
#include "SDL/SDL_opengl.h"
using namespace std;


float rotation_x=0; float rotation_x_increment=0.0f;
float rotation_y=0; float rotation_y_increment=0.0f;
float rotation_z=0; float rotation_z_increment=0.03f;

obj_type object;

//enum for the filename
int ThreeDSFileSelector = 17;

//Screen attributes
const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 575;
const int SCREEN_BPP = 32;

//The button states in the sprite sheet
const int CLIP_MOUSEOVER = 0;
const int CLIP_MOUSEOUT = 1;
const int CLIP_MOUSEDOWN = 2;
const int CLIP_MOUSEUP = 3;

//The surfaces
SDL_Surface *background = NULL;
SDL_Surface *background2 = NULL;
SDL_Surface *title = NULL;
SDL_Surface *comment = NULL;
SDL_Surface *comment2 = NULL;
SDL_Surface *comment_dummy = NULL;
SDL_Surface *buttonSheet = NULL;
SDL_Surface *grid = NULL;
SDL_Surface *P1Indicator = NULL; 
SDL_Surface *P2Indicator = NULL; 
SDL_Surface *screen = NULL;
SDL_Surface *ship5 = NULL;
SDL_Surface *ship4a = NULL;
SDL_Surface *ship4b = NULL;
SDL_Surface *ship3 = NULL;
SDL_Surface *ship2 = NULL;
SDL_Surface *tile_miss = NULL;
SDL_Surface *tile_hit = NULL;
SDL_Surface *tile_x = NULL;

//The font and the color
TTF_Font *font = NULL;
TTF_Font *font2 = NULL;
TTF_Font *font3 = NULL;
SDL_Color textColor_Red = {255,0,0};
SDL_Color textColor_White =  {255,255,255};

//The event structure
SDL_Event event;

//Quit flag for the main program
bool quit = false;

//Flag for the DEPLOYP1 and DEPLOYP2 state
bool Fixed5 = false;
bool Fixed4a = false;
bool Fixed4b = false;
bool Fixed3 = false;
bool Fixed2 = false;

//Store the "head" position of every ship in pixel
Point HeadP15 = Point(0,0);
Point HeadP14a = Point(0,0);
Point HeadP14b = Point(0,0);
Point HeadP13 = Point(0,0);
Point HeadP12 = Point(0,0);

Point HeadP25 = Point(0,0);
Point HeadP24a = Point(0,0);
Point HeadP24b = Point(0,0);
Point HeadP23 = Point(0,0);
Point HeadP22 = Point(0,0);

//"State" regulator for the main program
enum ProgramState{WELCOME_SCREEN, GAME, DEPLOYP1, DEPLOYP2, VICTORY, GALLERY_INTRO, GALLERY};
ProgramState CurrentState = WELCOME_SCREEN;

//determines the playing player
enum PlayerState{P1,P2};
PlayerState CurrentPlayer = P1;

//determines the game pointer orientation (for DEPLOYP1 state and GAME state)
//if the length of the pointer is 1, H orientation goes the same as V orientation
enum PointerOrientation{H,V};
PointerOrientation CurrentOri = H;

//enum for tile appearances
enum Tile{MISS,HIT,DESTROYED};

//Visual pointer of the game
SDL_Surface *Pointer = NULL;
int X = 50;
int Y = 100;
int XVel = 0;
int YVel = 0;

//Stores the grid position of the clicked grid
Point ClickedPosition = Point(0,0);

//handle the pointer movement
void Move(int BlockLength, PlayerState CurrentPlayer, PointerOrientation CurrentOri)
{
	int LeftBound;
	int RightBound;
	int UpBound;
	int DownBound;

	//set the boundaries for each player's grid
	if (CurrentPlayer == P1)
	{
		LeftBound = 50;
		RightBound = 450;
		UpBound = 100;
		DownBound = 500;
	}
	if (CurrentPlayer == P2)
	{
		LeftBound = 574;
		RightBound = 974;
		UpBound = 100;
		DownBound = 500;
	}

	//set the boundaries for each pointer orientation
	if (CurrentOri == H)
	{
		//Move the pointer left or right
		X += XVel;

		//If the position is too far to the left or right
		if( ( X < LeftBound ) || ( X + (BlockLength*40) > RightBound ) )
		{
			//move back
			X -= XVel;
		}

		//Move the pointer up or down
		Y += YVel;

		//If the position is too far up or down
		if( ( Y < UpBound ) || ( Y + 40 > DownBound ) )
		{
			//move back
			Y -= YVel;
		}
	}
	if (CurrentOri == V)
	{
		//Move the pointer left or right
		X += XVel;

		//If the position is too far to the left or right
		if( ( X < LeftBound ) || ( X + 40 > RightBound ) )
		{
			//move back
			X -= XVel;
		}

		//Move the pointer up or down
		Y += YVel;

		//If the position is too far up or down
		if( ( Y < UpBound ) || ( Y + (BlockLength*40) > DownBound ) )
		{
			//move back
			Y -= YVel;
		}
	}
}

//The button
class Button
{
    private:

	//The name of the button
	string Name;

    //The attributes of the button
    SDL_Rect box;

	//The clip regions of the sprite sheet
	SDL_Rect clips[ 4 ];

    //The part of the button sprite sheet that will be shown
    SDL_Rect* clip;

    public:
    //Initialize the variables
    Button( int x, int y, int w, int h, string NewName);

    //Handles events and set the button's sprite region
    void handle_events();

    //Shows the button on the screen
    void show();

	//Clip the button
	void set_clips(int clip_const, int x, int y, int w, int h);
};

SDL_Surface *load_image( std::string filename )
{
    //The image that's loaded
    SDL_Surface* loadedImage = NULL;

    //The optimized surface that will be used
    SDL_Surface* optimizedImage = NULL;

    //Load the image
    loadedImage = IMG_Load( filename.c_str() );

    //If the image loaded
    if( loadedImage != NULL )
    {
        //Create an optimized surface
        optimizedImage = SDL_DisplayFormat( loadedImage );

        //Free the old surface
        SDL_FreeSurface( loadedImage );

        //If the surface was optimized
        if( optimizedImage != NULL )
        {
            //Color key surface
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 0xFF, 0, 0xFF ) );
        }
    }

    //Return the optimized surface
    return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL )
{
    //Holds offsets
    SDL_Rect offset;

    //Get offsets
    offset.x = x;
    offset.y = y;

    //Blit
    SDL_BlitSurface( source, clip, destination, &offset );
}

//using SDL's software rendering
//used in all state except GALLERY state
bool init()
{
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }

    //Set up the screen
	screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

    //If there was an error in setting up the screen
    if( screen == NULL )
    {
        return false;
    }

	//Init SDL ttf
	if(TTF_Init() == -1)
	{
		return false;
	}

    //Set the window caption
    SDL_WM_SetCaption( "Battleship", NULL );

    //If everything initialized fine
    return true;
}

//using OpenGL's hardware accelerated rendering
//used in GALLERY state
bool initGL()
{
	glClearColor(0.0, 0.0, 0.0, 0.0); // This clear the background color to black
	glShadeModel(GL_SMOOTH); // Type of shading for the polygons

    //Initialize Projection Matrix
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

	gluPerspective(45.0f,(GLfloat)SCREEN_WIDTH/(GLfloat)SCREEN_HEIGHT,10.0f,10000.0f);

	glEnable(GL_DEPTH_TEST); // We enable the depth test (also called z buffer)
    glPolygonMode (GL_FRONT_AND_BACK, GL_LINE); 
	
	CThreeMaxLoader::Load3DS(&object,"model/17_Rowboat.3ds");

    ////Initialize Modelview Matrix
    //glMatrixMode( GL_MODELVIEW );
    //glLoadIdentity();

    //Check for error
    GLenum error = glGetError();
    if( error != GL_NO_ERROR )
    {
        printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
        return false;
    }

    return true;
}

bool initGallery()
{
    //Initialize SDL
    if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
    {
        return false;
    }

    //Create Window
    if( SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_OPENGL ) == NULL )
    {
        return false;
    }

    ////Enable unicode
    //SDL_EnableUNICODE( SDL_TRUE );

    //Initialize OpenGL
    if( initGL() == false )
    {
        return false;
    }

    //Set caption
    SDL_WM_SetCaption( "Battleship", NULL );

    return true;
}
void update()
{

}

void render()
{
	int l_index;

    //Clear color buffer
	glClear(GL_COLOR_BUFFER_BIT |  GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW); // Modeling transformation
	glLoadIdentity();

	glTranslatef(0.0,-150.0,-600.0);
	glColor3d(1,1,0);
    glRotatef(rotation_x,1.0,0.0,0.0); // Rotations of the object (the model matrix is multiplied by the rotation matrices)
    glRotatef(rotation_y,0.0,1.0,0.0);
    glRotatef(rotation_z,0.0,0.0,1.0);

	rotation_x = rotation_x + rotation_x_increment;
    rotation_y = rotation_y + rotation_y_increment;
    //rotation_z = rotation_z + rotation_z_increment;

	if (rotation_x > 359) rotation_x = 0;
	if (rotation_x < 0) rotation_x = 359;
    if (rotation_y > 359) rotation_y = 0;
	if (rotation_y < 0) rotation_y = 359;
    if (rotation_z > 359) rotation_z = 0;

    glBegin(GL_TRIANGLES); // glBegin and glEnd delimit the vertices that define a primitive (in our case triangles)
    for (l_index=0;l_index<object.polygons_qty;l_index++)
    {
        //----------------- FIRST VERTEX -----------------
        // Coordinates of the first vertex
        glVertex3f( object.vertex[ object.polygon[l_index].a ].x,
                    object.vertex[ object.polygon[l_index].a ].y,
                    object.vertex[ object.polygon[l_index].a ].z); //Vertex definition

        //----------------- SECOND VERTEX -----------------
        // Coordinates of the second vertex
		//float x= object.vertex[ object.polygon[l_index].b ].x;

        glVertex3f( object.vertex[ object.polygon[l_index].b ].x,
                    object.vertex[ object.polygon[l_index].b ].y,
                    object.vertex[ object.polygon[l_index].b ].z);
        
        //----------------- THIRD VERTEX -----------------
        // Coordinates of the Third vertex
        glVertex3f( object.vertex[ object.polygon[l_index].c ].x,
                    object.vertex[ object.polygon[l_index].c ].y,
                    object.vertex[ object.polygon[l_index].c ].z);
    }
    glEnd();

    //Update screen
    SDL_GL_SwapBuffers();
}


bool load_files()
{
	//Load the background
	background = load_image( "image/background.jpg" );
	background2 = load_image( "image/background2.jpg" );
	comment_dummy = load_image("image/comment_dummy.jpg");

	//Load the font
	font = TTF_OpenFont ("font/TREAMD.ttf",50);
	font2 = TTF_OpenFont ("font/Arial.ttf",30);
	font3 = TTF_OpenFont ("font/Arial.ttf",20);

	//If the background didn't load
    if( background == NULL )
    {
        return false;
    }

	if( background2 == NULL )
    {
        return false;
    }

	if( font == NULL )
    {
        return false;
    }

	if( font2 == NULL )
    {
        return false;
    }

	if( font3 == NULL )
    {
        return false;
    }

	 //Load the button sprite sheet
    buttonSheet = load_image( "image/button.png" );

    //If there was a problem in loading the button sprite sheet
    if( buttonSheet == NULL )
    {
        return false;
    }

	//Load the game element
	//Load the grid
	grid = load_image( "image/Grid.jpg" );
	if( grid == NULL )
    {
        return false;
    }

	//load the P1 and P2 indicator for the grid
	P1Indicator = load_image ( "image/P1.png" );
	if( P1Indicator == NULL )
    {
        return false;
    }

	P2Indicator = load_image ( "image/P2.png" );
	if( P2Indicator == NULL )
    {
        return false;
    }

	//load the pointer used in deployment state
	Pointer = load_image("image/5Blocks_H.png");
	if ( Pointer == NULL )
	{
		return false;
	}

	//load the tiles (hit,miss,destroyed ship)
	tile_miss = load_image("image/tile_miss.jpg");
	if ( tile_miss == NULL )
	{
		return false;
	}

	tile_hit = load_image("image/tile_hit.jpg");
	if ( tile_hit == NULL )
	{
		return false;
	}

	tile_x = load_image("image/tile_x.png");
	if ( tile_x == NULL )
	{
		return false;
	}

    //If everything loaded fine
    return true;
}

void clean_up()
{
    //Free the surface
	SDL_FreeSurface(background);
	SDL_FreeSurface(background2);
    SDL_FreeSurface( buttonSheet );
	SDL_FreeSurface( title );
	SDL_FreeSurface( comment );
	SDL_FreeSurface( comment2 );
	SDL_FreeSurface( comment_dummy );
	SDL_FreeSurface( grid );
	SDL_FreeSurface( P1Indicator );
	SDL_FreeSurface( P2Indicator );
	SDL_FreeSurface( Pointer);
	SDL_FreeSurface( ship5 );
	SDL_FreeSurface( ship4a );
	SDL_FreeSurface( ship4b );
	SDL_FreeSurface( ship3 );
	SDL_FreeSurface( ship2 );
	SDL_FreeSurface( tile_miss );
	SDL_FreeSurface( tile_hit );
	SDL_FreeSurface( tile_x );
	SDL_FreeSurface( screen );
    //Quit SDL
    SDL_Quit();
}

void Button::set_clips(int clip_const, int x, int y, int w, int h)
{
    //Clip the sprite sheet
    clips[ clip_const ].x = x;
    clips[ clip_const ].y = y;
    clips[ clip_const ].w = w;
    clips[ clip_const ].h = h;
}

Button::Button( int x, int y, int w, int h, string NewName)
{
	//Set the button's name
	Name  = NewName;
    //Set the button's attributes
    box.x = x;
    box.y = y;
    box.w = w;
    box.h = h;

    //Set the default sprite
    clip = &clips[ CLIP_MOUSEOUT ];
}

void Button::handle_events()
{
    //The mouse offsets
    int x = 0, y = 0;

    //If the mouse moved
    if( event.type == SDL_MOUSEMOTION )
    {
        //Get the mouse offsets
        x = event.motion.x;
        y = event.motion.y;

        //If the mouse is over the button
        if( ( x > box.x ) && ( x < box.x + box.w ) && ( y > box.y ) && ( y < box.y + box.h ) )
        {
            //Set the button sprite
            clip = &clips[ CLIP_MOUSEOVER ];
        }
        //If not
        else
        {
            //Set the button sprite
            clip = &clips[ CLIP_MOUSEOUT ];
        }
    }
    //If a mouse button was pressed
    if( event.type == SDL_MOUSEBUTTONDOWN )
    {
        //If the left mouse button was pressed
        if( event.button.button == SDL_BUTTON_LEFT )
        {
            //Get the mouse offsets
            x = event.button.x;
            y = event.button.y;

            //If the mouse is over the button
            if( ( x > box.x ) && ( x < box.x + box.w ) && ( y > box.y ) && ( y < box.y + box.h ) )
            {
                //Set the button sprite
                clip = &clips[ CLIP_MOUSEDOWN ];
            }
        }
    }
    //If a mouse button was released
    if( event.type == SDL_MOUSEBUTTONUP )
    {
        //If the left mouse button was released
        if( event.button.button == SDL_BUTTON_LEFT )
        {
            //Get the mouse offsets
            x = event.button.x;
            y = event.button.y;

            //If the mouse is over the button
            if( ( x > box.x ) && ( x < box.x + box.w ) && ( y > box.y ) && ( y < box.y + box.h ) )
            {
                //Set the button sprite
                clip = &clips[ CLIP_MOUSEUP ];
				if(Name == "ButtonQuit")
				{
					quit = true;
				}
				if(Name == "ButtonPlay")
				{
					CurrentState = DEPLOYP1;
				}
				if((Name == "ButtonGallery") && (CurrentState == GALLERY_INTRO))
				{
					CurrentState = GALLERY;
				}
				if((Name == "ButtonGallery") && (CurrentState == WELCOME_SCREEN))
				{
					CurrentState = GALLERY_INTRO;
				}
				if(Name == "ButtonMainMenu")
				{
					CurrentState = WELCOME_SCREEN;
				}
            }
        }
    }
}

//Handle pointer movement in DEPLOYP1 and DEPLOYP2 state
void HandlePointer(int BlockLength)
{
	//handle the pointer key event
	if( event.type == SDL_KEYDOWN )
	{
		//Adjust the velocity
		switch( event.key.keysym.sym )
		{
			case SDLK_UP: YVel -= 40; break;
			case SDLK_DOWN: YVel += 40; break;
			case SDLK_LEFT: XVel -= 40; break;
			case SDLK_RIGHT: XVel += 40; break;
			case SDLK_SPACE: if(CurrentOri == H)
							 {
								 CurrentOri = V;
								 switch(BlockLength)
								 {
									 case 5: Pointer = load_image("image/5Blocks_V.png") ;break;
									 case 4: Pointer = load_image("image/4Blocks_V.png") ;break;
									 case 3: Pointer = load_image("image/3Blocks_V.png") ;break;
									 case 2: Pointer = load_image("image/2Blocks_V.png") ;break;
								 }
							 }
							 else 
							 {
								 CurrentOri = H;
								 switch(BlockLength)
								 {
									 case 5: Pointer = load_image("image/5Blocks_H.png") ;break;
									 case 4: Pointer = load_image("image/4Blocks_H.png") ;break;
									 case 3: Pointer = load_image("image/3Blocks_H.png") ;break;
									 case 2: Pointer = load_image("image/2Blocks_H.png") ;break;
								 }
							 };
							 break;
		}
	}
	//If a key was released
	else if( event.type == SDL_KEYUP )
	{
		//Adjust the velocity
		switch( event.key.keysym.sym )
		{
			case SDLK_UP: YVel += 40; break;
			case SDLK_DOWN: YVel -= 40; break;
			case SDLK_LEFT: XVel += 40; break;
			case SDLK_RIGHT: XVel -= 40; break;
		}
	}
}

//Get the block position of the clicked grid
Point GetClickedGridPos(int XPixel, int YPixel)
{
	Point P = Point(77,77);

	if(CurrentPlayer == P2)
	{
		//convert XPixel to grid's X position
		if ((XPixel>=50)&&(XPixel<90)) { P.SetAbsis(1);}
		if ((XPixel>=90)&&(XPixel<130)) { P.SetAbsis(2);}
		if ((XPixel>=130)&&(XPixel<170)) { P.SetAbsis(3);}
		if ((XPixel>=170)&&(XPixel<210)) { P.SetAbsis(4);}
		if ((XPixel>=210)&&(XPixel<250)) { P.SetAbsis(5);}
		if ((XPixel>=250)&&(XPixel<290)) { P.SetAbsis(6);}
		if ((XPixel>=290)&&(XPixel<330)) { P.SetAbsis(7);}
		if ((XPixel>=330)&&(XPixel<370)) { P.SetAbsis(8);}
		if ((XPixel>=370)&&(XPixel<410)) { P.SetAbsis(9);}
		if ((XPixel>=410)&&(XPixel<450)) { P.SetAbsis(10);}
	}
	else //if(CurrentPlayer == P1)
	{
		if ((XPixel>=574)&&(XPixel<614)) { P.SetAbsis(1);}
		if ((XPixel>=614)&&(XPixel<654)) { P.SetAbsis(2);}
		if ((XPixel>=654)&&(XPixel<694)) { P.SetAbsis(3);}
		if ((XPixel>=694)&&(XPixel<734)) { P.SetAbsis(4);}
		if ((XPixel>=734)&&(XPixel<774)) { P.SetAbsis(5);}
		if ((XPixel>=774)&&(XPixel<814)) { P.SetAbsis(6);}
		if ((XPixel>=814)&&(XPixel<854)) { P.SetAbsis(7);}
		if ((XPixel>=854)&&(XPixel<894)) { P.SetAbsis(8);}
		if ((XPixel>=894)&&(XPixel<934)) { P.SetAbsis(9);}
		if ((XPixel>=934)&&(XPixel<974)) { P.SetAbsis(10);}
	}

	//convert XPixel to grid's X position
	if ((YPixel>=100)&&(YPixel<140)) { P.SetOrdinat(1);}
	if ((YPixel>=140)&&(YPixel<180)) { P.SetOrdinat(2);}
	if ((YPixel>=180)&&(YPixel<220)) { P.SetOrdinat(3);}
	if ((YPixel>=220)&&(YPixel<260)) { P.SetOrdinat(4);}
	if ((YPixel>=260)&&(YPixel<300)) { P.SetOrdinat(5);}
	if ((YPixel>=300)&&(YPixel<340)) { P.SetOrdinat(6);}
	if ((YPixel>=340)&&(YPixel<380)) { P.SetOrdinat(7);}
	if ((YPixel>=380)&&(YPixel<420)) { P.SetOrdinat(8);}
	if ((YPixel>=420)&&(YPixel<460)) { P.SetOrdinat(9);}
	if ((YPixel>=460)&&(YPixel<500)) { P.SetOrdinat(10);}

	return P;
}

//Checks whether a shoot to a block is valid
//Returns true if the player "shoot" a block that has not been hit
bool IsShootValid(int XPixel, int YPixel, Grid G)
{
	Point P = Point(0,0);
	//Determines if the clicking point is out of grid boundaries
	bool OutOfBound = true;
	//Determines if the block had ever been hit
	bool EverHit = false;
	//the boundaries of the grid
	int UpBound = 100;
	int DownBound = 500;
	int LeftBound = 0;
	int RightBound = 0;
	if(CurrentPlayer == P2)
	{
		LeftBound = 50;
		RightBound = 450;
	}
	else //if(CurrentPlayer == P1)
	{
		LeftBound = 574;
		RightBound = 974;
	}

	//checks if the clicked area is in the grid
	if ((XPixel >= LeftBound) && (XPixel <= RightBound) && (YPixel >= UpBound) && (YPixel <= DownBound))
	{
		OutOfBound = false;
	}

	if (OutOfBound == false)
	{
		P = GetClickedGridPos(XPixel, YPixel);
		//checks if the block had ever been hit
		if(G.GetHitStatus(P.GetAbsis(), P.GetOrdinat()) == true)
		{
			EverHit = true;
		}
	}

	return ((!(OutOfBound)) && (!(EverHit)));
}

void Button::show()
{
    //Show the button
    apply_surface( box.x, box.y, buttonSheet, screen, clip );
}

//Validity checking for the ship deployment
//X and Y represents the position of the ship's head in pixel
bool IsDeployValid(int XPointer, int YPointer, int BlockLength, Grid G)
{
	bool Valid = true;
	bool Occupied = false;

	//Grid boundaries for each player
	int RightBound;
	int DownBound = 500;

	//Convert X and Y position to block
	int XBlock;
	int YBlock = (Y-60)/40;
	if(CurrentPlayer == P1)
	{
		XBlock = (X-10)/40;
		RightBound = 450;
	}
	else
	{
		XBlock = (X-534)/40;
		RightBound = 974;
	}

	//Check if the ship is out of area boundaries
	if(CurrentOri == H)
	{
		if (XPointer+(BlockLength*40) > RightBound)
		{
			Valid = false;
		}
	}
	else
	{
		if (YPointer+(BlockLength*40) > DownBound)
		{
			Valid = false;
		}
	}

	if (Valid == true)
	{
		//Traverse through the grid, checking validity
		if(CurrentOri == H)
		{
			for (int i = XBlock; i < XBlock+BlockLength; i++)
			{
				//if a block has been occupied by another ship
				if(G.GetOccupationalStatus(i,YBlock) == true)
				{
					Occupied = true;
					break;
				}
			}
		}
		else //if(CurrentOri == V)
		{
			for (int i = YBlock; i < YBlock+BlockLength; i++)
			{
				//if a block has been occupied by another ship
				if(G.GetOccupationalStatus(XBlock,i) == true)
				{
					Occupied = true;
					break;
				}
			}
		}
	}
	return (Valid && !(Occupied));
}

//apply the surface for the tile_miss
void ApplyTileSurface(Point P, Tile TileStatus)
{
	int XPixel = 0;
	int YPixel = 0;

	if(CurrentPlayer == P2)
	{
		XPixel = 10 + (P.GetAbsis() * 40);
	}
	else //if(CurrentPlayer == P1)
	{
		XPixel = 534 + (P.GetAbsis() * 40);
	}

	YPixel = 60 + (P.GetOrdinat() * 40);

	if(TileStatus == MISS)
	{
		apply_surface(XPixel, YPixel, tile_miss, screen);
	}
	else
	{
		if(TileStatus == HIT)
		{
			apply_surface(XPixel, YPixel, tile_hit, screen);
		}
		else //if(TileStatus == DESTROYED)
		{
			apply_surface(XPixel, YPixel, tile_x, screen);
		}
	}
}

//replace the tile with the x tile if a ship was destroyed
void DestroyShipTile(Ship S)
{
	for(int i = 0; i < S.GetLength(); i++)
	{
		ApplyTileSurface(S.GetPosition(i), DESTROYED);
	}
}

int main( int argc, char* args[] )
{
	//Initialize the playing area, which is the grid
	Grid AreaP1 = Grid();
	Grid AreaP2 = Grid();

	//Initialize the ships in the game
	Ship ShipP15 = Ship("P1 Shipyard", 5);
	Ship ShipP14a = Ship("P1 Battleship", 4);
	Ship ShipP14b = Ship("P1 Battleship", 4);
	Ship ShipP13 = Ship("P1 Destroyer Ship", 3);
	Ship ShipP12 = Ship("P1 Transport Ship", 2);

	Ship ShipP25 = Ship("P2 Shipyard", 5);
	Ship ShipP24a = Ship("P2 Battleship", 4);
	Ship ShipP24b = Ship("P2 Battleship", 4);
	Ship ShipP23 = Ship("P2 Destroyer Ship", 3);
	Ship ShipP22 = Ship("P2 Transport Ship", 2);

	SDLINIT:
    //Initialize
    if( init() == false )
    {
        return 1;
    }

    //Load the files
    if( load_files() == false )
    {
        return 1;
    }
	
	//Construct the buttons
	Button ButtonPlay( 784, 342, 200, 50, "ButtonPlay");
	Button ButtonGallery( 784, 417, 200, 50, "ButtonGallery");
	Button ButtonQuit( 784, 492, 200, 50, "ButtonQuit");
	Button ButtonMainMenu( 784, 520, 200, 50, "ButtonMainMenu");

	//Clip the sprite sheet for Button Play
	ButtonPlay.set_clips(CLIP_MOUSEOVER, 600, 0, 200, 50);
	ButtonPlay.set_clips(CLIP_MOUSEOUT, 400, 0, 200, 50);
	ButtonPlay.set_clips(CLIP_MOUSEDOWN, 600, 0, 200, 50);
	ButtonPlay.set_clips(CLIP_MOUSEUP, 600, 0, 200, 50);
	//Clip the sprite sheet for Button Gallery
	ButtonGallery.set_clips(CLIP_MOUSEOVER, 1000, 0, 200, 50);
	ButtonGallery.set_clips(CLIP_MOUSEOUT, 800, 0, 200, 50);
	ButtonGallery.set_clips(CLIP_MOUSEDOWN, 1000, 0, 200, 50);
	ButtonGallery.set_clips(CLIP_MOUSEUP, 1000, 0, 200, 50);
	//Clip the sprite sheet for Button Quit
	ButtonQuit.set_clips(CLIP_MOUSEOVER, 1400, 0, 200, 50);
	ButtonQuit.set_clips(CLIP_MOUSEOUT, 1200, 0, 200, 50);
	ButtonQuit.set_clips(CLIP_MOUSEDOWN, 1400, 0, 200, 50);
	ButtonQuit.set_clips(CLIP_MOUSEUP, 1400, 0, 200, 50);
	//Clip the sprite sheet for Button MainMenu
	ButtonMainMenu.set_clips(CLIP_MOUSEOVER, 200, 0, 200, 50);
	ButtonMainMenu.set_clips(CLIP_MOUSEOUT, 0, 0, 200, 50);
	ButtonMainMenu.set_clips(CLIP_MOUSEDOWN, 200, 0, 200, 50);
	ButtonMainMenu.set_clips(CLIP_MOUSEUP, 200, 0, 200, 50);

	//Render the title and error handling
	title = TTF_RenderText_Solid( font, "BATTLESHIP", textColor_Red);
	if ( title == NULL)
	{
		return 1;
	}
	
	comment = TTF_RenderText_Solid( font2, "Okay Player 1, deploy your unit!", textColor_White);
	comment2 = TTF_RenderText_Solid( font3, "Position your unit using arrow keys, Press [Space] to change unit's orientation, Press [Enter] to deploy the unit", textColor_White);

    //While the user hasn't quit
    while( quit == false )
    {
		while(CurrentState == WELCOME_SCREEN)
		{
			//If there's events to handle
			if( SDL_PollEvent( &event ) )
			{
				//If the user has Xed out the window
				if( event.type == SDL_QUIT )
				{
					//Quit the program
					quit = true;
				}

				//Handle button events
				ButtonPlay.handle_events();
				ButtonGallery.handle_events();
				ButtonQuit.handle_events();
				if (quit == true)
				{
					break;
				}
			}

			//Apply the surfaces to the screen
			apply_surface( 0, 0, background, screen );
			apply_surface(600, 70, title, screen );

			//Show the button (apply button "surface")
			ButtonPlay.show();
			ButtonGallery.show();
			ButtonQuit.show();

			//Update the screen
			SDL_Flip(screen);
		}

		while(CurrentState == DEPLOYP1)
		{
			//The 5-block-length ship deployment
			while(!(Fixed5))
			{
				//If there's events to handle
				if( SDL_PollEvent( &event ) )
				{
					//If the user has Xed out the window
					if( event.type == SDL_QUIT )
					{
						//Quit the program
						quit = true;
						break;
					}

					//Handle button events
					ButtonMainMenu.handle_events();
					if (CurrentState == WELCOME_SCREEN)
					{
						break;
					}

					//handle pointer movement
					HandlePointer(5);

					//Place the ship
					if( event.type == SDL_KEYDOWN )
					{
						if(event.key.keysym.sym == SDLK_RETURN)
						{
							if(!(IsDeployValid(X,Y,5,AreaP1)))
							{
								comment = TTF_RenderText_Solid( font2, "Invalid deployment", textColor_White);
							}
							//if valid
							else
							{
								HeadP15.SetAbsis(X);
								HeadP15.SetOrdinat(Y);
								if (CurrentOri == H)
								{
									ship5 = load_image("image/5Blocks_H.png");
									//set the deployment info to the Grid and Ship object
									AreaP1.OccupyGrid(X,Y,5,'H',"P1");
									ShipP15.SetPartPosition(X,Y,'H',"P1");
								}
								else
								{
									ship5 = load_image("image/5Blocks_V.png");
									AreaP1.OccupyGrid(X,Y,5,'V',"P1");
									ShipP15.SetPartPosition(X,Y,'V',"P1");
								}
								Fixed5 = true;
							}
						}
					}
				}

				//ngetes DEPLOYP1
				if (CurrentOri == H)
				{
					Move(5,P1,H);
				}
				if (CurrentOri == V)
				{
					Move(5,P1,V);
				}

				//Apply the surfaces to the screen
				apply_surface( 0, 0, background2, screen );
				apply_surface( 390, 0, comment, screen );
				apply_surface( 0, 40, comment2, screen );
				apply_surface( 50,100,grid,screen );
				apply_surface( 574,100,grid,screen );
				apply_surface( 50,500,P1Indicator,screen );
				apply_surface( 574,500,P2Indicator,screen );

				apply_surface( X,Y,Pointer,screen );

				//Show the button (apply button "surface")
				ButtonMainMenu.show();

				//Update the screen
				SDL_Flip(screen);
			}

			if (CurrentState == WELCOME_SCREEN)
			{
				break;
			}
			if (quit == true)
			{
				break;
			}

			//reset
			CurrentOri = H;
			X = 50;
			Y = 100;
			Pointer = load_image("image/4Blocks_H.png");
			//The 4-block-length ship deployment
			while(!(Fixed4a))
			{
				//If there's events to handle
				if( SDL_PollEvent( &event ) )
				{
					//If the user has Xed out the window
					if( event.type == SDL_QUIT )
					{
						//Quit the program
						quit = true;
						break;
					}

					//Handle button events
					ButtonMainMenu.handle_events();
					if (CurrentState == WELCOME_SCREEN)
					{
						break;
					}

					//handle pointer movement
					HandlePointer(4);

					//Place the ship
					if( event.type == SDL_KEYDOWN )
					{
						if(event.key.keysym.sym == SDLK_RETURN)
						{
							if(!(IsDeployValid(X,Y,4,AreaP1)))
							{
								comment = TTF_RenderText_Solid( font2, "Invalid deployment", textColor_White);
							}
							//if valid
							else
							{
								HeadP14a.SetAbsis(X);
								HeadP14a.SetOrdinat(Y);
								if (CurrentOri == H)
								{
									ship4a = load_image("image/4Blocks_H.png");
									//set the deployment info to the Grid and Ship object
									AreaP1.OccupyGrid(X,Y,4,'H',"P1");
									ShipP14a.SetPartPosition(X,Y,'H',"P1");
								}
								if (CurrentOri == V)
								{
									ship4a = load_image("image/4Blocks_V.png");
									AreaP1.OccupyGrid(X,Y,4,'V',"P1");
									ShipP14a.SetPartPosition(X,Y,'V',"P1");
								}
								Fixed4a = true;
							}
						}
					}
				}

				//ngetes DEPLOYP1
				if (CurrentOri == H)
				{
					Move(4,P1,H);
				}
				if (CurrentOri == V)
				{
					Move(4,P1,V);
				}

				//Apply the surfaces to the screen
				apply_surface( 0, 0, background2, screen );
				apply_surface( 390, 0, comment, screen );
				apply_surface( 0, 40, comment2, screen );
				apply_surface( 50,100,grid,screen );
				apply_surface( 574,100,grid,screen );
				apply_surface( 50,500,P1Indicator,screen );
				apply_surface( 574,500,P2Indicator,screen );
				apply_surface ( HeadP15.GetAbsis(),HeadP15.GetOrdinat(),ship5,screen);

				apply_surface( X,Y,Pointer,screen );

				//Show the button (apply button "surface")
				ButtonMainMenu.show();

				//Update the screen
				SDL_Flip(screen);
			}

			if (CurrentState == WELCOME_SCREEN)
			{
				break;
			}
			if (quit == true)
			{
				break;
			}

			//reset
			CurrentOri = H;
			X = 50;
			Y = 100;
			Pointer = load_image("image/4Blocks_H.png");
			//The 4-block-length ship deployment
			while(!(Fixed4b))
			{
				//If there's events to handle
				if( SDL_PollEvent( &event ) )
				{
					//If the user has Xed out the window
					if( event.type == SDL_QUIT )
					{
						//Quit the program
						quit = true;
						break;
					}

					//Handle button events
					ButtonMainMenu.handle_events();
					if (CurrentState == WELCOME_SCREEN)
					{
						break;
					}

					//handle pointer movement
					HandlePointer(4);

					//Place the ship
					if( event.type == SDL_KEYDOWN )
					{
						if(event.key.keysym.sym == SDLK_RETURN)
						{
							if(!(IsDeployValid(X,Y,4,AreaP1)))
							{
								comment = TTF_RenderText_Solid( font2, "Invalid deployment", textColor_White);
							}
							//if valid
							else
							{
								HeadP14b.SetAbsis(X);
								HeadP14b.SetOrdinat(Y);
								if (CurrentOri == H)
								{
									ship4b = load_image("image/4Blocks_H.png");
									//set the deployment info to the Grid and Ship object
									AreaP1.OccupyGrid(X,Y,4,'H',"P1");
									ShipP14b.SetPartPosition(X,Y,'H',"P1");
								}
								if (CurrentOri == V)
								{
									ship4b = load_image("image/4Blocks_V.png");
									AreaP1.OccupyGrid(X,Y,4,'V',"P1");
									ShipP14b.SetPartPosition(X,Y,'V',"P1");
								}
								Fixed4b = true;
							}
						}
					}
				}

				//ngetes DEPLOYP1
				if (CurrentOri == H)
				{
					Move(4,P1,H);
				}
				if (CurrentOri == V)
				{
					Move(4,P1,V);
				}

				//Apply the surfaces to the screen
				apply_surface( 0, 0, background2, screen );
				apply_surface( 390, 0, comment, screen );
				apply_surface( 0, 40, comment2, screen );
				apply_surface( 50,100,grid,screen );
				apply_surface( 574,100,grid,screen );
				apply_surface( 50,500,P1Indicator,screen );
				apply_surface( 574,500,P2Indicator,screen );
				apply_surface ( HeadP15.GetAbsis(),HeadP15.GetOrdinat(),ship5,screen);
				apply_surface ( HeadP14a.GetAbsis(),HeadP14a.GetOrdinat(),ship4a,screen);

				apply_surface( X,Y,Pointer,screen );

				//Show the button (apply button "surface")
				ButtonMainMenu.show();

				//Update the screen
				SDL_Flip(screen);
			}

			if (CurrentState == WELCOME_SCREEN)
			{
				break;
			}
			if (quit == true)
			{
				break;
			}

			//reset
			CurrentOri = H;
			X = 50;
			Y = 100;
			Pointer = load_image("image/3Blocks_H.png");
			//The 3-block-length ship deployment
			while(!(Fixed3))
			{
				//If there's events to handle
				if( SDL_PollEvent( &event ) )
				{
					//If the user has Xed out the window
					if( event.type == SDL_QUIT )
					{
						//Quit the program
						quit = true;
						break;
					}

					//Handle button events
					ButtonMainMenu.handle_events();
					if (CurrentState == WELCOME_SCREEN)
					{
						break;
					}

					//handle pointer movement
					HandlePointer(3);

					//Place the ship
					if( event.type == SDL_KEYDOWN )
					{
						if(event.key.keysym.sym == SDLK_RETURN)
						{
							if(!(IsDeployValid(X,Y,3,AreaP1)))
							{
								comment = TTF_RenderText_Solid( font2, "Invalid deployment", textColor_White);
							}
							//if valid
							else
							{
								HeadP13.SetAbsis(X);
								HeadP13.SetOrdinat(Y);
								if (CurrentOri == H)
								{
									ship3 = load_image("image/3Blocks_H.png");
									//set the deployment info to the Grid and Ship object
									AreaP1.OccupyGrid(X,Y,3,'H',"P1");
									ShipP13.SetPartPosition(X,Y,'H',"P1");
								}
								if (CurrentOri == V)
								{
									ship3 = load_image("image/3Blocks_V.png");
									AreaP1.OccupyGrid(X,Y,3,'V',"P1");
									ShipP13.SetPartPosition(X,Y,'V',"P1");
								}
								Fixed3 = true;
							}
						}
					}
				}

				//ngetes DEPLOYP1
				if (CurrentOri == H)
				{
					Move(3,P1,H);
				}
				if (CurrentOri == V)
				{
					Move(3,P1,V);
				}

				//Apply the surfaces to the screen
				apply_surface( 0, 0, background2, screen );
				apply_surface( 390, 0, comment, screen );
				apply_surface( 0, 40, comment2, screen );
				apply_surface( 50,100,grid,screen );
				apply_surface( 574,100,grid,screen );
				apply_surface( 50,500,P1Indicator,screen );
				apply_surface( 574,500,P2Indicator,screen );
				apply_surface ( HeadP15.GetAbsis(),HeadP15.GetOrdinat(),ship5,screen);
				apply_surface ( HeadP14a.GetAbsis(),HeadP14a.GetOrdinat(),ship4a,screen);
				apply_surface ( HeadP14b.GetAbsis(),HeadP14b.GetOrdinat(),ship4b,screen);

				apply_surface( X,Y,Pointer,screen );

				//Show the button (apply button "surface")
				ButtonMainMenu.show();

				//Update the screen
				SDL_Flip(screen);
			}

			if (CurrentState == WELCOME_SCREEN)
			{
				break;
			}
			if (quit == true)
			{
				break;
			}

			//reset
			CurrentOri = H;
			X = 50;
			Y = 100;
			Pointer = load_image("image/2Blocks_H.png");
			//The 2-block-length ship deployment
			while(!(Fixed2))
			{
				//If there's events to handle
				if( SDL_PollEvent( &event ) )
				{
					//If the user has Xed out the window
					if( event.type == SDL_QUIT )
					{
						//Quit the program
						quit = true;
						break;
					}

					//Handle button events
					ButtonMainMenu.handle_events();
					if (CurrentState == WELCOME_SCREEN)
					{
						break;
					}

					//handle pointer movement
					HandlePointer(2);

					//Place the ship
					if( event.type == SDL_KEYDOWN )
					{
						if(event.key.keysym.sym == SDLK_RETURN)
						{
							if(!(IsDeployValid(X,Y,2,AreaP1)))
							{
								comment = TTF_RenderText_Solid( font2, "Invalid deployment", textColor_White);
							}
							//if valid
							else
							{
								HeadP12.SetAbsis(X);
								HeadP12.SetOrdinat(Y);
								if (CurrentOri == H)
								{
									ship2 = load_image("image/2Blocks_H.png");
									//set the deployment info to the Grid and Ship object
									AreaP1.OccupyGrid(X,Y,2,'H',"P1");
									ShipP12.SetPartPosition(X,Y,'H',"P1");
								}
								if (CurrentOri == V)
								{
									ship2 = load_image("image/2Blocks_V.png");
									AreaP1.OccupyGrid(X,Y,2,'V',"P1");
									ShipP12.SetPartPosition(X,Y,'V',"P1");
								}
								Fixed2 = true;
							}
						}
					}
				}

				//ngetes DEPLOYP1
				if (CurrentOri == H)
				{
					Move(2,P1,H);
				}
				if (CurrentOri == V)
				{
					Move(2,P1,V);
				}

				//Apply the surfaces to the screen
				apply_surface( 0, 0, background2, screen );
				apply_surface( 390, 0, comment, screen );
				apply_surface( 0, 40, comment2, screen );
				apply_surface( 50,100,grid,screen );
				apply_surface( 574,100,grid,screen );
				apply_surface( 50,500,P1Indicator,screen );
				apply_surface( 574,500,P2Indicator,screen );
				apply_surface ( HeadP15.GetAbsis(),HeadP15.GetOrdinat(),ship5,screen);
				apply_surface ( HeadP14a.GetAbsis(),HeadP14a.GetOrdinat(),ship4a,screen);
				apply_surface ( HeadP14b.GetAbsis(),HeadP14b.GetOrdinat(),ship4b,screen);
				apply_surface ( HeadP13.GetAbsis(),HeadP13.GetOrdinat(),ship3,screen);

				apply_surface( X,Y,Pointer,screen );

				//Show the button (apply button "surface")
				ButtonMainMenu.show();

				//Update the screen
				SDL_Flip(screen);
			}

			if (CurrentState == WELCOME_SCREEN)
			{
				break;
			}
			if (quit == true)
			{
				break;
			}

			CurrentState = DEPLOYP2;
		}

		//reset for P2 deployment
		CurrentPlayer = P2;
		CurrentOri = H;
		X = 574;
		Y = 100;
		Fixed5 = false;
		Fixed4a = false;
		Fixed4b = false;
		Fixed3 = false;
		Fixed2 = false;
		Pointer = load_image("image/5Blocks_H.png");
		comment = TTF_RenderText_Solid( font2, "Alright Player 2, deploy your unit!", textColor_White);

		while(CurrentState == DEPLOYP2)
		{
			//The 5-block-length ship deployment
			while(!(Fixed5))
			{
				//If there's events to handle
				if( SDL_PollEvent( &event ) )
				{
					//If the user has Xed out the window
					if( event.type == SDL_QUIT )
					{
						//Quit the program
						quit = true;
						break;
					}

					//Handle button events
					ButtonMainMenu.handle_events();
					if (CurrentState == WELCOME_SCREEN)
					{
						break;
					}

					//handle pointer movement
					HandlePointer(5);

					//Place the ship
					if( event.type == SDL_KEYDOWN )
					{
						if(event.key.keysym.sym == SDLK_RETURN)
						{
							if(!(IsDeployValid(X,Y,5,AreaP2)))
							{
								comment = TTF_RenderText_Solid( font2, "Invalid deployment", textColor_White);
							}
							//if valid
							else
							{
								HeadP25.SetAbsis(X);
								HeadP25.SetOrdinat(Y);
								if (CurrentOri == H)
								{
									ship5 = load_image("image/5Blocks_H.png");
									//set the deployment info to the Grid and Ship object
									AreaP2.OccupyGrid(X,Y,5,'H',"P2");
									ShipP25.SetPartPosition(X,Y,'H',"P2");
								}
								else
								{
									ship5 = load_image("image/5Blocks_V.png");
									AreaP2.OccupyGrid(X,Y,5,'V',"P2");
									ShipP25.SetPartPosition(X,Y,'V',"P2");
								}
								Fixed5 = true;
							}
						}
					}
				}

				//ngetes DEPLOYP1
				if (CurrentOri == H)
				{
					Move(5,P2,H);
				}
				if (CurrentOri == V)
				{
					Move(5,P2,V);
				}

				//Apply the surfaces to the screen
				apply_surface( 0, 0, background2, screen );
				apply_surface( 390, 0, comment, screen );
				apply_surface( 0, 40, comment2, screen );
				apply_surface( 50,100,grid,screen );
				apply_surface( 574,100,grid,screen );
				apply_surface( 50,500,P1Indicator,screen );
				apply_surface( 574,500,P2Indicator,screen );

				apply_surface( X,Y,Pointer,screen );

				//Show the button (apply button "surface")
				ButtonMainMenu.show();

				//Update the screen
				SDL_Flip(screen);
			}

			if (CurrentState == WELCOME_SCREEN)
			{
				break;
			}
			if (quit == true)
			{
				break;
			}

			//reset
			CurrentOri = H;
			X = 574;
			Y = 100;
			Pointer = load_image("image/4Blocks_H.png");

			//The 5-block-length ship deployment
			while(!(Fixed4a))
			{
				//If there's events to handle
				if( SDL_PollEvent( &event ) )
				{
					//If the user has Xed out the window
					if( event.type == SDL_QUIT )
					{
						//Quit the program
						quit = true;
						break;
					}

					//Handle button events
					ButtonMainMenu.handle_events();
					if (CurrentState == WELCOME_SCREEN)
					{
						break;
					}

					//handle pointer movement
					HandlePointer(4);

					//Place the ship
					if( event.type == SDL_KEYDOWN )
					{
						if(event.key.keysym.sym == SDLK_RETURN)
						{
							if(!(IsDeployValid(X,Y,4,AreaP2)))
							{
								comment = TTF_RenderText_Solid( font2, "Invalid deployment", textColor_White);
							}
							//if valid
							else
							{
								HeadP24a.SetAbsis(X);
								HeadP24a.SetOrdinat(Y);
								if (CurrentOri == H)
								{
									ship4a = load_image("image/4Blocks_H.png");
									//set the deployment info to the Grid and Ship object
									AreaP2.OccupyGrid(X,Y,4,'H',"P2");
									ShipP24a.SetPartPosition(X,Y,'H',"P2");
								}
								else
								{
									ship4a = load_image("image/4Blocks_V.png");
									AreaP2.OccupyGrid(X,Y,4,'V',"P2");
									ShipP24a.SetPartPosition(X,Y,'V',"P2");
								}
								Fixed4a = true;
							}
						}
					}
				}

				//ngetes DEPLOYP1
				if (CurrentOri == H)
				{
					Move(4,P2,H);
				}
				if (CurrentOri == V)
				{
					Move(4,P2,V);
				}

				//Apply the surfaces to the screen
				apply_surface( 0, 0, background2, screen );
				apply_surface( 390, 0, comment, screen );
				apply_surface( 0, 40, comment2, screen );
				apply_surface( 50,100,grid,screen );
				apply_surface( 574,100,grid,screen );
				apply_surface( 50,500,P1Indicator,screen );
				apply_surface( 574,500,P2Indicator,screen );
				apply_surface ( HeadP25.GetAbsis(),HeadP25.GetOrdinat(),ship5,screen);

				apply_surface( X,Y,Pointer,screen );

				//Show the button (apply button "surface")
				ButtonMainMenu.show();

				//Update the screen
				SDL_Flip(screen);
			}

			if (CurrentState == WELCOME_SCREEN)
			{
				break;
			}
			if (quit == true)
			{
				break;
			}

			//reset
			CurrentOri = H;
			X = 574;
			Y = 100;
			Pointer = load_image("image/4Blocks_H.png");

			//The 5-block-length ship deployment
			while(!(Fixed4b))
			{
				//If there's events to handle
				if( SDL_PollEvent( &event ) )
				{
					//If the user has Xed out the window
					if( event.type == SDL_QUIT )
					{
						//Quit the program
						quit = true;
						break;
					}

					//Handle button events
					ButtonMainMenu.handle_events();
					if (CurrentState == WELCOME_SCREEN)
					{
						break;
					}

					//handle pointer movement
					HandlePointer(4);

					//Place the ship
					if( event.type == SDL_KEYDOWN )
					{
						if(event.key.keysym.sym == SDLK_RETURN)
						{
							if(!(IsDeployValid(X,Y,4,AreaP2)))
							{
								comment = TTF_RenderText_Solid( font2, "Invalid deployment", textColor_White);
							}
							//if valid
							else
							{
								HeadP24b.SetAbsis(X);
								HeadP24b.SetOrdinat(Y);
								if (CurrentOri == H)
								{
									ship4b = load_image("image/4Blocks_H.png");
									//set the deployment info to the Grid and Ship object
									AreaP2.OccupyGrid(X,Y,4,'H',"P2");
									ShipP24b.SetPartPosition(X,Y,'H',"P2");
								}
								else
								{
									ship4b = load_image("image/4Blocks_V.png");
									AreaP2.OccupyGrid(X,Y,4,'V',"P2");
									ShipP24b.SetPartPosition(X,Y,'V',"P2");
								}
								Fixed4b = true;
							}
						}
					}
				}

				//ngetes DEPLOYP1
				if (CurrentOri == H)
				{
					Move(4,P2,H);
				}
				if (CurrentOri == V)
				{
					Move(4,P2,V);
				}

				//Apply the surfaces to the screen
				apply_surface( 0, 0, background2, screen );
				apply_surface( 390, 0, comment, screen );
				apply_surface( 0, 40, comment2, screen );
				apply_surface( 50,100,grid,screen );
				apply_surface( 574,100,grid,screen );
				apply_surface( 50,500,P1Indicator,screen );
				apply_surface( 574,500,P2Indicator,screen );
				apply_surface ( HeadP25.GetAbsis(),HeadP25.GetOrdinat(),ship5,screen);
				apply_surface ( HeadP24a.GetAbsis(),HeadP24a.GetOrdinat(),ship4a,screen);

				apply_surface( X,Y,Pointer,screen );

				//Show the button (apply button "surface")
				ButtonMainMenu.show();

				//Update the screen
				SDL_Flip(screen);
			}

			if (CurrentState == WELCOME_SCREEN)
			{
				break;
			}
			if (quit == true)
			{
				break;
			}

			//reset
			CurrentOri = H;
			X = 574;
			Y = 100;
			Pointer = load_image("image/3Blocks_H.png");

			//The 5-block-length ship deployment
			while(!(Fixed3))
			{
				//If there's events to handle
				if( SDL_PollEvent( &event ) )
				{
					//If the user has Xed out the window
					if( event.type == SDL_QUIT )
					{
						//Quit the program
						quit = true;
						break;
					}

					//Handle button events
					ButtonMainMenu.handle_events();
					if (CurrentState == WELCOME_SCREEN)
					{
						break;
					}

					//handle pointer movement
					HandlePointer(3);

					//Place the ship
					if( event.type == SDL_KEYDOWN )
					{
						if(event.key.keysym.sym == SDLK_RETURN)
						{
							if(!(IsDeployValid(X,Y,3,AreaP2)))
							{
								comment = TTF_RenderText_Solid( font2, "Invalid deployment", textColor_White);
							}
							//if valid
							else
							{
								HeadP23.SetAbsis(X);
								HeadP23.SetOrdinat(Y);
								if (CurrentOri == H)
								{
									ship3 = load_image("image/3Blocks_H.png");
									//set the deployment info to the Grid and Ship object
									AreaP2.OccupyGrid(X,Y,3,'H',"P2");
									ShipP23.SetPartPosition(X,Y,'H',"P2");
								}
								else
								{
									ship3 = load_image("image/3Blocks_V.png");
									AreaP2.OccupyGrid(X,Y,3,'V',"P2");
									ShipP23.SetPartPosition(X,Y,'V',"P2");
								}
								Fixed3 = true;
							}
						}
					}
				}

				//ngetes DEPLOYP1
				if (CurrentOri == H)
				{
					Move(3,P2,H);
				}
				if (CurrentOri == V)
				{
					Move(3,P2,V);
				}

				//Apply the surfaces to the screen
				apply_surface( 0, 0, background2, screen );
				apply_surface( 390, 0, comment, screen );
				apply_surface( 0, 40, comment2, screen );
				apply_surface( 50,100,grid,screen );
				apply_surface( 574,100,grid,screen );
				apply_surface( 50,500,P1Indicator,screen );
				apply_surface( 574,500,P2Indicator,screen );
				apply_surface ( HeadP25.GetAbsis(),HeadP25.GetOrdinat(),ship5,screen);
				apply_surface ( HeadP24a.GetAbsis(),HeadP24a.GetOrdinat(),ship4a,screen);
				apply_surface ( HeadP24b.GetAbsis(),HeadP24b.GetOrdinat(),ship4b,screen);

				apply_surface( X,Y,Pointer,screen );

				//Show the button (apply button "surface")
				ButtonMainMenu.show();

				//Update the screen
				SDL_Flip(screen);
			}

			if (CurrentState == WELCOME_SCREEN)
			{
				break;
			}
			if (quit == true)
			{
				break;
			}

			//reset
			CurrentOri = H;
			X = 574;
			Y = 100;
			Pointer = load_image("image/2Blocks_H.png");

			//The 5-block-length ship deployment
			while(!(Fixed2))
			{
				//If there's events to handle
				if( SDL_PollEvent( &event ) )
				{
					//If the user has Xed out the window
					if( event.type == SDL_QUIT )
					{
						//Quit the program
						quit = true;
						break;
					}

					//Handle button events
					ButtonMainMenu.handle_events();
					if (CurrentState == WELCOME_SCREEN)
					{
						break;
					}

					//handle pointer movement
					HandlePointer(2);

					//Place the ship
					if( event.type == SDL_KEYDOWN )
					{
						if(event.key.keysym.sym == SDLK_RETURN)
						{
							if(!(IsDeployValid(X,Y,2,AreaP2)))
							{
								comment = TTF_RenderText_Solid( font2, "Invalid deployment", textColor_White);
							}
							//if valid
							else
							{
								HeadP22.SetAbsis(X);
								HeadP22.SetOrdinat(Y);
								if (CurrentOri == H)
								{
									ship2 = load_image("image/2Blocks_H.png");
									//set the deployment info to the Grid and Ship object
									AreaP2.OccupyGrid(X,Y,2,'H',"P2");
									ShipP22.SetPartPosition(X,Y,'H',"P2");
								}
								else
								{
									ship2 = load_image("image/2Blocks_V.png");
									AreaP2.OccupyGrid(X,Y,2,'V',"P2");
									ShipP22.SetPartPosition(X,Y,'V',"P2");
								}
								Fixed2 = true;
							}
						}
					}
				}

				//ngetes DEPLOYP1
				if (CurrentOri == H)
				{
					Move(2,P2,H);
				}
				if (CurrentOri == V)
				{
					Move(2,P2,V);
				}

				//Apply the surfaces to the screen
				apply_surface( 0, 0, background2, screen );
				apply_surface( 390, 0, comment, screen );
				apply_surface( 0, 40, comment2, screen );
				apply_surface( 50,100,grid,screen );
				apply_surface( 574,100,grid,screen );
				apply_surface( 50,500,P1Indicator,screen );
				apply_surface( 574,500,P2Indicator,screen );
				apply_surface ( HeadP25.GetAbsis(),HeadP25.GetOrdinat(),ship5,screen);
				apply_surface ( HeadP24a.GetAbsis(),HeadP24a.GetOrdinat(),ship4a,screen);
				apply_surface ( HeadP24b.GetAbsis(),HeadP24b.GetOrdinat(),ship4b,screen);
				apply_surface ( HeadP23.GetAbsis(),HeadP23.GetOrdinat(),ship3,screen);

				apply_surface( X,Y,Pointer,screen );

				//Show the button (apply button "surface")
				ButtonMainMenu.show();

				//Update the screen
				SDL_Flip(screen);
			}

			if (CurrentState == WELCOME_SCREEN)
			{
				break;
			}
			if (quit == true)
			{
				break;
			}

			CurrentState = GAME;
		}

		//reset
		CurrentPlayer = P1;
		apply_surface( 0, 0, background2, screen );
		apply_surface( 50,100,grid,screen );
		apply_surface( 574,100,grid,screen );
		apply_surface( 50,500,P1Indicator,screen );
		apply_surface( 574,500,P2Indicator,screen );
		comment = TTF_RenderText_Solid( font2, "It's P1's turn. Attack the right side area", textColor_White);


		while(CurrentState == GAME)
		{
			//Stores the conversion from clicked position in pixel to clicked position in grid coordinate
			//Point ClickedPosition = Point(0,0);
			int X = event.button.x;
			int Y = event.button.y;

			if(CurrentPlayer == P1)
			{
				//If there's events to handle
				if( SDL_PollEvent( &event ) )
				{
					//If the user has Xed out the window
					if( event.type == SDL_QUIT )
					{
						//Quit the program
						quit = true;
						break;
					}

					//Handle button events
					ButtonMainMenu.handle_events();

					if( event.type == SDL_MOUSEBUTTONUP )
					{
						//If the left mouse button was released
						if( event.button.button == SDL_BUTTON_LEFT )
						{
							ClickedPosition = GetClickedGridPos(X, Y);
							if(!(IsShootValid(X, Y, AreaP2)))
							{
								comment = TTF_RenderText_Solid( font2, "Invalid area", textColor_White);
							}
							else //if the shoot was valid
							{
								//if the tile is empty
								if(AreaP2.GetOccupationalStatus(ClickedPosition.GetAbsis(), ClickedPosition.GetOrdinat()) == false)
								{
									ApplyTileSurface(ClickedPosition, MISS);
									//change commentary
									comment = TTF_RenderText_Solid( font2, "Missed!", textColor_White);
									AreaP2.SetHitStatus(ClickedPosition.GetAbsis(), ClickedPosition.GetOrdinat(), true);
									//change current player
									CurrentPlayer = P2;
									comment = TTF_RenderText_Solid( font2, "It's P2's turn. Attack the left side area", textColor_White);
								}
								//otherwise, if the shoot hits a ship
								else
								{
									ApplyTileSurface(ClickedPosition, HIT);
									//change commentary
									comment = TTF_RenderText_Solid( font2, "It hits something! Allowed to shoot again!", textColor_White);
									AreaP2.SetHitStatus(ClickedPosition.GetAbsis(), ClickedPosition.GetOrdinat(), true);
									//reduce health of the ship
									ShipP25.HitPart(ClickedPosition);
									if(ShipP25.GetHealth() == 0)
									{
										DestroyShipTile(ShipP25);
									}
									ShipP24a.HitPart(ClickedPosition);
									if(ShipP24a.GetHealth() == 0)
									{
										DestroyShipTile(ShipP24a);
									}
									ShipP24b.HitPart(ClickedPosition);
									if(ShipP24b.GetHealth() == 0)
									{
										DestroyShipTile(ShipP24b);
									}
									ShipP23.HitPart(ClickedPosition);
									if(ShipP23.GetHealth() == 0)
									{
										DestroyShipTile(ShipP23);
									}
									ShipP22.HitPart(ClickedPosition);
									if(ShipP22.GetHealth() == 0)
									{
										DestroyShipTile(ShipP22);
									}
								}
							}
						}
					}
				}
			}
			else //if(CurrentPlayer == P2)
			{
				//If there's events to handle
				if( SDL_PollEvent( &event ) )
				{
					//If the user has Xed out the window
					if( event.type == SDL_QUIT )
					{
						//Quit the program
						quit = true;
						break;
					}

					//Handle button events
					ButtonMainMenu.handle_events();

					if( event.type == SDL_MOUSEBUTTONUP )
					{
						//If the left mouse button was released
						if( event.button.button == SDL_BUTTON_LEFT )
						{
							ClickedPosition = GetClickedGridPos(X, Y);
							if(!(IsShootValid(X, Y, AreaP1)))
							{
								comment = TTF_RenderText_Solid( font2, "Invalid area", textColor_White);
							}
							else //if the shoot was valid
							{
								//if the tile is empty
								if(AreaP1.GetOccupationalStatus(ClickedPosition.GetAbsis(), ClickedPosition.GetOrdinat()) == false)
								{
									ApplyTileSurface(ClickedPosition, MISS);
									//change commentary
									comment = TTF_RenderText_Solid( font2, "Missed!", textColor_White);
									AreaP1.SetHitStatus(ClickedPosition.GetAbsis(), ClickedPosition.GetOrdinat(), true);
									//change current player
									CurrentPlayer = P1;
									comment = TTF_RenderText_Solid( font2, "It's P1's turn. Attack the right side area", textColor_White);
								}
								//otherwise, if the shoot hits a ship
								else
								{
									ApplyTileSurface(ClickedPosition, HIT);
									//change commentary
									comment = TTF_RenderText_Solid( font2, "It hits something! Allowed to shoot again!", textColor_White);
									AreaP1.SetHitStatus(ClickedPosition.GetAbsis(), ClickedPosition.GetOrdinat(), true);
									//reduce health of the ship
									ShipP15.HitPart(ClickedPosition);
									if(ShipP15.GetHealth() == 0)
									{
										DestroyShipTile(ShipP15);
									}
									ShipP14a.HitPart(ClickedPosition);
									if(ShipP14a.GetHealth() == 0)
									{
										DestroyShipTile(ShipP14a);
									}
									ShipP14b.HitPart(ClickedPosition);
									if(ShipP14b.GetHealth() == 0)
									{
										DestroyShipTile(ShipP14b);
									}
									ShipP13.HitPart(ClickedPosition);
									if(ShipP13.GetHealth() == 0)
									{
										DestroyShipTile(ShipP23);
									}
									ShipP12.HitPart(ClickedPosition);
									if(ShipP12.GetHealth() == 0)
									{
										DestroyShipTile(ShipP12);
									}
								}
							}
						}
					}
				}
			}

			//Show the commentary
			apply_surface( 0, 0, comment_dummy, screen );
			apply_surface( 270, 0, comment, screen );

			//Show the button (apply button "surface")
			ButtonMainMenu.show();

			//Update the screen
			SDL_Flip(screen);

			if (CurrentState == WELCOME_SCREEN)
			{
				break;
			}
			if (quit == true)
			{
				break;
			}
			//checks victory conditions
			if (ShipP15.GetHealth()==0 && ShipP14a.GetHealth()==0 && ShipP14b.GetHealth()==0 && ShipP13.GetHealth()==0 && ShipP12.GetHealth()==0)
			{
				CurrentState = VICTORY;
				comment = TTF_RenderText_Solid( font2, "The winner is Player 2! Congrats!", textColor_White);
				comment2 = TTF_RenderText_Solid( font2, "Click on the bottom-right button to quit to main menu", textColor_White);
			}
			if (ShipP25.GetHealth()==0 && ShipP24a.GetHealth()==0 && ShipP24b.GetHealth()==0 && ShipP23.GetHealth()==0 && ShipP22.GetHealth()==0)
			{
				CurrentState = VICTORY;
				comment = TTF_RenderText_Solid( font2, "The winner is Player 1! Congrats!", textColor_White);
				comment2 = TTF_RenderText_Solid( font3, "Click on the bottom-right button to quit to main menu", textColor_White);
			}
		}
		while(CurrentState == VICTORY)
		{
			if( SDL_PollEvent( &event ) )
			{
				//If the user has Xed out the window
				if( event.type == SDL_QUIT )
				{
					//Quit the program
					quit = true;
					break;
				}
			}

			//checks victory conditions
			if (ShipP15.GetHealth()==0 && ShipP14a.GetHealth()==0 && ShipP14b.GetHealth()==0 && ShipP13.GetHealth()==0 && ShipP12.GetHealth()==0)
			{
				apply_surface( 0, 0, background2, screen );
				apply_surface( 390, 0, comment, screen );
				apply_surface( 0, 100, comment2, screen );
			}
			else
			{
				if (ShipP25.GetHealth()==0 && ShipP24a.GetHealth()==0 && ShipP24b.GetHealth()==0 && ShipP23.GetHealth()==0 && ShipP22.GetHealth()==0)
				{
					apply_surface( 0, 0, background2, screen );
					apply_surface( 390, 0, comment, screen );
					apply_surface( 0, 40, comment2, screen );
				}
			}
			ButtonMainMenu.handle_events();
			ButtonMainMenu.show();

			SDL_Flip(screen);

			if (CurrentState == WELCOME_SCREEN)
			{
				break;
			}
			if (quit == true)
			{
				break;
			}
		}

		comment = TTF_RenderText_Solid(font3,"When you enter the gallery, use the arrow keys to rotate the model",textColor_White);
		comment2 = TTF_RenderText_Solid(font3,"Press [Enter] to switch the model, and [Esc] to back to main menu",textColor_White);

		while(CurrentState == GALLERY_INTRO)
		{
			//If there's events to handle
			if( SDL_PollEvent( &event ) )
			{
				//If the user has Xed out the window
				if( event.type == SDL_QUIT )
				{
					//Quit the program
					quit = true;
				}

				//Handle button events
				ButtonGallery.handle_events();
				ButtonMainMenu.handle_events();
			}
			
			apply_surface(0,0,background2,screen);
			apply_surface( 0, 0, comment, screen );
			apply_surface( 0, 40, comment2, screen );

			ButtonGallery.show();
			ButtonMainMenu.show();

			SDL_Flip(screen);

			if(quit == true)
			{
				break;
			}
		}

		//reset the text
		comment = TTF_RenderText_Solid( font2, "Okay Player 1, deploy your unit!", textColor_White);
		comment2 = TTF_RenderText_Solid( font3, "Position your unit using arrow keys, Press [Space] to change unit's orientation, Press [Enter] to deploy the unit", textColor_White);

		if(CurrentState == GALLERY)
		{
			ThreeDSFileSelector = 17;
			//Initialize
			if( initGallery() == false )
			{
				return 1;
			}
		}

		while(CurrentState == GALLERY)
		{
			while( SDL_PollEvent( &event ) )
			{
				if (event.type == SDL_QUIT)
				{
					quit = true;
				}
				else
				{
					if( event.type == SDL_KEYDOWN )
					{
						switch( event.key.keysym.sym )
						{
							case SDLK_UP: rotation_x_increment = rotation_x_increment - 0.2f; break;
							case SDLK_DOWN: rotation_x_increment = rotation_x_increment + 0.2f; break;
							case SDLK_LEFT: rotation_y_increment = rotation_y_increment - 0.2f; break;
							case SDLK_RIGHT: rotation_y_increment = rotation_y_increment + 0.2f; break;
							case SDLK_ESCAPE: CurrentState = WELCOME_SCREEN;
							case SDLK_RETURN: ThreeDSFileSelector++;
											  if (ThreeDSFileSelector > 17)
											  {
												  ThreeDSFileSelector = 1;
											  };
											  switch(ThreeDSFileSelector)
											  {
												case 1: CThreeMaxLoader::Load3DS(&object,"model/1_HumanShipyard.3ds"); break;
												case 2: CThreeMaxLoader::Load3DS(&object,"model/2_HumanBattleship.3ds"); break;
												case 3: CThreeMaxLoader::Load3DS(&object,"model/3_HumanDestroyerShip.3ds"); break;
												case 4: CThreeMaxLoader::Load3DS(&object,"model/4_HumanTransportShip.3ds"); break;
												case 5: CThreeMaxLoader::Load3DS(&object,"model/5_GoblinShipyard.3ds"); break;
												case 6: CThreeMaxLoader::Load3DS(&object,"model/6_Juggernaut.3ds"); break;
												case 7: CThreeMaxLoader::Load3DS(&object,"model/7_OrcishDestroyerShip.3ds"); break;
												case 8: CThreeMaxLoader::Load3DS(&object,"model/8_OrcishTransportShip.3ds"); break;
												case 9: CThreeMaxLoader::Load3DS(&object,"model/9_NightElfShipyard.3ds"); break;
												case 10: CThreeMaxLoader::Load3DS(&object,"model/10_NightElfBattleship.3ds"); break;
												case 11: CThreeMaxLoader::Load3DS(&object,"model/11_NIghtElfDestroyerShip.3ds"); break;
												case 12: CThreeMaxLoader::Load3DS(&object,"model/12_NightElfTransportShip.3ds"); break;
												case 13: CThreeMaxLoader::Load3DS(&object,"model/13_UndeadShipyard.3ds"); break;
												case 14: CThreeMaxLoader::Load3DS(&object,"model/14_UndeadBattleship.3ds"); break;
												case 15: CThreeMaxLoader::Load3DS(&object,"model/15_UndeadDestroyerShip.3ds"); break;
												case 16: CThreeMaxLoader::Load3DS(&object,"model/16_UndeadTransportShip.3ds"); break;
												case 17: CThreeMaxLoader::Load3DS(&object,"model/17_Rowboat.3ds"); break;
											  } break;
						}
					}
					else if( event.type == SDL_KEYUP )
					{
						switch( event.key.keysym.sym )
						{
							case SDLK_UP: rotation_x_increment = rotation_x_increment + 0.2f; break;
							case SDLK_DOWN: rotation_x_increment = rotation_x_increment - 0.2f; break;
							case SDLK_LEFT: rotation_y_increment = rotation_y_increment + 0.2f; break;
							case SDLK_RIGHT: rotation_y_increment = rotation_y_increment - 0.2f; break;
						}
					}
				}
			}

			update();
			render();

			if (quit == true)
			{
				break;
			}
			if (CurrentState == WELCOME_SCREEN)
			{
				break;
			}
		}

		if (CurrentState == WELCOME_SCREEN)
		{
			////Free the surface
			//SDL_FreeSurface(background);
			//SDL_FreeSurface(background2);
			//SDL_FreeSurface( buttonSheet );
			//SDL_FreeSurface( title );
			//SDL_FreeSurface( comment );
			//SDL_FreeSurface( comment2 );
			//SDL_FreeSurface( comment_dummy );
			//SDL_FreeSurface( grid );
			//SDL_FreeSurface( Pointer);
			//SDL_FreeSurface( ship5 );
			//SDL_FreeSurface( ship4a );
			//SDL_FreeSurface( ship4b );
			//SDL_FreeSurface( ship3 );
			//SDL_FreeSurface( ship2 );
			//SDL_FreeSurface( tile_miss );
			//SDL_FreeSurface( tile_hit );
			//SDL_FreeSurface( tile_x );
			//SDL_FreeSurface( screen );
			goto SDLINIT;
		}
    }

    //Clean up
    clean_up();

    return 0;
}
