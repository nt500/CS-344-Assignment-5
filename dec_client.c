// Include necessary header files
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>
#include <ctype.h>

// Function prototypes
int getCharCountInFile(const char*);
void printErrorMessage(const char*);

int main(int argc, char *argv[])
{
  // Define necessary variables and arrays
	int Socket;
  int port;
  int charsWritten;
  int charsRead;
  int bytesRead;
  int reusable;
  int messageCharCount;
  int keyCharCount;
	char buffer[1024];
	char message[100000];
  char* message1;
  struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;

  // Check usage and args
	if (argc < 3) 
  { 
    fprintf(stderr, "USAGE: %s hostname port\n", argv[0]); exit(1);
  } 

	// Clear out the address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); 
  
  // Convert the port number from an integer to a string
	port = atoi(argv[3]);
  
  // Make the server address network capable
	serverAddress.sin_family = AF_INET;

  // Assign the port number to the variable port
	serverAddress.sin_port = htons(port);
  
  // The host name is localhost
	serverHostInfo = gethostbyname("localhost");

  // If there is no server host information, print an error message to the screen
	if (serverHostInfo == NULL) 
  { 
    fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
    exit(0); 
  }
  
  // Copy the first IP address from the DNS entry to sin_addr.s_addr
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr_list[0], serverHostInfo->h_length);
  
	// Create a socket
	Socket = socket(AF_INET, SOCK_STREAM, 0);
  
  // If the socket did not successfully open, print an error message to the screen
	if (Socket < 0)
  {
    printErrorMessage("CLIENT: ERROR opening socket");
  }
    
	reusable = 1;

  // Set the socket so that it can be reused
	setsockopt(Socket, SOL_SOCKET, SO_REUSEADDR, &reusable, sizeof(int));

	// Connect to server
	if (connect(Socket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
  { 
    // Print error message to the screen if the client was not successfully connected to the server
		printErrorMessage("CLIENT: ERROR connecting");
	}

	messageCharCount = getCharCountInFile(argv[1]);
	keyCharCount = getCharCountInFile(argv[2]);

	// If the message character count is greater than the key character count, print an error message on the screen
	if(messageCharCount > keyCharCount)
  {
    printErrorMessage("ERROR: Your key does not contain enough characters");
  }	
	
	message1 = "dec_client";

  // Send message to the server
	charsWritten = send(Socket, message1, strlen(message1), 0);

	// Clear out the buffer array
	memset(buffer, '\0', sizeof(buffer)); 
	
	charsWritten = 0;
  
	while(charsWritten == 0)
  {
    // Write data to the socket
    charsWritten = recv(Socket, buffer, sizeof(buffer) - 1, 0);
  } 

	// If the client is not connected to the correct server, print an error message on the screen
	if(strcmp(buffer, "no") == 0) 
  {
    printErrorMessage("ERROR: Not connected to correct server");	
  }
	
	// Clear out the buffer array
	memset(buffer, '\0', sizeof(buffer)); 

	// Put the message character count in the buffer
	sprintf(buffer, "%d", messageCharCount);
	
	// Send the message character count to the server
	charsWritten = send(Socket, buffer, sizeof(buffer), 0);
	
	// Clear out the buffer array
	memset(buffer, '\0', sizeof(buffer)); 
	
	// Open the file for reading only
	int file1 = open(argv[1], 'r');
	charsWritten = 0;
		
	while(charsWritten <= messageCharCount)
	{
		// Read the file
		bytesRead = read(file1, buffer, sizeof(buffer)-1);

		// Send the message to the server
		charsWritten += send(Socket, buffer, strlen(buffer), 0);	
	}
	// Clear out the buffer array
	memset(buffer, '\0', sizeof(buffer));
	
	// Open the file for reading only
	file1 = open(argv[2], 'r');
	
  charsWritten = 0;
	
  while(charsWritten <= messageCharCount)
	{
		// Read the file
    bytesRead = read(file1, buffer, sizeof(buffer)-1);
    
    // Send the key to the server
		charsWritten += send(Socket, buffer, strlen(buffer), 0);	
	}
	
	// Clear out the buffer array
	memset(buffer, '\0', sizeof(buffer)); 

	charsWritten = 0;

	while(charsWritten < messageCharCount)
	{	
		// Read data from the socket
		charsWritten += recv(Socket, buffer, sizeof(buffer) - 1, 0);
		
    // Append the data in the buffer to the message array
		strcat(message, buffer);
	}
  
	// Append a newline character to the message array
	strcat(message, "\n");

	// Print the message on the screen
	printf("%s", message);
	
  // Close the socket
	close(Socket);

	return 0;
}

// The function getCharCountinFile gets and returns the count of the characters in the file
int getCharCountInFile(const char* fileName)
{
	char ch;
	int charCount = 0;

	// The function fopen opens the file.  The flag "r" indicates that the file will be open for reading only.
	FILE* file = fopen(fileName, "r");

	// Get a character from the file
	ch = fgetc(file);

  while (!(ch == EOF || ch == '\n')) 
	{
		// Validate the characters in the file.  If there are characters that are not upper case letters or spaces, print an error message on the screen. 
		if(!isupper(ch) && ch != ' ')
    {
			printErrorMessage("File contains invalid characters!");
    }  
		
		// The function fgetc gets a character from the file
		ch = fgetc(file);

		// Increment the count
    charCount++;
  }

	// Close the file
	fclose(file);
  
	// Return the count of the characters in the file
	return charCount;
}

// This function prints a specific error message on the screen
void printErrorMessage(const char* errorMessage)
{
  perror(errorMessage);                   
  exit(1);
}