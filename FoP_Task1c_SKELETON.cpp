//---------------------------------------------------------------------------
//Program: Skeleton for Task 1c – group assignment
//Author: Pascale Vacher
//Last updated: 26 February 2018
//---------------------------------------------------------------------------

//Go to 'View > Task List' menu to open the 'Task List' pane listing the initial amendments needed to this program

//---------------------------------------------------------------------------
//----- include libraries
//---------------------------------------------------------------------------

//include standard libraries
#include <iostream>	
#include <iomanip> 
#include <conio.h> 
#include <cassert> 
#include <string>
#include <sstream>
#include <vector>
#include <ctime>
using namespace std;

//include our own libraries
#include "RandomUtils.h"    //for Seed, Random
#include "ConsoleUtils.h"	//for Clrscr, Gotoxy, etc.

//---------------------------------------------------------------------------
//----- define constants
//---------------------------------------------------------------------------

//defining the size of the grid
const int  SIZEX(25);    	//horizontal dimension
const int  SIZEY(15);		//vertical dimension
							//defining symbols used for display of the grid and content
const char SPOT('@');   	//spot
const char TUNNEL(' ');    	//tunnel
const char WALL('#');    	//border
const char HOLE('0');
const char POWERPILL('*');
const char ZOMBIE('Z');
const int MAXHOLES(12);      //number of holes
const int MAXPILLS(8);
const int MAXZOMBIES(4);
//defining the command letters to move the spot on the maze
const int  UP(72);			//up arrow
const int  DOWN(80); 		//down arrow
const int  RIGHT(77);		//right arrow
const int  LEFT(75);		//left arrow
							//defining the other command letters
const char QUIT('Q');		//to end the game

struct Item {
	int x, y;
	char symbol;
};

struct Stats {
	int lives;
	int powerpillsRemaining;
	int zombiesRemaining;
};

//---------------------------------------------------------------------------
//----- run game
//---------------------------------------------------------------------------


int main()
{
	//function declarations (prototypes)
	bool menuScreen(string& username);
	void initialiseGame(char g[][SIZEX], char m[][SIZEX], Item& spot, vector<Item>& holes, vector<Item>& pills, vector<Item>& zombies);
	void paintGame(const char g[][SIZEX], string mess, string spotMess, string zombieMess, Stats stats, string username);
	bool wantsToQuit(const int key);
	bool isArrowKey(const int k);
	int  getKeyPress();
	void updateGameData(const char g[][SIZEX], Item& spot, const int key, string& mess, string& spotMess, string& zombieMess, Stats& stats, char m[][SIZEX], vector<Item>& pills, vector<Item>& zombies);
	void updateGrid(char g[][SIZEX], const char m[][SIZEX], const Item spot, vector<Item>& holes, vector<Item>& pills, vector<Item>& zombies);
	void endProgram(Stats stats);
	void moveZombie(const char g[][SIZEX], Item& spot, string& zombieMess, Stats& stats, vector<Item>& zombies, Item& zombie, size_t zombieNumber);


	//local variable declarations 

	char grid[SIZEY][SIZEX];			//grid for display
	char maze[SIZEY][SIZEX];			//structure of the maze

	bool start = false;
	string username;

	Item spot = { 0, 0, SPOT }; 		//spot's position and symbol
	vector<Item> holes(MAXHOLES);
	vector<Item> pills(MAXPILLS);
	vector<Item> zombies(MAXZOMBIES);
	Item zombie = { 0, 0, ZOMBIE };
	Stats stats = { 5, MAXPILLS, MAXZOMBIES };
	string message("LET'S START...");	//current message to player
	string spotMess = "";
	string zombieMess = "";

										//action...
	Seed();								//seed the random number generator
	SetConsoleTitle("Spot and Zombies Game - FoP 2017-18");
	while (!start) {
		start = menuScreen(username);

	}

	initialiseGame(grid, maze, spot, holes, pills, zombies);	//initialise grid (incl. walls and spot)
	paintGame(grid, message, spotMess, zombieMess, stats, username);			//display game info, modified grid and messages
	int key;							//current key selected by player
	do {
		//TODO: command letters should not be case sensitive
		key = getKeyPress(); 	//read in  selected key: arrow or letter command
		if (isArrowKey(key))
		{
			updateGameData(grid, spot, key, message, spotMess, zombieMess, stats, maze, pills, zombies);		//move spot in that direction
			updateGrid(grid, maze, spot, holes, pills, zombies);					//update grid information
			for (size_t i(0); i < zombies.size(); ++i) {
				moveZombie(grid, spot, zombieMess, stats, zombies, zombies.at(i), i);
				updateGrid(grid, maze, spot, holes, pills, zombies);

			}
			
		}
		else
			message = "INVALID KEY!";	//set 'Invalid key' message
		paintGame(grid, message, spotMess, zombieMess, stats, username);		//display game info, modified grid and messages
	} while (!wantsToQuit(key) && stats.lives>0 && stats.powerpillsRemaining>0);		//while user does not want to quit and they have lives
	endProgram(stats);						//display final message
	return 0;
}

//---------------------------------------------------------------------------
//----- set menu screen
//---------------------------------------------------------------------------
bool menuScreen(string& username) {
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	time_t now = time(0);
	tm *ltm = localtime(&now);
	char letter;
	showMessage(clDarkGrey, clYellow, 42, 2, "DATE: ");
	cout << std::setfill('0') << std::setw(2) << ltm->tm_mday << "/" << std::setfill('0') << std::setw(2) << 1 + ltm->tm_mon << "/" << 1900 + ltm->tm_year << endl;
	showMessage(clDarkGrey, clYellow, 42, 3, "TIME: ");
	cout << std::setfill('0') << std::setw(2) << ltm->tm_hour << ":" << std::setfill('0') << std::setw(2) << ltm->tm_min << ":" << std::setfill('0') << std::setw(2) << ltm->tm_sec << endl;
	showMessage(clDarkGrey, clYellow, 10, 5, "----------------------");
	showMessage(clDarkGrey, clYellow, 10, 6, "|  SPOT AND ZOMBIES  |");
	showMessage(clDarkGrey, clYellow, 10, 7, "----------------------");
	showMessage(clDarkGrey, clYellow, 42, 5, "TO MOVE USE KEYBOARD ARROWS");
	showMessage(clDarkGrey, clYellow, 42, 6, "TO FREEZE ZOMBIES PRESS 'F'");
	showMessage(clDarkGrey, clYellow, 42, 7, "TO KILL ZOMBIES PRESS 'X'");
	showMessage(clDarkGrey, clYellow, 42, 8, "TO QUIT ENTER 'Q'");
	showMessage(clDarkGrey, clYellow, 10, 12, "GROUP CS3A  - 2017-18 ");
	showMessage(clDarkGrey, clYellow, 10, 13, "Adam Pick     27018599");
	showMessage(clDarkGrey, clYellow, 10, 14, "Dec Stanley   27039772");
	showMessage(clDarkGrey, clYellow, 10, 15, "Andrew Walker 27020770");
	showMessage(clDarkGrey, clYellow, 10, 20, "Enter your name: ");
	showMessage(clDarkGrey, clRed, 27, 20, username);
	cin >> username;
	for (int i = 0; i < 21; i++) {
		showMessage(clDarkGrey, clDarkGrey, 0, i, "                                                                     ");
	}
	return true;
}


//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------

void initialiseGame(char grid[][SIZEX], char maze[][SIZEX], Item& spot, vector<Item>& holes, vector<Item>& pills, vector<Item>& zombies)
{ //initialise grid and place spot in middle
	void setInitialMazeStructure(char maze[][SIZEX]);
	void setSpotInitialCoordinates(Item& spot);
	void setHoles(vector<Item>& holes, Item spot, vector<Item>& pills);
	void updateGrid(char g[][SIZEX], const char m[][SIZEX], Item b, vector<Item>& holes, vector<Item>& pills, vector<Item>& zombies);
	void setPills(vector<Item>& pills, Item spot);
	void setZombieCoordinates(vector<Item>& zombies);

	setInitialMazeStructure(maze);		//initialise maze
	setSpotInitialCoordinates(spot);
	setZombieCoordinates(zombies);
	setPills(pills, spot);

	setHoles(holes, spot, pills); //sets coordinates of holes


	updateGrid(grid, maze, spot, holes, pills, zombies);		//prepare grid




}

void setZombieCoordinates(vector<Item>& zombies) {
	zombies.at(0).x = 1; zombies.at(0).y = 1; zombies.at(0).symbol = ZOMBIE;
	zombies.at(1).x = 1; zombies.at(1).y = 13;  zombies.at(1).symbol = ZOMBIE;
	zombies.at(2).x = 23; zombies.at(2).y = 1;  zombies.at(2).symbol = ZOMBIE;
	zombies.at(3).x = 23; zombies.at(3).y = 13;  zombies.at(3).symbol = ZOMBIE;
}

void setPills(vector<Item>& pills, Item spot) {
	for (size_t i(0); i < pills.size(); ++i) {
		pills.at(i).x = Random(SIZEX - 2);
		pills.at(i).y = Random(SIZEY - 2);
		pills.at(i).symbol = POWERPILL;


		for (size_t k(0); k < pills.size(); ++k) { //Compares all the holes, and if a hole has the same coordinate as spot, or another hole, reassigns them coordinates
			for (size_t j(0); j < pills.size(); ++j) {
				if (((pills.at(k).x == pills.at(j).x) && (pills.at(k).y == pills.at(j).y)) || ((pills.at(k).x == spot.x) && pills.at(k).y == spot.y || (pills.at(k).x == 1 && pills.at(k).y == 1)
					|| (pills.at(k).x == 1 && pills.at(k).y == 13) || (pills.at(k).x == 23 && pills.at(k).y == 1) || (pills.at(k).x == 23 && pills.at(k).y == 13))) {
					pills.at(k).x = Random(SIZEX - 2);
					pills.at(k).y = Random(SIZEY - 2);
					pills.at(k).symbol = POWERPILL;
				}
			}
		}
	}
}

void setHoles(vector<Item>& holes, Item spot, vector<Item>& pills) { //sets the coordinates of 12 holes
	for (size_t i(0); i < holes.size(); ++i) {
		holes.at(i).x = Random(SIZEX - 2);
		holes.at(i).y = Random(SIZEY - 2);
		holes.at(i).symbol = HOLE;

		for (size_t k(0); k < holes.size(); ++k) { //Compares all the holes, and if a hole has the same coordinate as spot, or another hole, reassigns them coordinates
			for (size_t j(0); j < holes.size(); ++j) {
				if (((holes.at(k).x == holes.at(j).x) && (holes.at(k).y == holes.at(j).y)) || ((holes.at(k).x == spot.x) && holes.at(k).y == spot.y || (holes.at(k).x == 1 && holes.at(k).y == 1)
					|| (holes.at(k).x == 1 && holes.at(k).y == 13) || (holes.at(k).x == 23 && holes.at(k).y == 1) || (holes.at(k).x == 23 && holes.at(k).y == 13))) {
					holes.at(k).x = Random(SIZEX - 2);
					holes.at(k).y = Random(SIZEY - 2);
					holes.at(k).symbol = HOLE;
				}

				for (size_t l(0); l < pills.size(); ++l) {
					for (size_t m(0); m < pills.size(); ++m) {
						if ((pills.at(l).x == holes.at(j).x) && (pills.at(m).y == holes.at(k).y)) {
							holes.at(k).x = Random(SIZEX - 2);
							holes.at(k).y = Random(SIZEY - 2);
							holes.at(k).symbol = HOLE;
						}
					}

				}

			}
		}




	}
}


void setSpotInitialCoordinates(Item& spot)
{ //set spot coordinates inside the grid at random at beginning of game
  //TODO: Spot should not spwan on inner walls
	spot.y = Random(SIZEY - 2);      //vertical coordinate in range [1..(SIZEY - 2)]
	spot.x = Random(SIZEX - 2);      //horizontal coordinate in range [1..(SIZEX - 2)]
}

void setInitialMazeStructure(char maze[][SIZEX])
{ //set the position of the walls in the maze
  //TODO: initial maze configuration should be amended (size changed and inner walls removed)
  //initialise maze configuration
	char initialMaze[SIZEY][SIZEX] 	//local array to store the maze structure
		= { { '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#','#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
		{ '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#','#' } };
	//with '#' for wall, ' ' for tunnel, etc. 
	//copy into maze structure with appropriate symbols
	for (int row(0); row < SIZEY; ++row)
		for (int col(0); col < SIZEX; ++col)
			switch (initialMaze[row][col])
			{
				//not a direct copy, in case the symbols used change
			case '#': maze[row][col] = WALL; break;
			case ' ': maze[row][col] = TUNNEL; break;
			}
}

//---------------------------------------------------------------------------
//----- update grid state
//---------------------------------------------------------------------------

void updateGrid(char grid[][SIZEX], const char maze[][SIZEX], const Item spot, vector<Item>& holes, vector<Item>& pills, vector<Item>& zombies)
{ //update grid configuration after each move
	void setMaze(char g[][SIZEX], const char b[][SIZEX]);
	void placeItem(char g[][SIZEX], const Item spot);
	void drawHoles(char g[][SIZEX], vector<Item>& holes);
	void drawPills(char g[][SIZEX], vector<Item>& pills);
	void drawZombies(char g[][SIZEX], vector<Item>& zombies);
	setMaze(grid, maze);	//reset the empty maze configuration into grid
	placeItem(grid, spot);	//set spot in grid
	drawHoles(grid, holes);
	drawPills(grid, pills);
	drawZombies(grid, zombies);


}

void drawZombies(char g[][SIZEX], vector<Item>& zombies) {
	for (size_t i(0); i < zombies.size(); ++i) {
		g[zombies.at(i).y][zombies.at(i).x] = zombies[0].symbol;
	}
}

void drawPills(char g[][SIZEX], vector<Item>& pills) {
	for (size_t i(0); i < pills.size(); ++i) {
		g[pills.at(i).y][pills.at(i).x] = pills[0].symbol;
	}
}


void drawHoles(char g[][SIZEX], vector<Item>& holes) {
	for (size_t i(0); i < holes.size(); ++i) {
		g[holes.at(i).y][holes.at(i).x] = holes[0].symbol;
	}
}

void setMaze(char grid[][SIZEX], const char maze[][SIZEX])
{ //reset the empty/fixed maze configuration into grid
	for (int row(0); row < SIZEY; ++row)
		for (int col(0); col < SIZEX; ++col)
			grid[row][col] = maze[row][col];
}

void placeItem(char g[][SIZEX], const Item item)
{ //place item at its new position in grid
	g[item.y][item.x] = item.symbol;
}

//---------------------------------------------------------------------------
//----- move items on the grid
//---------------------------------------------------------------------------
void updateGameData(const char g[][SIZEX], Item& spot, const int key, string& mess, string& spotMess, string& zombieMess, Stats& stats, char m[][SIZEX], vector<Item>& pills, vector<Item>& zombies)
{ //move spot in required direction
	bool isArrowKey(const int k);
	void setKeyDirection(int k, int& dx, int& dy);
	assert(isArrowKey(key));

	//reset message to blank
	mess = "                                         ";		//reset message to blank

															//calculate direction of movement for given key
	int dx(0), dy(0);
	setKeyDirection(key, dx, dy);

	//check new target position in grid and update game data (incl. spot coordinates) if move is possible
	switch (g[spot.y + dy][spot.x + dx])
	{			//...depending on what's on the target position in grid...
	case TUNNEL:		//can move
		spot.y += dy;	//go in that Y direction
		spot.x += dx;	//go in that X direction
		spotMess = "                 ";
		break;
	case WALL:  		//hit a wall and stay there
		spotMess = "CANNOT GO THERE!";
		break;
	case HOLE:
		stats.lives = stats.lives - 1;
		spot.y += dy;
		spot.x += dx;
		spotMess = "LOST A LIFE!";
		break;
	case POWERPILL:
		stats.lives = stats.lives + 1;
		spot.y += dy;
		spot.x += dx;
		for (size_t i(0); i < pills.size(); ++i) {                                  //Deletes the powerpill when spot eats it
			if (spot.x == pills.at(i).x && spot.y == pills.at(i).y)					//
			{																		//
				pills.erase(pills.begin() + i);										//
			}																		//
		}																			//
		m[spot.y][spot.x] = TUNNEL;
		stats.powerpillsRemaining--;
		spotMess = "ATE A PILL!";
		break;


	}
	zombieMess = "                ";
}

void moveZombie(const char g[][SIZEX], Item& spot, string& zombieMess, Stats& stats, vector<Item>& zombies, Item& zombie, size_t zombieNumber) {
	int xDiff, yDiff, dy = 0, dx = 0;
	xDiff = spot.x - zombie.x;
	yDiff = spot.y - zombie.y;
	if (xDiff > 0) dx += 1;
	else if (xDiff < 0) dx -= 1;
	if (yDiff > 0) dy += 1;
	else if (yDiff < 0) dy -= 1;
	switch (g[zombie.y + dy][zombie.x + dx]) {
	case TUNNEL:
		zombie.y += dy;
		zombie.x += dx;
		zombieMess = "                ";
		break;
	case HOLE:
		stats.zombiesRemaining--;
		zombieMess = "FELL IN A HOLE";
		zombies.erase(zombies.begin() + zombieNumber);
		break;
	case SPOT:
		zombieMess = "HIT SPOT";
		stats.lives -= 1;
		zombie.x = (zombieNumber / 2) * 22 + 1;
		zombie.y = zombieNumber % 2 * 12 + 1;
		break;
	case POWERPILL:
		zombie.y += dy;
		zombie.x += dx;
		zombieMess = "              ";
		break;
	case ZOMBIE:
		zombieMess = "COLLIDED";
		for (size_t i(0); i < zombies.size(); ++i) {
			if (zombies.at(i).x == zombie.x + dx && zombies.at(i).y == zombie.y + dy) {
				zombies.at(i).x = (i / 2) * 22 + 1;
				zombies.at(i).y = i % 2 * 12 + 1;
			}
		}
		zombie.x = (zombieNumber / 2) * 22 + 1;
		zombie.y = zombieNumber % 2 * 12 + 1;
		break;

	}
}

//---------------------------------------------------------------------------
//----- process key
//---------------------------------------------------------------------------
void setKeyDirection(const int key, int& dx, int& dy)
{ //calculate direction indicated by key
  //TODO: UP and DOWN arrow keys should work
	bool isArrowKey(const int k);
	assert(isArrowKey(key));
	switch (key)	//...depending on the selected key...
	{
	case LEFT:  	//when LEFT arrow pressed...
		dx = -1;	//decrease the X coordinate
		dy = 0;
		break;
	case RIGHT: 	//when RIGHT arrow pressed...
		dx = +1;	//increase the X coordinate
		dy = 0;
		break;
	case UP:
		dx = 0;
		dy = -1;
		break;
	case DOWN:
		dx = 0;
		dy = +1;
		break;

	}
}

int getKeyPress()
{ //get key or command (in uppercase) selected by user
  //KEEP THIS FUNCTION AS GIVEN
	int keyPressed;
	keyPressed = _getch();			//read in the selected arrow key or command letter
	while (keyPressed == 224) 		//ignore symbol following cursor key
		keyPressed = _getch();
	return keyPressed;		//return it in uppercase 
}

bool isArrowKey(const int key)
{	//check if the key pressed is an arrow key (also accept 'K', 'M', 'H' and 'P')
	//TODO: UP and DOWN arrow keys should be detected
	return (key == LEFT) || (key == RIGHT) || (key == UP) || (key == DOWN);
}
bool wantsToQuit(const int key)
{	//check if the user wants to quit (when key is 'Q' or 'q')
	return key == QUIT;
}

//---------------------------------------------------------------------------
//----- display info on screen
//---------------------------------------------------------------------------

string toString(int x)
{	//convert an integer to a string
	std::ostringstream os;
	os << x;
	return os.str();
}
string tostring(char x)
{	//convert a char to a string
	std::ostringstream os;
	os << x;
	return os.str();
}
void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message)
{	//display a string using specified colour at a given position 
	Gotoxy(x, y);
	SelectBackColour(backColour);
	SelectTextColour(textColour);
	cout << message;
}


void paintGame(const char g[][SIZEX], string mess, string spotMess, string zombieMess, Stats stats, string username)
{ //display game title, messages, maze, spot and other items on screen
	string tostring(char x);
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	void paintGrid(const char g[][SIZEX]);
	time_t now = time(0);
	tm *ltm = localtime(&now);
	//TODO: Change the colour of the messages
	system("color 86");
	//display game title
	showMessage(clDarkGrey, clYellow, 0, 0, "___SPOT AND ZOMBIES GAME___");
	//TODO: Date and time should be displayed from the system
	showMessage(clDarkGrey, clYellow, 40, 0, "Adam Pick, Dec Stanley & Andrew Walker:");//TODO: Show your course, your group number and names on screen
	showMessage(clDarkGrey, clYellow, 40, 1, "CS3A - April 2018");
	showMessage(clDarkGrey, clYellow, 40, 2, "DATE: ");
	cout << std::setfill('0') << std::setw(2) << ltm->tm_mday << "/" << std::setfill('0') << std::setw(2) << 1 + ltm->tm_mon << "/" << 1900 + ltm->tm_year << endl;
	showMessage(clDarkGrey, clYellow, 40, 3, "TIME: ");
	cout << std::setfill('0') << std::setw(2) << ltm->tm_hour << ":" << std::setfill('0') << std::setw(2) << ltm->tm_min << ":" << std::setfill('0') << std::setw(2) << ltm->tm_sec << endl;
	//display menu options available
	showMessage(clDarkGrey, clYellow, 40, 7, "TO MOVE USE KEYBOARD ARROWS ");
	showMessage(clDarkGrey, clYellow, 40, 8, "TO QUIT ENTER 'Q'           ");

	//print auxiliary messages if any
	showMessage(clDarkGrey, clYellow, 40, 10, mess);	//display current message

	showMessage(clDarkGrey, clYellow, 0, 17, "SPOT " + spotMess);	//display spot message
	showMessage(clDarkGrey, clYellow, 0, 18, "ZOMBIE(S) " + zombieMess);	//display zombie message

														//display grid contents
	showMessage(clDarkGrey, clYellow, 40, 11, ("Lives Remaining: " + toString(stats.lives) + "      "));
	showMessage(clDarkGrey, clYellow, 40, 12, ("Powerpills Remaining: " + toString(stats.powerpillsRemaining) + "          "));
	showMessage(clDarkGrey, clYellow, 40, 13, ("Zombies Remaining: " + toString(stats.zombiesRemaining)));
	showMessage(clDarkGrey, clYellow, 40, 20, ("PLAYER'S NAME: " + username + "          "));
	//showMessage(clDarkGrey, clYellow, 40, 21, ("PLAYER'S PREVIOUS BEST SCORE: " +  + "          "));
	paintGrid(g);
}



void paintGrid(const char g[][SIZEX])
{ //display grid content on screen
	SelectBackColour(clBlack);
	Gotoxy(0, 2);
	for (int row(0); row < SIZEY; ++row)
	{
		for (int col(0); col < SIZEX; ++col) {
			char cell = g[row][col];
			switch (cell) {
			case HOLE:
				SelectTextColour(clRed);
				break;
			case POWERPILL:
				SelectTextColour(clYellow);
				break;
			case SPOT:
				SelectTextColour(clWhite);
				break;
			case WALL:
				SelectTextColour(clGrey);
				break;
			case ZOMBIE:
				SelectTextColour(clGreen);
				break;
			}
			cout << cell;
		}
		cout << endl;
	}
}

void endProgram(Stats stats)
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	if (stats.lives == 0) showMessage(clDarkGrey, clYellow, 40, 7, "YOU LOST!                  ");
	else if (stats.powerpillsRemaining == 0) showMessage(clDarkGrey, clYellow, 40, 7, "YOU WON WITH " + toString(stats.lives) + " REMAINING");
	showMessage(clDarkGrey, clYellow, 40, 7, "YOU QUIT!                  ");
	showMessage(clDarkGrey, clYellow, 40, 8, "");
	system("pause");	//hold output screen until a keyboard key is hit
}
