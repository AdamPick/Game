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
#include <fstream>
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
const char HOLE('0');       //Hole
const char POWERPILL('*'); //powerpill
const char ZOMBIE('Z');    //zombie
const int MAXHOLES(12);      //number of holes
const int MAXPILLS(8);      //max number of pills	
const int MAXZOMBIES(4);    //max number of zombies
							//defining the command letters to move the spot on the maze
const int  UP(72);			//up arrow
const int  DOWN(80); 		//down arrow
const int  RIGHT(77);		//right arrow
const int  LEFT(75);		//left arrow
							//defining the other command letters
const char QUIT('Q');		//to end the game

struct Item {             // item struct which is used for all things placed in the game 
	int x, y;
	char symbol;
};

struct Stats {                         //stats structure .
	int lives;                       //spots number of lives
	int powerpillsRemaining;        //number of powerpills left to eat
	int zombiesRemaining;           //number of zombies remaining	
	int highscore;
};

//---------------------------------------------------------------------------
//----- run game
//---------------------------------------------------------------------------


int main()
{
	//function declarations (prototypes)
	bool entryScreen(string& username);
	bool menuScreen(Stats stats, bool& wantsToQuit);
	void initialiseGame(char g[][SIZEX], char m[][SIZEX], Item& spot, vector<Item>& holes, vector<Item>& pills, vector<Item>& zombies);
	void paintGame(const char g[][SIZEX], string mess, string spotMess, string zombieMess, string cheatMess, Stats stats, string username);
	bool isArrowKey(const int k);
	int  getKeyPress();
	void updateGameData(const char g[][SIZEX], Item& spot, const int key, string& mess, string& spotMess, string& zombieMess, Stats& stats, char m[][SIZEX], vector<Item>& pills, vector<Item>& zombies, int& movementcount);
	void updateGrid(char g[][SIZEX], const char m[][SIZEX], const Item spot, vector<Item>& holes, vector<Item>& pills, vector<Item>& zombies);
	void endProgram(Stats stats, string username, bool cheated);
	void moveZombie(const char g[][SIZEX], Item& spot, string& zombieMess, Stats& stats, vector<Item>& zombies, Item& zombie, size_t zombieNumber, int movementcount);
	void setZombieCoordinates(vector<Item>& zombies);
	bool displayInfo(const char g[][SIZEX]);


	//local variable declarations 

	char grid[SIZEY][SIZEX];			//grid for display
	char maze[SIZEY][SIZEX];			//structure of the maze
	int movementcount = 0;             //number of moves SUPERSPOT has left before transforming

	bool start = false;
	string username;					//username of the player entered at start of game
	Item spot = { 0, 0, SPOT }; 		//spot's position and symbol
	vector<Item> holes(MAXHOLES);      //Creates vector where the different holes and their coordinates will be stored
	vector<Item> pills(MAXPILLS);		//Creates vector where the different pills and their coordinates will be stored
	vector<Item> zombies(MAXZOMBIES);   //Creates vector where the different zombies and their coordinates will be stored
	Item zombie = { 0, 0, ZOMBIE };
	Stats stats = { 5, MAXPILLS, MAXZOMBIES, -1 };
	string message("LET'S START...");	//current message to player
	string spotMess = "";               //contains information about spot's movements and conditions
	string zombieMess = "";				//contains information about zombie events
	string cheatMess = "";				//contains information on what cheats have been activated
	bool zombiesCanMove = true;			//used for the freezing zombies cheat
	bool zombiesWereKilled = false;		//used for killing zombies cheat
	bool cheated = false;				//used to store whether user has cheated or not
	bool wantsToQuit = false;			//used for loop to end game
	bool showInfo = false;

	//action...
	Seed();								//seed the random number generator
	SetConsoleTitle("Spot and Zombies Game - FoP 2017-18");
	while (!start) {
		start = entryScreen(username);
	}
	ifstream fin(username + ".txt");
	fin >> stats.highscore;
	fin.close();
	start = false;
	while (!start) {
		start = menuScreen(stats, wantsToQuit);
	}
	system("cls");

	initialiseGame(grid, maze, spot, holes, pills, zombies);	//initialise grid (incl. walls and spot)
	paintGame(grid, message, spotMess, zombieMess, cheatMess, stats, username);			//display game info, modified grid and messages
	int key;							//current key selected by player
	do {
		key = getKeyPress(); 	//read in  selected key: arrow or letter command
		if (isArrowKey(key))
		{
			updateGameData(grid, spot, key, message, spotMess, zombieMess, stats, maze, pills, zombies, movementcount);		//move spot in that direction
			updateGrid(grid, maze, spot, holes, pills, zombies);					//update grid information
			if (zombiesCanMove) {
				for (size_t i(0); i < zombies.size(); ++i) {
					moveZombie(grid, spot, zombieMess, stats, zombies, zombies.at(i), i, movementcount);
					updateGrid(grid, maze, spot, holes, pills, zombies);

				}
			}

		}
		else
			switch (toupper(key)) {
			case 'F':                                         //switch case for freezing zombies
				zombiesCanMove = !zombiesCanMove;
				if (!zombiesCanMove) cheatMess = "CHEAT: FREEZE!";
				else cheatMess = "CHEAT: UNFREEZE!";
				cheated = true;
				break;
			case 'E':                                      //cheat for eating all powerpills
				pills.clear();
				stats.powerpillsRemaining = 0;				//sets powerpills remaining to 0 for the message 
				updateGrid(grid, maze, spot, holes, pills, zombies);
				cheatMess = "CHEAT: EAT!";
				cheated = true;								//used for seeing whether user has cheated to invalidate high scores
				break;
			case 'X':
				if (!zombiesWereKilled) {					//kills all zombies if they're alive
					zombies.clear();
					stats.zombiesRemaining = 0;
					cheatMess = "CHEAT: KILL!";

				}
				else {
					for (int i = 0; i < MAXZOMBIES; i++) { //respawns zombies if they were killed
						zombies.push_back(zombie);
					}

					stats.zombiesRemaining = MAXZOMBIES;
					setZombieCoordinates(zombies);
					cheatMess = "CHEAT: UNKILL!";

				}
				updateGrid(grid, maze, spot, holes, pills, zombies);
				zombiesWereKilled = !zombiesWereKilled;
				cheated = true;
				break;
			case 'Q':
				menuScreen(stats, wantsToQuit);
				break;
			case 'I':
				showInfo = !showInfo;
				if (showInfo) {
					bool display = true;
					while (display) {
						display = displayInfo(grid);
					}
				}
				break;
			default:
				message = "INVALID KEY!";	//set 'Invalid key' message
				break;

			}
		paintGame(grid, message, spotMess, zombieMess, cheatMess, stats, username);		//display game info, modified grid and messages
	} while (!wantsToQuit && stats.lives>0 && stats.powerpillsRemaining>0);		//while user does not want to quit and they have lives
	endProgram(stats, username, cheated);						//display final message
	return 0;
}

//---------------------------------------------------------------------------
//----- Entry Screen
//---------------------------------------------------------------------------
bool entryScreen(string& username) { //entry screen of game
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	time_t now = time(0);
	tm *ltm = localtime(&now);  //sets the time
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
	showMessage(clDarkGrey, clYellow, 42, 8, "TO EAT ALL PILLS PRESS 'E'");
	showMessage(clDarkGrey, clYellow, 42, 9, "TO QUIT ENTER 'Q'");
	showMessage(clDarkGrey, clYellow, 10, 12, "GROUP CS3A  - 2017-18 ");
	showMessage(clDarkGrey, clYellow, 10, 13, "Adam Pick     27018599");
	showMessage(clDarkGrey, clYellow, 10, 14, "Dec Stanley   27039772");
	showMessage(clDarkGrey, clYellow, 10, 15, "Andrew Walker 27020770");
	showMessage(clDarkGrey, clYellow, 10, 20, "Enter your name: ");
	showMessage(clDarkGrey, clRed, 27, 20, username);
	cin >> username;
	system("cls");
	return true;
}

//------------------------------------------------------------------------------
//-------- Menu Screen
//------------------------------------------------------------------------------
bool displayInfo(const char g[][SIZEX]) {
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	void paintGrid(const char g[][SIZEX]);
	int getKeyPress();
	int key;
	system("cls");
	system("color 86");
	paintGrid(g);
	showMessage(clDarkGrey, clYellow, 26, 5, "WHEN SPOT EATS A POWERPILL, HE GAINS ONE LIFE");
	showMessage(clDarkGrey, clYellow, 26, 7, "WHEN SPOT FALLS INTO A HOLE, HE LOSES ONE LIFE");
	showMessage(clDarkGrey, clYellow, 26, 9, "WHEN SPOT HITS A ZOMBIE HE LOSES ONE LIFE");
	showMessage(clDarkGrey, clYellow, 26, 10, "AND THE ZOMBIE RETURNS TO ITS CORNER");
	showMessage(clDarkGrey, clYellow, 26, 12, "WHEN SPOT EATS A POWERPILL, HE GAINS ONE LIFE");
	showMessage(clDarkGrey, clYellow, 26, 14, "SPOT AND ZOMBIES CANNOT LEAVE THE BOUNDARIES");
	showMessage(clDarkGrey, clYellow, 26, 16, "WHEN ZOMBIES BUMP INTO EACH OTHER,");
	showMessage(clDarkGrey, clYellow, 26, 17, "THEY RETURN TO THEIR CORNERS");
	showMessage(clDarkGrey, clYellow, 26, 19, "WHEN A ZOMBIE FALLS DOWN A HOLE, IT DIES AND DOES");
	showMessage(clDarkGrey, clYellow, 26, 20, "NOT RETURN");
	key = getKeyPress();
	if (toupper(key) == 'I') {
		system("cls");
		return false;
	}
	else return true;
}

bool menuScreen(Stats stats, bool& wantsToQuit) {
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	string toString(int);
	int getKeyPress();
	int key;							//current key selected by player
	bool wantsToPlay = false;
	system("cls");
	system("color 86");

	showMessage(clDarkGrey, clYellow, 25, 10, "MENU");
	showMessage(clDarkGrey, clYellow, 25, 14, "'P' - PLAY GAME");
	showMessage(clDarkGrey, clYellow, 25, 15, "'S' - SHOW SCORE");
	showMessage(clDarkGrey, clYellow, 25, 16, "'R' - SHOW RULES");
	showMessage(clDarkGrey, clYellow, 25, 17, "'Q' - QUIT GAME");
	showMessage(clDarkGrey, clYellow, 50, 20, "");





	do {
		key = getKeyPress(); 	//read in  selected key: arrow or letter command

		switch (toupper(key)) {
		case 'B':
			system("cls");
			showMessage(clDarkGrey, clYellow, 25, 10, "MENU");
			showMessage(clDarkGrey, clYellow, 25, 14, "'P' - PLAY GAME");
			showMessage(clDarkGrey, clYellow, 25, 15, "'S' - SHOW SCORE");
			showMessage(clDarkGrey, clYellow, 25, 16, "'R' - SHOW RULES");
			showMessage(clDarkGrey, clYellow, 25, 17, "'Q' - QUIT GAME");
			break;
		case 'P':
			wantsToPlay = true;
			system("cls");
			break;
		case 'S':
			system("cls");
			showMessage(clDarkGrey, clYellow, 25, 10, "CURRENT SCORE: " + toString(stats.lives));
			showMessage(clDarkGrey, clYellow, 25, 15, "PRESS B TO GO BACK TO MENU");
			break;
		case 'R':
			system("cls");
			showMessage(clDarkGrey, clYellow, 10, 5, "WHEN SPOT EATS A POWERPILL, HE GAINS ONE LIFE");
			showMessage(clDarkGrey, clYellow, 10, 7, "WHEN SPOT FALLS INTO A HOLE, HE LOSES ONE LIFE");
			showMessage(clDarkGrey, clYellow, 10, 9, "WHEN SPOT HITS A ZOMBIE HE LOSES ONE LIFE");
			showMessage(clDarkGrey, clYellow, 10, 10, "AND THE ZOMBIE RETURNS TO ITS CORNER");
			showMessage(clDarkGrey, clYellow, 10, 12, "WHEN SPOT EATS A POWERPILL, HE GAINS ONE LIFE");
			showMessage(clDarkGrey, clYellow, 10, 14, "SPOT AND ZOMBIES CANNOT LEAVE THE BOUNDARIES");
			showMessage(clDarkGrey, clYellow, 10, 16, "WHEN ZOMBIES BUMP INTO EACH OTHER, THEY RETURN TO THEIR CORNERS");
			showMessage(clDarkGrey, clYellow, 10, 18, "WHEN A ZOMBIE FALLS DOWN A HOLE, IT DIES AND DOES NOT RETURN");
			showMessage(clDarkGrey, clYellow, 10, 22, "PRESS B TO GO BACK TO MENU");
			break;
		case 'Q':
			wantsToQuit = true;
			system("pls");
		}
	} while (!wantsToQuit && !wantsToPlay);
	return true;
}

//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------

void initialiseGame(char grid[][SIZEX], char maze[][SIZEX], Item& spot, vector<Item>& holes, vector<Item>& pills, vector<Item>& zombies)
{ //initialise grid and place spot in middle
	void setInitialMazeStructure(char maze[][SIZEX], char grid[][SIZEX]);
	void updateGrid(char g[][SIZEX], const char m[][SIZEX], Item b, vector<Item>& holes, vector<Item>& pills, vector<Item>& zombies);
	void placeItemsInGrid(vector<Item>& pills, char g[][SIZEX], vector<Item>& holes, vector<Item>& zombies, Item& spot);
	setInitialMazeStructure(maze, grid);		//initialise maze

	placeItemsInGrid(pills, grid, holes, zombies, spot); //sets coordinates of holes


	updateGrid(grid, maze, spot, holes, pills, zombies);		//prepare grid




}

void placeItemsInGrid(vector<Item>& pills, char g[][SIZEX], vector<Item>& holes, vector<Item>& zombies, Item& spot) {

	char grid2[SIZEX][SIZEY]; //a second grid for storing information about game characters so they can be put in different places 
	zombies.at(0).x = 1; zombies.at(0).y = 1; zombies.at(0).symbol = ZOMBIE;
	zombies.at(1).x = 1; zombies.at(1).y = 13;  zombies.at(1).symbol = ZOMBIE;
	zombies.at(2).x = 23; zombies.at(2).y = 1;  zombies.at(2).symbol = ZOMBIE;
	zombies.at(3).x = 23; zombies.at(3).y = 13;  zombies.at(3).symbol = ZOMBIE;   //sets the initial coordinates of the zombies 

	for (size_t i(0); i < zombies.size(); ++i) {
		grid2[zombies.at(i).y][zombies.at(i).x] = 'Z';
	}

	spot.y = Random(SIZEY - 2);      //vertical coordinate in range [1..(SIZEY - 2)]
	spot.x = Random(SIZEX - 2);      //horizontal coordinate in range [1..(SIZEX - 2)]

	grid2[spot.y][spot.x] = spot.symbol;

	while (grid2[spot.y][spot.x] == ZOMBIE || g[spot.y][spot.x] == '#') {
		spot.y = Random(SIZEY - 2);
		spot.x = Random(SIZEX - 2);
	}
	for (size_t i(0); i < pills.size(); ++i) {
		pills.at(i).x = Random(SIZEX - 2);
		pills.at(i).y = Random(SIZEY - 2);
		pills.at(i).symbol = POWERPILL;

		while ((grid2[pills.at(i).y][pills.at(i).x] == 'Z') || (g[pills.at(i).x][pills.at(i).y] == SPOT) || (grid2[pills.at(i).x][pills.at(i).y] == WALL)) {
			pills.at(i).x = Random(SIZEX - 2);
			pills.at(i).y = Random(SIZEY - 2);
			pills.at(i).symbol = POWERPILL;
		}
		if (i == 0) {
			grid2[pills.at(i).x][pills.at(i).y] = POWERPILL;
		}
		else {
			while ((grid2[pills.at(i).y][pills.at(i).x] == 'Z') || (g[pills.at(i).x][pills.at(i).y] == SPOT) || (grid2[pills.at(i).x][pills.at(i).y] == POWERPILL) || (grid2[pills.at(i).x][pills.at(i).y] == WALL)) {
				pills.at(i).x = Random(SIZEX - 2);
				pills.at(i).y = Random(SIZEY - 2);
				pills.at(i).symbol = POWERPILL;

			}
			grid2[pills.at(i).x][pills.at(i).y] = POWERPILL;
		}
	}
	for (size_t i(0); i < holes.size(); ++i) {
		holes.at(i).x = Random(SIZEX - 2);
		holes.at(i).y = Random(SIZEY - 2);
		holes.at(i).symbol = HOLE;

		while ((grid2[holes.at(i).y][holes.at(i).x] == 'Z') || (g[holes.at(i).x][holes.at(i).y] == SPOT) || (grid2[holes.at(i).x][holes.at(i).y] == POWERPILL)) {
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
				holes.at(i).y = Random(SIZEY - 2);
				holes.at(i).symbol = HOLE;

			}
			grid2[holes.at(i).x][holes.at(i).y] = HOLE;
		}
	}
}


void setZombieCoordinates(vector<Item>& zombies) {
	zombies.at(0).x = 1; zombies.at(0).y = 1; zombies.at(0).symbol = ZOMBIE;
	zombies.at(1).x = 1; zombies.at(1).y = 13;  zombies.at(1).symbol = ZOMBIE;
	zombies.at(2).x = 23; zombies.at(2).y = 1;  zombies.at(2).symbol = ZOMBIE;
	zombies.at(3).x = 23; zombies.at(3).y = 13;  zombies.at(3).symbol = ZOMBIE;
}


void setInitialMazeStructure(char maze[][SIZEX], char grid[][SIZEX])
{ //set the position of the walls in the maze
  //TODO: initial maze configuration should be amended (size changed and inner walls removed)
  //initialise maze configuration
	char initialMaze[SIZEY][SIZEX]; 
	ifstream fin("levels/level2.txt");
	for (int i = 0; i < SIZEY; i++) {
		for (int j = 0; j < SIZEX; j++) {
			fin.get(initialMaze[i][j]);
		}
	}
	fin.close();

	for (int row(0); row < SIZEY; ++row)
		for (int col(0); col < SIZEX; ++col)
			switch (initialMaze[row][col])
			{
				//not a direct copy, in case the symbols used change
			case '#': maze[row][col] = WALL; grid[row][col] = WALL; break;
			case ' ': maze[row][col] = TUNNEL; grid[row][col] = TUNNEL; break;
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
void updateGameData(const char g[][SIZEX], Item& spot, const int key, string& mess, string& spotMess, string& zombieMess, Stats& stats, char m[][SIZEX], vector<Item>& pills, vector<Item>& zombies, int& movementcount)
{ //move spot in required direction
	bool isArrowKey(const int k);
	void setKeyDirection(int k, int& dx, int& dy);

	assert(isArrowKey(key));

	//reset message to blank
	mess = "                                         ";		//reset message to blank

	if (movementcount == 0) {
		spot.symbol = SPOT;
	}

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
		spot.symbol = '$';
		movementcount = 10;
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

	if (movementcount > 0) {
		movementcount = movementcount - 1;
	}

}

void moveZombie(const char g[][SIZEX], Item& spot, string& zombieMess, Stats& stats, vector<Item>& zombies, Item& zombie, size_t zombieNumber, int movementcount) {
	int xDiff, yDiff, dy = 0, dx = 0;



	xDiff = spot.x - zombie.x;
	yDiff = spot.y - zombie.y;
	if (xDiff > 0) dx += 1;
	else if (xDiff < 0) dx -= 1;
	if (yDiff > 0) dy += 1;
	else if (yDiff < 0) dy -= 1;
	if (movementcount != 0) {
		cout << '\a';
		dy = dy * -1;
		dx = dx * -1;
	}
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
	case '$':
		stats.zombiesRemaining--;
		zombieMess = "EATEN BY SUPERSPOT!";
		zombies.erase(zombies.begin() + zombieNumber);
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
	case WALL:
		if (g[zombie.y][zombie.x + dx] == TUNNEL) zombie.x += dx;
		else if (g[zombie.y + dy][zombie.x] == TUNNEL) zombie.y += dy;
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


void paintGame(const char g[][SIZEX], string mess, string spotMess, string zombieMess, string cheatMess, Stats stats, string username)
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
	showMessage(clDarkGrey, clYellow, 40, 6, "TO MOVE USE KEYBOARD ARROWS ");
	showMessage(clDarkGrey, clYellow, 40, 7, "FOR INFORMATION PRESS 'I'");

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

	showMessage(clDarkGrey, clYellow, 40, 15, "TO FREEZE ZOMBIES PRESS 'F'");
	showMessage(clDarkGrey, clYellow, 40, 16, "TO KILL ZOMBIES PRESS 'X'");
	showMessage(clDarkGrey, clYellow, 40, 17, "TO EAT ALL PILLS PRESS 'E'");

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
			case '$':
				SelectTextColour(clBlue);
				break;
			}
			cout << cell;
		}
		cout << endl;
	}
}

void endProgram(Stats stats, string username, bool cheated)
{
	void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string message);
	if (stats.lives == 0) showMessage(clDarkGrey, clYellow, 40, 7, "YOU LOST!                  ");
	else if (stats.powerpillsRemaining == 0) {
		showMessage(clDarkGrey, clYellow, 40, 7, "YOU WON WITH " + toString(stats.lives) + " LIVES REMAINING");
		if (stats.lives > stats.highscore && !cheated) {
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
