// 2048
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <conio.h>

int screen(int tiles[4][4], int *scr, int *hscr, int preTiles[4][4], int *tmod);  // Declarations of the functions below the main function
int play(int tiles[4][4], int direction, int preTiles[4][4], int *scr, int *hscr, int *prescr, char conti);
void start(int tiles[4][4], int preTiles[4][4]);
void waitms(int miliseconds);
void save(FILE *thef, int *scr, int *hscr, int tiles[4][4]);
void load(FILE *thef, int *scr, int *hscr, int tiles[4][4]);
int simulate(int tiles[4][4]);
int simulateCollide(int tiles[4][4], int direction);

int main() {
	int i, j, k;
	int tiles[4][4], preTiles[4][4], end = 0;
	int arrow, isArrow, scr = 0, hscr = 0, prescr = 0;
	int tmod = 0, reach = 0;
	char conti = 'n';
	FILE *savef;
	srand(time(NULL));
	
	start(tiles, preTiles);
	
	do {
		end = 0;
		while(end == 0) {
			system("cls");
			screen(tiles, &scr, &hscr, preTiles, &tmod);
			printf("\n\nPress the arrow keys to select a direction to move tiles. \n");
			isArrow = getch();
			if (isArrow == 0xE0) { // This controls if a scrool key was pressed. So it may not be an arrow key but a scrool, it will be controlled later.
				arrow = getch();
				switch (arrow) { // 0: Top, 1: Down, 2: Left, 3: Right for play function
					case 72: // Up
						end = play(tiles, 0, preTiles, &scr, &hscr, &prescr, conti);
						break;
					case 80: // Down	
						end = play(tiles, 1, preTiles, &scr, &hscr, &prescr, conti);
						break;
					case 75: // Left
						end = play(tiles, 2, preTiles, &scr, &hscr, &prescr, conti);
						break;
					case 77: // Right
						end = play(tiles, 3, preTiles, &scr, &hscr, &prescr, conti);
						break;
					default:
						break; // In case of if its not an arrow key.
				};
			} 
			else if (isArrow == 'r') {
				scr = prescr;
				for(i = 0; i < 4; i++) {
					for(j = 0; j < 4; j++) {
						tiles[i][j] = preTiles[i][j];
					}
				}			
			}
			else if (isArrow == 'e') {
				end = 1;
			}
			else if (isArrow == 'x') {
				scr = 0;
				reach = 0;
				conti = 'n';
				start(tiles, preTiles);
			}
			else if (isArrow == 's') {
				save(savef, &scr, &hscr, tiles);
			}
			else if (isArrow == 'l') {
				load(savef, &scr, &hscr, tiles);
			}
			else if (isArrow == 't') {
				if (tmod == 1) {
					tmod = 0;
				} else {
					tmod = 1;
				}
			}
		}
	
		if(end == 1) { // Condition to lose
			system("cls");
			screen(tiles, &scr, &hscr, preTiles, &tmod);
			printf("\n\n        GAME OVER\n        YOU LOST!\n\n");
			waitms(1000);
			printf("Do you want to try again?\nY: Yes     N: No\n");
			scanf(" %c", &conti);
		}
		else if (end == 2 && reach == 0) { // Condition to win
			reach = 1;
			system("cls");
			screen(tiles, &scr, &hscr, preTiles, &tmod);
			printf("\n\n        YOU WIN!\n\n");
			waitms(1000);
			printf("Do you want to continue?\nY: Yes     N: No\n");
			scanf(" %c", &conti);
		}
		if (conti < 'a') { // If conti is an uppercase letter, this will convert it to a lowercase letter.
			conti = conti - 'A' + 'a';
		}
		if (conti != 'n' && conti != 'y') {
			printf("That's not an option. But I suppose you want to continue.\n");
			conti = 'y';
			waitms(2100);
		}
	} while (conti != 'N' && conti != 'n');
	
	return 0;
}

// tmod variable is a variable for test mode. The interface changes according to its value.
int screen(int tiles[4][4], int *scr, int *hscr, int preTiles[4][4], int *tmod) {
	int i, j;
	if(*scr >= *hscr) {
		*hscr = *scr;
	}
	printf("SCORE: %06d     HIGHEST SCORE: %06d\n\n", *scr, *hscr);
	if(*tmod == 1) {
		printf("Current State                    Previous State              \n");
	}
	for(i = 0; i < 4; i++) {
		printf("-------------------------"); // our hypen characters for the top of our interface of the game
		if (*tmod == 1) {
			printf("        -------------------------");
		}
		printf("\n|");
		for(j = 0; j < 4; j++) {
			if(tiles[i][j] > 0) { // Printing the numbers if there are numbers in tiles matrix.
				printf("%4d |", tiles[i][j]);
			} 
			else if (tiles[i][j] < 0) {
				tiles[i][j] = -tiles[i][j];
				printf("%3d* |", tiles[i][j]);
			}
			else {
				printf("%4c |", ' ');
			}
		}
		if (*tmod == 1) {
			printf("        |");
			for(j = 0; j < 4; j++) {
				if(preTiles[i][j] > 0) {
					printf("%4d |", preTiles[i][j]);
				} 
				else {
					printf("%4c |", ' ');
				}
			}
		}
		
		printf("\n");
	}
	printf("-------------------------"); 
	if(*tmod == 1) {
		printf("        -------------------------");
	}
	printf("\n\n");
	printf("Rewind: R      Restart: X      End: E      Save: S      Load: L      Change Interface: T");
	return 0;
}

int play(int tiles[4][4], int direction, int preTiles[4][4], int *scr, int *hscr, int *prescr, char conti) {
	int i, j, tempx, tempy, k, zeros = 0, moveCount = 0, temp[4][4], tempscr, zero_tiles[16][2], trand, rval = 0; 
	int c1, c2, c3, c4, c5, c6; // Variables for different cases. For directions.
	tempscr = *prescr;
	*prescr = *scr;
	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			temp[i][j] = preTiles[i][j];
			preTiles[i][j] = tiles[i][j];
		}
	}
	
	switch (direction) {
		case 0: c1 = 0; c2 = 1; c3 = 0; c4 = 1; c5 = 0; c6 = 0;   break; // Top
		case 1: c1 = 1; c2 = -1; c3 = 0; c4 = 1; c5 = 0; c6 = 0;  break; // Down
		case 2: c1 = 0; c2 = 0; c3 = 1; c4 = 0; c5 = 0; c6 = 1;   break; // Left
		case 3: c1 = 0; c2 = 0; c3 = 1; c4 = 0; c5 = 1; c6 = -1;  break; // Right
	}
	
	for(i = 0; i < 4; i++) { // Rearrangement of the tiles
		for (j = 0; j < 3; j++) {
			if (tiles[(3*c1) + (j*c2) + (i*c3)][(i*c4) + (3*c5) + (j*c3*c6)] == 0) {
				tempx = (3*c1) + (j*c2) + (i*c3); tempy = (i*c4) + (3*c5) + (j*c3*c6);
				k = j + 1;
				while(k < 4) {
					if(tiles[(3*c1) + (k*c2) + (i*c3)][(i*c4) + (3*c5) + (k*c3*c6)] == 0) {
						k++;
					} else {
						moveCount++;
						tiles[tempx][tempy] = tiles[(3*c1) + (k*c2) + (i*c3)][(i*c4) + (3*c5) + (k*c3*c6)];
						tiles[(3*c1) + (k*c2) + (i*c3)][(i*c4) + (3*c5) + (k*c3*c6)] = 0;
						break;
					}
				}
			}
		}
	}
	// This functions may seems like complex because it has six variables in it.
	// But it's more easy to handle like this.
	for(i = 0; i < 4; i++) { // Colliding tiles
		for(j = 0; j < 3; j++) {
			if(tiles[(3*c1) + (j*c2) + (i*c3)][(i*c4) + (3*c5) + (j*c3*c6)] != 0) {
				k = j + 1;
				while (k < 4) {
					if(tiles[(3*c1) + (j*c2) + (i*c3)][(i*c4) + (3*c5) + (j*c3*c6)] == tiles[(3*c1) + (k*c2) + (i*c3)][(i*c4) + (3*c5) + (k*c3*c6)]) {
						tiles[(3*c1) + (j*c2) + (i*c3)][(i*c4) + (3*c5) + (j*c3*c6)] = tiles[(3*c1) + (j*c2) + (i*c3)][(i*c4) + (3*c5) + (j*c3*c6)] + tiles[(3*c1) + (k*c2) + (i*c3)][(i*c4) + (3*c5) + (k*c3*c6)];
						*scr = *scr + tiles[(3*c1) + (j*c2) + (i*c3)][(i*c4) + (3*c5) + (j*c3*c6)];
						tiles[(3*c1) + (k*c2) + (i*c3)][(i*c4) + (3*c5) + (k*c3*c6)] = 0;
						moveCount++;
						break;
					}
					else if(tiles[(3*c1) + (k*c2) + (i*c3)][(i*c4) + (3*c5) + (k*c3*c6)] != 0) {
						break;
					}
					k++;
				}
			}
		}
	}
	
	for(i = 0; i < 4; i++) { // Rearrangement of the tiles (After colliding(s) of the tiles), in order to some problems after colliding of the tiles.
		for (j = 0; j < 3; j++) {
			if (tiles[(3*c1) + (j*c2) + (i*c3)][(i*c4) + (3*c5) + (j*c3*c6)] == 0) {
				tempx = (3*c1) + (j*c2) + (i*c3); tempy = (i*c4) + (3*c5) + (j*c3*c6);
				k = j + 1;
				while(k < 4) {
					if(tiles[(3*c1) + (k*c2) + (i*c3)][(i*c4) + (3*c5) + (k*c3*c6)] == 0) {
						k++;
					} else {
						moveCount++;
						tiles[tempx][tempy] = tiles[(3*c1) + (k*c2) + (i*c3)][(i*c4) + (3*c5) + (k*c3*c6)];
						tiles[(3*c1) + (k*c2) + (i*c3)][(i*c4) + (3*c5) + (k*c3*c6)] = 0;
						break;
					}
				}
			}
		}
	}
	
	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			if(tiles[i][j] == 0) { // Counting the number of empty tiles
				zero_tiles[zeros][0] = i;
				zero_tiles[zeros][1] = j;
				zeros++;
			}
			if(tiles[i][j] == 2048) {
					rval = 2;
			}
		}
	}
	
	if(moveCount == 0) { // It will not generate a new number incase of invalid rounds.
		*prescr = tempscr;
		for(i = 0; i < 4; i++) {
			for(j = 0; j < 4; j++) {
				preTiles[i][j] = temp[i][j];
			}
		}
		if(zeros == 0 && simulate(tiles) == 0) {
			return 1;
		}
		return rval;
	}
	
	trand = rand() % zeros; // Zeros variable is the length of zero_tiles array.
	tempx = zero_tiles[trand][0];
	tempy = zero_tiles[trand][1];
	
	if((rand() % 10) > 3) {
		tiles[tempx][tempy] = 2;
	}
	else {
		tiles[tempx][tempy] = 4;
	}
	
	if(zeros == 1) {
		if (simulate(tiles) == 0) {
			tiles[tempx][tempy] = (-1) * tiles[tempx][tempy];	
			return 1;
		} 
	}
	
	tiles[tempx][tempy] = (-1) * tiles[tempx][tempy];	
	return rval;
}

void start(int tiles[4][4], int preTiles[4][4]) {	
	int i, j, tempx, tempy, randx = -1, randy = -1;	// variables for random coordinates
	
	for(i = 0; i < 4; i++) {
		for(j = 0; j < 4; j++) {
			tiles[i][j] = 0;
			preTiles[i][j] = 0;
		}
	}
	
	for(i = 0; i < 2; i++) {
		do { // This loop prevents the randomize same coordinates
			tempx = rand() % 4;
			tempy = rand() % 4;
		} while (tempx == randx && tempy == randy);
		randx = tempx; randy = tempy;
		tiles[randx][randy] = 2;
		preTiles[randx][randy] = 2;
	}
}

void waitms(int miliseconds) { // A function to wait for a specific time in miliseconds.
	int time_ms, time_end;
	
	time_ms = clock() * 1000 / CLOCKS_PER_SEC; 
	time_end = time_ms + miliseconds; // Time to tend.
	
	do {
		time_ms = clock() * 1000 / CLOCKS_PER_SEC;
	} while (time_ms <= time_end);
}

void save(FILE *thef, int *scr, int *hscr, int tiles[4][4]) { // The F, the file
	char *buffer;
	char file[] = "\\data.bin";
	char path[100];
	int saveArray[18];
	int b_len = 0, f_len = 10, i, j = 0;
	
	saveArray[0] = *scr; // Copying datas to an array.
	saveArray[1] = *hscr;
	for(i = 2; i < 18; i++) {
		saveArray[i] = tiles[j][(i-2)%4];
		if((i-2)%4 == 3) {
			j++;
		}
	}
	
	if((buffer = getcwd( NULL, 0 )) == NULL) { // getcwd() => gets the current working directory
		perror("Error: Couldn't get the current working directory : ");
	} 
	else {		
		while(buffer[b_len] != '\0') {
			b_len++;
		}
		if(b_len + f_len + 1 > 100) {
			printf("\nCouldn't save the game because the directory's path is too long.");
			waitms(1000);
		} 
		else {
			for(i = 0; i < (b_len + f_len); i++) {
				if(i < b_len) {
					path[i] = buffer[i];
				}
				else {
					path[i] = file[i - b_len];
				}
			}
			path[i] = '\0'; 
			// printf("%s", path); Checking for path
		}
	}
	if ((thef = fopen(path, "wb")) == NULL) {
		printf("\nError! Couldn't open the save file");
		waitms(1000);
	} else {
		fwrite(saveArray, sizeof(saveArray), 1, thef); // size of int = 4, 4*18 = 72
		fclose(thef);
		free(buffer); 
	}
}

void load(FILE *thef, int *scr, int *hscr, int tiles[4][4]) {
	char *buffer;
	char file[] = "\\data.bin";
	char path[100];
	int loadArray[18];
	int b_len = 0, f_len = 10, i, j = 0;
	
	if((buffer = getcwd( NULL, 0 )) == NULL) { // getcwd() => gets the current working directory
		perror("Error: Couldn't get the current working directory : ");
	} 
	else {		
		while(buffer[b_len] != '\0') {
			b_len++;
		}
		// printf("%s \nLength: %d\n", buffer, b_len);
		if(b_len + f_len + 1 > 100) {
			printf("\nCouldn't save the game because the directory's path is too long.");
			waitms(1000);
		} 
		else {
			for(i = 0; i < (b_len + f_len); i++) { 
				if(i < b_len) {
					path[i] = buffer[i];
				}
				else {
					path[i] = file[i - b_len];
				}
			}
			path[i] = '\0'; 
			// printf("%s", path); Checking for path
		}
	}
	if ((thef = fopen(path, "rb")) == NULL) { // Checking if the path is null (which shouldn't be in normal conditions)
		printf("\nError! Couldn't open the save file");
		waitms(1000);
	} else {
		fread(loadArray, sizeof(loadArray), 1, thef);
		*scr = loadArray[0];
		*hscr = loadArray[1];
		for(i = 2; i < 18; i++) {
			tiles[j][(i-2)%4] = loadArray[i];
			if((i-2)%4 == 3) {
				j++;
			}
		}
		fclose(thef);
		free(buffer);
	}
}

int simulate(int tiles[4][4]) { // Simulating the current game
	int i;
	int num_of_plays = 0;
	for(i = 0; i < 4; i++) {
		num_of_plays = num_of_plays + simulateCollide(tiles, i);
	}
	return num_of_plays;
}

int simulateCollide(int tiles[4][4], int direction) { // Same as play function to see playable moves if there is.
	int i, j, k;
	int c1, c2, c3, c4, c5, c6;
	int colCount = 0;
	
	switch (direction) {
		case 0: c1 = 0; c2 = 1; c3 = 0; c4 = 1; c5 = 0; c6 = 0;   break; // Top
		case 1: c1 = 1; c2 = -1; c3 = 0; c4 = 1; c5 = 0; c6 = 0;  break; // Down
		case 2: c1 = 0; c2 = 0; c3 = 1; c4 = 0; c5 = 0; c6 = 1;   break; // Left
		case 3: c1 = 0; c2 = 0; c3 = 1; c4 = 0; c5 = 1; c6 = -1;  break; // Right
		default: c1 = 0; c2 = 0; c3 = 1; c4 = 0; c5 = 1; c6 = -1; break; // Top direction for default
	}
	
	for(i = 0; i < 4; i++) { // Colliding tiles
		for(j = 0; j < 3; j++) {
			if(tiles[(3*c1) + (j*c2) + (i*c3)][(i*c4) + (3*c5) + (j*c3*c6)] != 0) {
				k = j + 1;
				while (k < 4) {
					if(tiles[(3*c1) + (j*c2) + (i*c3)][(i*c4) + (3*c5) + (j*c3*c6)] == tiles[(3*c1) + (k*c2) + (i*c3)][(i*c4) + (3*c5) + (k*c3*c6)]) {
						colCount++;
						break;
					}
					else if(tiles[(3*c1) + (k*c2) + (i*c3)][(i*c4) + (3*c5) + (k*c3*c6)] != 0) {
						break;
					}
					k++;
				}
			}
		}
	}
	
	return colCount;
}



