// Include necessary header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char* argv[]) {
	
	// If the user did not specify a key length, print an error message stating that the length argument is missing
	if(argc < 2)
	{
		fprintf(stderr, "Error: Not enough arguments");
		exit(1);
	}

	// The function atoi converts the user specified key length, which was entered in as a string, to an integer
	int keyLength = atoi(argv[1]);
	
  // Declare an array to hold the key
  char key[keyLength+1];
	
  // Variable to hold index
  int idx=0;

	// This array will hold the 27 allowed characters (letters A-Z and the space character)
	char* allowedCharacters ="ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

	// Initialize the random number generator
	srand(time(0));

	while(idx<keyLength)
	{
		// Generate a random number between 0 and 26
    int randNum = rand() % 27;
		// The random number generated above will now be used to select a character from the allowedCharacters array, and then assign that character to the current element in the key array
		key[idx] = allowedCharacters[randNum];
		// Increment the index of the key array
    idx++;
	}
	// Assign a null terminator to the last element of the key array
	key[keyLength] = '\0'; 

	// Print the key on the screen
	printf("%s\n", key); 
	return 0;
}