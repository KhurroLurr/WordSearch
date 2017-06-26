// Nicholas Espinosa
// PID n2431401
// COP 3502C - 0001
// HW 2 - WordSearch

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "WordSearch.h"

#define SIZE 25
#define DIRECTIONS 8

// Reading in the given dictionary file
int readDictionary(Dictionary *dict)
{
	int i;
	char *temp;
	FILE *ifp = fopen("dictionary.txt", "r");
	
	// If the file cannot be opened, the function returns 0
	if(ifp == NULL)
		return 0;

	// Scanning in the size of the dictionary and creating the counter
	fscanf(ifp, "%d", &dict->size);
	
	dict->counts = malloc(sizeof(int) * dict->size);
	for(i = 0; i < dict->size; i++)
		dict->counts[i] = 0;	
	
	if(dict->counts == NULL)
		return 0;

	// Creating the 2D array of words
	dict->words = (char **)malloc(sizeof(char *) * dict->size);
	
	for(i = 0; i < dict->size; i++)
	{
		// Temp is allocated to a set size and then the string is scanned into it
		temp = malloc(sizeof(char) * (SIZE + 1));
		fscanf(ifp, "%s", temp);
		
		// The array for the word in the dictionary is allocated to
		// the length of the word plus one 
		dict->words[i] = malloc(sizeof(char) * ((int)strlen(temp) + 1));
		
		// The string is copied over
		strcpy(dict->words[i], temp);
	
		// Temp is then freed
		free(temp);
	}

	// The file is closed
	fclose(ifp);

	return 1;
}

// Destroying the Dictionary
void destroyDictionary(Dictionary *dict)
{
	int i;

	// Freeing the words within the dictionary
	for(i = 0; i < dict->size; i++)
		free(dict->words[i]);

	// Freeing the string array, counter, and resetting the size 
	free(dict->words);
	free(dict->counts);
	dict->size = 0;
}

// Reading in the puzzle to analyze
int readPuzzle(WordSearchPuzzle *puzzle)
{
	int i, j;
	
	// Reading in the puzzle height and width
	scanf("%d", &puzzle->height);
	scanf("%d", &puzzle->width);

	// Dynamically allocating the puzzle board 
	puzzle->grid = (char **)malloc(sizeof(char *) * puzzle->height);
	
	// If malloc fails, return 0
	if(puzzle->grid == NULL)
		return 0;
		
	// Making the puzzle grid a 2D array
	for(i = 0; i < puzzle->height; i++)
	{
		puzzle->grid[i] = malloc(sizeof(char) * puzzle->width);

		// If malloc fails, return 0
		if(puzzle->grid[i] == NULL)
			return 0;
	}

	// Placing the letters inside the board
	for(i = 0; i < puzzle->height; i++)
	{
		for(j = 0; j < puzzle->width; j++)
		{
			scanf("%c", &puzzle->grid[i][j]);

			// Ensuring that only valid characters enter the board
			while(puzzle->grid[i][j] == ' ' || puzzle->grid[i][j] == '\n')
				scanf("%c", &puzzle->grid[i][j]);
		}
	}
	
	return 1;
}

// Destroying the Puzzle
void destroyPuzzle(WordSearchPuzzle *puzzle)
{
	int i;

	// Freeing the puzzle columns
	for(i = 0; i < puzzle->height; i++)
		free(puzzle->grid[i]);

	// Freeing the rows
	free(puzzle->grid);
	
	// Resetting the puzzle height and width
	puzzle->width = 0;
	puzzle->height = 0;

}

// Determines if a valid word was found
int checkString(Dictionary *dict, char *str)
{
	int lo = 0, mid, hi = dict->size - 1;

	// Binary search through the dictionary
	while(hi >= lo)
	{
		mid = (hi + lo) / 2;

		// If valid word is found, return 1
		if(strcmp(str, dict->words[mid]) == 0)
		{
			dict->counts[mid]++;
			return 1;
		}
		else if(strcmp (str, dict->words[mid]) < 0)
			hi = mid - 1;
		else
			lo = mid + 1;
	}

	// Otherwise, return 0
	return 0;
}

// Searches the puzzle to find possible matches
void searchPuzzle(Dictionary *dict, WordSearchPuzzle *puzzle)
{
	// Represents all possible directions
	int xMult[DIRECTIONS] = {-1,-1, -1, 0, 0, 1, 1, 1};
	int yMult[DIRECTIONS] = {-1, 0, 1, -1, 1, -1, 0, 1};
	
	// Variables used to words to check
	int i, j, k, l, kMax, x, y, Max, flag;
	char **word;

	// Determining the maximum length for a letter
	if(puzzle->height > puzzle->width)
		kMax = puzzle->height;
	else
		kMax = puzzle->width;

	// Dynamically allocating the array
	word = malloc(sizeof(char *) * DIRECTIONS);

	for(i = 0; i < DIRECTIONS; i++)
		word[i] = malloc(sizeof(char) * (kMax + 1));

	// For all rows
	for(i = 0; i < puzzle->height; i++)
	{
		// For all columns
		for(j = 0; j < puzzle->width; j++)
		{
		//printf("Checking Column %d\n", j + 1);
			// For every possible letter that can be added
			for(k = 0; k < kMax; k++)
			{
				//printf("Checking Combo %d\n", k + 1);	
				// Resets the flag for single letters
				if(k == 0)
					flag = 0;
				
				// For every direction
				for(l = 0; l < DIRECTIONS; l++)
				{
					// Determines the coordinates
					x = j + (k * xMult[l]);
					y = i + (k * yMult[l]);

					// If the coordinates are within bounds
					if((x >= 0 && x < puzzle->width) && (y >= 0 && y < puzzle->height))
					{
						
						// The letter is put in that area
						word[l][k] = puzzle->grid[y][x];
						word[l][k + 1] = '\0';
						
						// Ensures a single letter is only entered once
						if(k == 0 && flag == 0)
						{
							checkString(dict, word[l]);
							flag++;
						}
						else if(k > 0) // Checks valid words of 2 letters or more
							checkString(dict, word[l]);
						
					}
				}
			}
		}
	}
	

	// Freeing the allocated data
	for(i = 0; i < DIRECTIONS; i++)
		free(word[i]);

	free(word);
}

// Prints the final results of the word search
void printResults(Dictionary *dict)
{
	int i;

	for(i = 0; i < dict->size; i++)
	{
		// Only prints the words that are found 
		if(dict->counts[i] > 0)
			printf("%s (%d)\n", dict->words[i], dict->counts[i]);
	}
}

int main(void)
{
	Dictionary *dict = malloc(sizeof(Dictionary));
	WordSearchPuzzle *puzzle = malloc(sizeof(WordSearchPuzzle));

	// Creating the dictionary and puzzle
	readDictionary(dict);
	readPuzzle(puzzle);

	// Searching through the puzzle
	searchPuzzle(dict, puzzle);

	// Printing the results
	printResults(dict);

	// Destroying the puzzle and dictionary
	destroyDictionary(dict);
	destroyPuzzle(puzzle);

	return 0;
}
