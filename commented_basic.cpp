//---------------------------------------------------------------------------
//Program: Skeleton for Task 1c – group assignment
//Author: Adam Pick, Dec Stanley & Andrew Walker
//Last updated: 30 April 2018
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
#include <fstream>
using namespace std;

//include our own libraries
#include "RandomUtils.h"    //for Seed, Random
#include "ConsoleUtils.h"	//for Clrscr, Gotoxy, etc.
#include "TimeUtils.h"      //for GetDate, GetTime   

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
const char HOLE('0');		//hole
const char POWERPILL('*');	//powerpills
const char ZOMBIE('Z');		//zombie
const int MAXHOLES(12);      //number of holes
const int MAXPILLS(8);		//max number of pills
const int MAXZOMBIES(4);	//max number of zombies 
							//defining the command letters to move the spot on the maze
const int  UP(72);			//up arrow
const int  DOWN(80); 		//down arrow
const int  RIGHT(77);		//right arrow
const int  LEFT(75);		//left arrow
							//defining the other command letters
const char QUIT('Q');		//to end the game

struct Item {			//item to hold the coordinates and symbol 
	int x, y;
	char symbol;
};

struct Stats {
	int lives;                 //number of lives remaining	
	int powerpillsRemaining;  //number of powerpills remaining
	int zombiesRemaining;     //number of zombies remaining
	int highscore;            //stores the users highest score
};

//---------------------------------------------------------------------------
//----- run game
//---------------------------------------------------------------------------

int main()
{
	//function declarations (prototypes)
	bool entryScreen(string& username);
	void initialiseGame(char g[][SIZEX], char m[][SIZEX], Item& spot, vector<Item>& holes, vector<Item>& pills, vector<Item>& zombies);
	void paintGame(const char g[][SIZEX], string mess, string spotMess, string zombieMess, string cheatMess, Stats stats, string username);
	bool wantsToQuit(const int key);
	bool isArrowKey(const int k);
	int  getKeyPress();
	void updateGameData(const char g[][SIZEX], Item& spot, const int key, string& mess, string& spotMess, string& zombieMess, Stats& stats, char m[][SIZEX], vector<Item>& pills, vector<Item>& zombies);
	void updateGrid(char g[][SIZEX], const char m[][SIZEX], const Item spot, vector<Item>& holes, vector<Item>& pills, vector<Item>& zombies);
	void endProgram(Stats stats, string username);
	void moveZombie(const char g[][SIZEX], Item& spot, string& zombieMess, Stats& stats, vector<Item>& zombies, Item& zombie, size_t zombieNumber);
	void setZombieCoordinates(vector<Item>& zombies);
	//local variable declarations 
	char grid[SIZEY][SIZEX];			//grid for display
	char maze[SIZEY][SIZEX];			//structure of the maze
	bool start = false;
	string username;
	Item spot = { 0, 0, SPOT }; 		//spot's position and symbol
	vector<Item> holes(MAXHOLES);		//vector storing the holes
	vector<Item> pills(MAXPILLS);		//vector storing the pills
	vector<Item> zombies(MAXZOMBIES);	//vector storing the zombies
	Item zombie = { 0, 0, ZOMBIE };		//zombie item
	Stats stats = { 5, MAXPILLS, MAXZOMBIES, -1 }; //struct used to hold game values
	string message("LET'S START...");	//current message to player
	string spotMess = "";				//stores messages to do with spot's status
	string zombieMess = "";				//stores messages to do with zombies' status
	string cheatMess = "";				//stores messages to do with using cheats
	bool zombiesCanMove = true;			//bool for using the kill zombies cheat
	bool zombiesWereKilled = false;		//bool for respawning zombies after cheats used
	bool cheated = false;				//used to check whether cheats were used to invalidate high score
	Seed();								//seed the random number generator
	SetConsoleTitle("Spot and Zombies Game - FoP 2017-18"); //title of console game
	while (!start) { //if the user has not entered a valid username then continue to display the entry screen
		start = entryScreen(username);
	}
	ifstream fin(username + ".txt");  //loads highscores of the user from their text file
	fin >> stats.highscore; // load in previous highscore
	fin.close(); //close input stream
	initialiseGame(grid, maze, spot, holes, pills, zombies);	//initialise grid (incl. walls and spot)
	paintGame(grid, message, spotMess, zombieMess, cheatMess, stats, username);       //display game info, modified grid and messages
	int key;							//current key selected by player
	do {
		key = getKeyPress(); 	//read in  selected key: arrow or letter command
		if (isArrowKey(key)) // proceed with spot movement if an arrow key is pressed
		{
			updateGameData(grid, spot, key, message, spotMess, zombieMess, stats, maze, pills, zombies);		//move spot in that direction
			updateGrid(grid, maze, spot, holes, pills, zombies);					//update grid information
			if (zombiesCanMove) {
				for (size_t i(0); i < zombies.size(); ++i) {
					moveZombie(grid, spot, zombieMess, stats, zombies, zombies.at(i), i); //calls the function dealing with moving zombies
					updateGrid(grid, maze, spot, holes, pills, zombies);				//updates grid information
				}
			}
		}
		else
			switch (toupper(key)) {  //switch statement for cheats, saving and loading
			case 'F':    //cheats for freezing zombies
				zombiesCanMove = !zombiesCanMove; //zombies can no longer move when this is valid
				if (zombiesCanMove) cheatMess = "CHEAT: FREEZE!";
				else cheatMess = "CHEAT: UNFREEZE!";
				cheated = true; //set cheated to true so no new highscore can be noted
				break;
			case 'E':    //cheats for eating the pills 
				pills.clear(); //wipe power pills off the grid
				stats.powerpillsRemaining = 0; //sets powerpills remaining to 0
				updateGrid(grid, maze, spot, holes, pills, zombies); //updates grid information
				cheatMess = "CHEAT: EAT!"; //display a message to show that all power pills have been eaten
				cheated = true; //set cheated to true so no new highscore can be noted
				break;
			case 'X':   //cheats for exterminating zombies 
						//if zombies are still present
				if (!zombiesWereKilled) {
					zombies.clear(); //wipe zombies off the grid
					stats.zombiesRemaining = 0;   //clears zombies and sets stats remaining to 0 
					cheatMess = "CHEAT: KILL!"; //display a message to show that all zombies have been killed

				}
				else {
					for (int i = 0; i < MAXZOMBIES; i++) {
						zombies.push_back(zombie); //put zombies back into their corners
					}
					stats.zombiesRemaining = MAXZOMBIES; //sets the zombies remaining to 8 
					setZombieCoordinates(zombies); //reset zombie positions
					cheatMess = "CHEAT: UNKILL!"; //display a message to show that all zombies have been resurrected
				}
				updateGrid(grid, maze, spot, holes, pills, zombies); //updates grid information
				zombiesWereKilled = !zombiesWereKilled; //reset value so zombies cannot be killed twice
				cheated = true; //set cheated to true so no new highscore can be noted
				break;
			default:
				message = "INVALID KEY!";	//set 'Invalid key' message
				break;
			}
		paintGame(grid, message, spotMess, zombieMess, cheatMess, stats, username);		//display game info, modified grid and messages
	} while (!wantsToQuit(key) && stats.lives>0 && stats.powerpillsRemaining>0);		//while user does not want to quit and they have lives
	endProgram(stats, username);						//display final messages including score
	return 0; //return value 0 to indicate the end of the program
}

//---------------------------------------------------------------------------
//----- set entry screen
//---------------------------------------------------------------------------
bool entryScreen(string& username) { //function for the entry screen
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message); //forward reference showMessage so the individual lines can be displayed
	assert(true); //no preconditions
				  //use showMessage to display the entry screen
	showMessage(clDarkGrey, clYellow, 42, 2, "DATE: " + GetDate()); //display current date on entry, loaded from TimeUtils
	showMessage(clDarkGrey, clYellow, 42, 3, "TIME: " + GetTime()); //display current time on entry, loaded from TimeUtils
																	// game title
	showMessage(clDarkGrey, clYellow, 10, 5, "----------------------");
	showMessage(clDarkGrey, clYellow, 10, 6, "|  SPOT AND ZOMBIES  |");
	showMessage(clDarkGrey, clYellow, 10, 7, "----------------------");
	// game controls to display
	showMessage(clDarkGrey, clYellow, 42, 5, "TO MOVE USE KEYBOARD ARROWS");
	showMessage(clDarkGrey, clYellow, 42, 6, "TO FREEZE ZOMBIES PRESS 'F'");
	showMessage(clDarkGrey, clYellow, 42, 7, "TO KILL ZOMBIES PRESS 'X'");
	showMessage(clDarkGrey, clYellow, 42, 8, "TO EAT ALL PILLS PRESS 'E'");
	showMessage(clDarkGrey, clYellow, 42, 9, "TO QUIT ENTER 'Q'");
	// display the group members involved
	showMessage(clDarkGrey, clYellow, 10, 12, "GROUP CS3A  - 2017-18 ");
	showMessage(clDarkGrey, clYellow, 10, 13, "Adam Pick     27018599");
	showMessage(clDarkGrey, clYellow, 10, 14, "Dec Stanley   27039772");
	showMessage(clDarkGrey, clYellow, 10, 15, "Andrew Walker 27020770");
	// display a request for the user to enter their chosen username
	showMessage(clDarkGrey, clYellow, 10, 20, "Enter your name: ");
	showMessage(clDarkGrey, clRed, 27, 20, username); //displays the username you enter 
	cin >> username; // player's username is set when entered
	for (int i = 0; i < 40; i++) { //once the username is entered wipe the screen so the text does not go onto the next screen
		showMessage(clDarkGrey, clDarkGrey, 0, i, "                                                                                     "); //allows you to enter your username
	}
	return true; // return true to show that the entry screen has been passed through
}

//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------

void initialiseGame(char grid[][SIZEX], char maze[][SIZEX], Item& spot, vector<Item>& holes, vector<Item>& pills, vector<Item>& zombies)
{ //initialise grid and place spot in middle
  //forward references for procedures used
	void setInitialMazeStructure(char maze[][SIZEX]);
	void updateGrid(char g[][SIZEX], const char m[][SIZEX], Item b, vector<Item>& holes, vector<Item>& pills, vector<Item>& zombies);
	void setGameElements(vector<Item>& pills, char g[][SIZEX], vector<Item>& holes, vector<Item>& zombies, Item& spot);
	setInitialMazeStructure(maze);		//initialise maze
	setGameElements(pills, grid, holes, zombies, spot); //sets the coordinates of all game items
	updateGrid(grid, maze, spot, holes, pills, zombies); //prepare grid
}

void setZombieCoordinates(vector<Item>& zombies) {  //sets coordinates of zombies and their symbol for when they need to be respawned
	zombies.at(0).x = 1; zombies.at(0).y = 1; zombies.at(0).symbol = ZOMBIE;
	zombies.at(1).x = 1; zombies.at(1).y = 13;  zombies.at(1).symbol = ZOMBIE;
	zombies.at(2).x = 23; zombies.at(2).y = 1;  zombies.at(2).symbol = ZOMBIE;
	zombies.at(3).x = 23; zombies.at(3).y = 13;  zombies.at(3).symbol = ZOMBIE;
}

void setGameElements(vector<Item>& pills, char g[][SIZEX], vector<Item>& holes, vector<Item>& zombies, Item& spot) {

	char grid2[SIZEX][SIZEY];

	zombies.at(0).x = 1; zombies.at(0).y = 1; zombies.at(0).symbol = ZOMBIE;
	zombies.at(1).x = 1; zombies.at(1).y = 13;  zombies.at(1).symbol = ZOMBIE;
	zombies.at(2).x = 23; zombies.at(2).y = 1;  zombies.at(2).symbol = ZOMBIE;
	zombies.at(3).x = 23; zombies.at(3).y = 13;  zombies.at(3).symbol = ZOMBIE;

	for (size_t i(0); i < zombies.size(); ++i) {
		grid2[zombies.at(i).y][zombies.at(i).x] = 'Z';
	}

	spot.y = Random(SIZEY - 2);      //vertical coordinate in range [1..(SIZEY - 2)]
	spot.x = Random(SIZEX - 2);      //horizontal coordinate in range [1..(SIZEX - 2)]

	grid2[spot.y][spot.x] = spot.symbol;

	while (grid2[spot.y][spot.x] == ZOMBIE) {
		spot.y = Random(SIZEY - 2);
		spot.x = Random(SIZEX - 2);
	}

	for (size_t i(0); i < pills.size(); ++i) {
		pills.at(i).x = Random(SIZEX - 2);
		pills.at(i).y = Random(SIZEY - 2);
		pills.at(i).symbol = POWERPILL;



		while ((grid2[pills.at(i).y][pills.at(i).x] == 'Z') || (g[pills.at(i).x][pills.at(i).y] == SPOT)) { //stops powerpills from overlapping with other items
			pills.at(i).x = Random(SIZEX - 2);
			pills.at(i).y = Random(SIZEY - 2);
			pills.at(i).symbol = POWERPILL;
		}

		if (i == 0) {
			grid2[pills.at(i).x][pills.at(i).y] = POWERPILL;
		}
		else {

			while ((grid2[pills.at(i).y][pills.at(i).x] == 'Z') || (g[pills.at(i).x][pills.at(i).y] == SPOT) || (grid2[pills.at(i).x][pills.at(i).y] == POWERPILL)) {

				pills.at(i).x = Random(SIZEX - 2);
				pills.at(i).y = Random(SIZEY - 2);   //changes powerpill's coordinates if they overlap with anything else
				pills.at(i).symbol = POWERPILL;

			}

			grid2[pills.at(i).x][pills.at(i).y] = POWERPILL; //enters powerpill into the grid

		}


	}




	for (size_t i(0); i < holes.size(); ++i) { //creates random coordinates for all holes 
		holes.at(i).x = Random(SIZEX - 2);
		holes.at(i).y = Random(SIZEY - 2);
		holes.at(i).symbol = HOLE;




		while ((grid2[holes.at(i).y][holes.at(i).x] == 'Z') || (g[holes.at(i).x][holes.at(i).y] == SPOT) || (grid2[holes.at(i).x][holes.at(i).y] == POWERPILL)) { //changes coordinates if they overlap
			holes.at(i).x = Random(SIZEX - 2);
			holes.at(i).y = Random(SIZEY - 2);
			holes.at(i).symbol = HOLE;
		}

		if (i == 0) {
			grid2[holes.at(i).x][holes.at(i).y] = HOLE;
		}
		else {

			while ((grid2[holes.at(i).y][holes.at(i).x] == 'Z') || (g[holes.at(i).x][holes.at(i).y] == SPOT) || (grid2[holes.at(i).x][holes.at(i).y] == POWERPILL) || (grid2[holes.at(i).x][holes.at(i).y] == HOLE)) {

				holes.at(i).x = Random(SIZEX - 2);
				holes.at(i).y = Random(SIZEY - 2); // changes coordinates of holes if they overlap with other holes
				holes.at(i).symbol = HOLE;

			}

			grid2[holes.at(i).x][holes.at(i).y] = HOLE;
		}

	}

}

void setInitialMazeStructure(char maze[][SIZEX])
	{
	 //set the position of the walls in the maze
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
	drawHoles(grid, holes); //draws holes in grid
	drawPills(grid, pills); //draws pills in grid
	drawZombies(grid, zombies); //draws zombies in grid
}

void drawZombies(char g[][SIZEX], vector<Item>& zombies) { //place zombies on the grid at their new position when called
	for (size_t i(0); i < zombies.size(); ++i) { //for every zombie, set a position and the symbol on the grid
		g[zombies.at(i).y][zombies.at(i).x] = zombies[0].symbol;
	}
}

void drawPills(char g[][SIZEX], vector<Item>& pills) { //place pills on the grid at their new position when called
	for (size_t i(0); i < pills.size(); ++i) { //for every power pill, set a position and the symbol on the grid
		g[pills.at(i).y][pills.at(i).x] = pills[0].symbol;
	}
}


void drawHoles(char g[][SIZEX], vector<Item>& holes) { //place holes on the grid at their new position when called
	for (size_t i(0); i < holes.size(); ++i) { //for every hole, set a position and the symbol on the grid
		g[holes.at(i).y][holes.at(i).x] = holes[0].symbol;
	}
}

void setMaze(char grid[][SIZEX], const char maze[][SIZEX]) //set maze onto existing grid
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
	bool isArrowKey(const int k); //check if key held is an arrow key
	void setKeyDirection(int k, int& dx, int& dy);  //calculate direction of movement for given key
	assert(isArrowKey(key));
	//reset message to blank
	mess = "                                         ";		//reset message to blank		
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
	case HOLE:                                //takes a life if spot falls down a hole
		stats.lives = stats.lives - 1;
		spot.y += dy;
		spot.x += dx;
		spotMess = "LOST A LIFE!";
		break;
	case POWERPILL: //if eaten, add a life to spot
		stats.lives = stats.lives + 1;
		spot.y += dy;
		spot.x += dx;
		for (size_t i(0); i < pills.size(); ++i) {                                  //Deletes the powerpill when spot eats it
			if (spot.x == pills.at(i).x && spot.y == pills.at(i).y)					
			{																		
				pills.erase(pills.begin() + i);										
			}																		
		}																			
		m[spot.y][spot.x] = TUNNEL;
		stats.powerpillsRemaining--;
		spotMess = "ATE A PILL!";
		break;
	}
	zombieMess = "                ";
}

void moveZombie(const char g[][SIZEX], Item& spot, string& zombieMess, Stats& stats, vector<Item>& zombies, Item& zombie, size_t zombieNumber) { //moves the zombie to chase spot
	int xDiff, yDiff, dy = 0, dx = 0;
	xDiff = spot.x - zombie.x; //chases spot using spot's coordinates
	yDiff = spot.y - zombie.y;
	if (xDiff > 0) dx += 1;
	else if (xDiff < 0) dx -= 1;
	if (yDiff > 0) dy += 1;
	else if (yDiff < 0) dy -= 1;
	switch (g[zombie.y + dy][zombie.x + dx]) {
	case TUNNEL:                                       //allows to zombie to move
		zombie.y += dy;
		zombie.x += dx;
		zombieMess = "                ";
		break;
	case HOLE:                                 //kills zombie if it falls down hole 
		stats.zombiesRemaining--;
		zombieMess = "FELL IN A HOLE";
		zombies.erase(zombies.begin() + zombieNumber);
		break;
	case SPOT:                                    //takes a life from spot if he is caught by zombies
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
	case ZOMBIE:									//sends zombie back to starting position if they collide with each other 
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
	case UP:      //when up arrow pressed decreases y coordinate
		dx = 0;
		dy = -1;
		break;
	case DOWN:   //when down arrow is pressed y coordinate increases 
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


void paintGame(const char g[][SIZEX], string mess, string spotMess, string zombieMess, string cheatMess, Stats stats, string username)
{ //display game title, messages, maze, spot and other items on screen
	string tostring(char x);
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	void paintGrid(const char g[][SIZEX]);
	system("color 86");
	//display game title
	showMessage(clDarkGrey, clYellow, 0, 0, "___SPOT AND ZOMBIES GAME___");
	showMessage(clDarkGrey, clYellow, 40, 0, "Adam Pick, Dec Stanley & Andrew Walker:");
	showMessage(clDarkGrey, clYellow, 40, 1, "CS3A - April 2018");
	showMessage(clDarkGrey, clYellow, 40, 2, "DATE: " + GetDate());
	showMessage(clDarkGrey, clYellow, 40, 3, "TIME: " + GetTime());  //displays the time
																		//display menu options available
	showMessage(clDarkGrey, clYellow, 40, 7, "TO MOVE USE KEYBOARD ARROWS ");
	showMessage(clDarkGrey, clYellow, 40, 8, "TO QUIT ENTER 'Q'           ");

	//print auxiliary messages if any
	showMessage(clDarkGrey, clYellow, 5, 23, mess);	//display current message

	showMessage(clDarkGrey, clYellow, 0, 17, "SPOT " + spotMess);	//display spot message
	showMessage(clDarkGrey, clYellow, 0, 18, "ZOMBIE(S) " + zombieMess);	//display zombie message
	showMessage(clDarkGrey, clYellow, 0, 19, cheatMess);



	//display grid contents
	showMessage(clDarkGrey, clYellow, 40, 10, ("Lives Remaining: " + toString(stats.lives) + "      "));
	showMessage(clDarkGrey, clYellow, 40, 11, ("Powerpills Remaining: " + toString(stats.powerpillsRemaining) + "          "));
	showMessage(clDarkGrey, clYellow, 40, 12, ("Zombies Remaining: " + toString(stats.zombiesRemaining)));
	//cheat keys
	showMessage(clDarkGrey, clYellow, 40, 15, "TO FREEZE ZOMBIES PRESS 'F'");
	showMessage(clDarkGrey, clYellow, 40, 16, "TO KILL ZOMBIES PRESS 'X'");
	showMessage(clDarkGrey, clYellow, 40, 17, "TO EAT ALL PILLS PRESS 'E'");
	//player info
	showMessage(clDarkGrey, clYellow, 40, 20, ("PLAYER'S NAME: " + username + "          "));
	showMessage(clDarkGrey, clYellow, 40, 21, ("PLAYER'S PREVIOUS BEST SCORE: " + toString(stats.highscore) + "          "));
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
				SelectTextColour(clRed);  //changes holes to red
				break;
			case POWERPILL:
				SelectTextColour(clYellow); //changes powerpills to yellow
				break;
			case SPOT:
				SelectTextColour(clWhite); //changes spot to white 
				break;
			case WALL:
				SelectTextColour(clGrey);  //changes walls to grey
				break;
			case ZOMBIE:
				SelectTextColour(clGreen); //changes zombies to green
				break;
			}
			cout << cell;
		}
		cout << endl;
	}
}

void endProgram(Stats stats, string username) //ends program 
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	if (stats.lives == 0) showMessage(clDarkGrey, clYellow, 40, 7, "YOU LOST!                  "); //losing message
	else if (stats.powerpillsRemaining == 0) { //winnning message
		showMessage(clDarkGrey, clYellow, 40, 7, "YOU WON WITH " + toString(stats.lives) + " LIVES REMAINING"); //displays final stats of user
		if (stats.lives > stats.highscore) {
			//open a file with the username given and set the new highscore
			ofstream fout;
			fout.open(username + ".txt", ios::out);
			fout << stats.lives;
			fout.close();
		}
	}
	else {
		showMessage(clDarkGrey, clYellow, 40, 7, "YOU QUIT!                  ");
	}
	showMessage(clDarkGrey, clYellow, 40, 8, "");
	system("pause");	//hold output screen until a keyboard key is hit
}
