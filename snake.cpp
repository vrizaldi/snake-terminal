#include <iostream>
#include <conio.h>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <thread>

using namespace std;

const int SCREEN_WIDTH = 50;
const int SCREEN_HEIGHT = 20;
const int INITIAL_SNAKE_LENGTH = 5;
const int INITIAL_TIME = 200;

const int NORTH = 101;
const int WEST = 102;
const int EAST = 103;
const int SOUTH = 104;

// functions for the main thread
void update();
void update_data();
void append_snake();
void generate_pill();
void check_collision();
void render();

// function for the input handler thread
void handle_input();

struct SnakeNode {
	int x;
	int y;
	SnakeNode *forward;
	SnakeNode *backward;	
};

struct Pill {
	int x;
	int y;
};

SnakeNode *head, *tail;
Pill pill;
int direction = NORTH;
int length;
bool over;
bool ready_for_input = false;
int main() {
	
	// initialise the snake
	head = new SnakeNode;
	tail = head;
	head->x = SCREEN_WIDTH / 2;
	head->y = SCREEN_HEIGHT / 2;
	length = 1;	
	for(int i = 0; i < INITIAL_SNAKE_LENGTH - 1; i++) append_snake();
	
	generate_pill();
	
	// game loop
	thread input_handler(handle_input);
	over = false;
	while(!over) {
		ready_for_input = true;
		update();
		this_thread::sleep_for(chrono::milliseconds(INITIAL_TIME / length));
	}
	
	// game over screen
	cout << "GAME OVER!!" << endl;
	cout << "Score: " << length << endl;
	system("pause");
	
	input_handler.detach();
	return 0;
}

void update() {
	
	update_data();
	render();	
}

void update_data() {
	// update the game data every game loop
	
	switch(direction) {
		case NORTH:
			tail->y = head->y - 1;
			if(tail->y < 0) tail->y = SCREEN_HEIGHT - 1;
			tail->x = head->x;	
			break;
		case WEST:
			tail->y = head->y;
			tail->x = head->x - 1;
			if(tail->x < 0) tail->x = SCREEN_WIDTH - 1;
			break;
		case EAST:
			tail->y = head->y;
			tail->x = head->x + 1;
			if(tail->x > SCREEN_WIDTH - 1) tail->x = 0;
			break;
		case SOUTH:
			tail->y = head->y + 1;
			if(tail->y > SCREEN_HEIGHT - 1) tail->y = 0;
			tail->x = head->x;
			break;
	}
	
	// move tail to be the new head
	head->forward = tail;
	tail->backward = head;
	head = tail;
	tail = tail->forward;
	head->forward = NULL;
	tail->backward = NULL;
	
	// check if pill is eaten
	if(head->x == pill.x && head->y == pill.y) {
		append_snake();
		generate_pill();
	}
	
	// check if collision happen
	check_collision();
}

void append_snake() {
	// add new node to the snake
	
	head->forward = new SnakeNode;
	
	head->forward->backward = head;
	int pos_x = head->x;
	int pos_y = head->y;
	switch(direction) {
		case NORTH:
			pos_y--;
			break;
		case WEST:
			pos_x--;
			break;
		case EAST:
			pos_x++;
			break;
		case SOUTH:
			pos_y++;
			break;
	}
	head->forward->x = pos_x;
	head->forward->y = pos_y;
	
	head = head->forward;
	length++;
}

void generate_pill() {
	// generate a new pill at random spot
	
	srand(time(NULL));
	pill.x = rand() % SCREEN_WIDTH;
	pill.y = rand() % SCREEN_HEIGHT; 
}

void check_collision() {
	// check if head collides with body
	
	for(SnakeNode *i = head->backward; i != NULL; i = i->backward) {
		if(head->x == i->x && head->y == i->y) over = true;
	}
}

void render() {	
	// render the game

	system("cls");
	
	// map where the snake nodes are
	bool map[SCREEN_WIDTH][SCREEN_HEIGHT];
	for(int x = 0; x < SCREEN_WIDTH; x++) {
		for(int y = 0; y < SCREEN_HEIGHT; y++) map[x][y] = false;
	}
	for(SnakeNode *i = head; i != NULL; i = i->backward) {
		map[i->x][i->y] = true;
	}
	map[pill.x][pill.y] = true;
	
	// render map
	for(int i = 0; i <= SCREEN_WIDTH; i++) cout << "-";
	cout << endl;
	for(int y = 0; y < SCREEN_HEIGHT; y++) {
		cout << "|";
		for(int x = 0; x < SCREEN_WIDTH; x++) {
			if(map[x][y]) cout << "O";
			else cout << " ";
		}
		cout << "|";
		cout << endl;
	}
	for(int i = 0; i <= SCREEN_WIDTH; i++) cout << "-";
	cout << endl;
}



void handle_input() {
	// handle input from user	
	
	while(true) {
		if(!ready_for_input) continue;		// prevent double input
		switch(getch()) {
			case 'w':
			case 'W':
				if(direction != SOUTH) direction = NORTH;
				break;
			case 'a':
			case 'A':
				if(direction != EAST) direction = WEST;
				break;
			case 'd':
			case 'D':
				if(direction != WEST) direction = EAST;
				break;
			case 's':
			case 'S':
				if(direction != NORTH) direction = SOUTH;
				break;				
		}
		ready_for_input = false;
	}
	
}
