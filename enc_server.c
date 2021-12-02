// Include necessary header files
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <string.h>

// Function prototypes
int chtoi (char);
char itoch(int);
void encrypt(char [], char []);
void printErrorMessage(const char*);

int main(int argc, char const *argv[])
{
  // Define necessary variables, arrays, and structures
  int listenSocket;
  int connectionSocket;
  int port;
  int status;
  int charsRead;
  int charsWritten;
	char buffer[1024];
  char message[100000];
	char key[100000];
  struct sockaddr_in serverAddress, clientAddress;
	socklen_t sizeOfClientInfo;
  pid_t pid;

  // Check usage and args
  if(argc != 2)
  {
    fprintf(stderr,"USAGE: %s port\n", argv[0]);
    exit(1);
  }
  
  // Clear out the server address struct
	memset((char *)&serverAddress, '\0', sizeof(serverAddress));

  // Convert the port number from an integer to a string
	port = atoi(argv[1]);  

  // Make the server address network capable
	serverAddress.sin_family = AF_INET;                              
  // Assign the port number to the variable port
	serverAddress.sin_port = htons(port);

  // Use the flag INADDR_ANY so that a client at any address can connect to this server
	serverAddress.sin_addr.s_addr = INADDR_ANY;

  // Create listening socket
	listenSocket = socket(AF_INET, SOCK_STREAM, 0);

  // If the socket did not open, print an error message on the screen
	if(listenSocket < 0)
  {
    printErrorMessage("ERROR opening socket");
  }

  // If server address did not bind to the socket, print an error message on the screen
	if(bind(listenSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
  {
	  printErrorMessage("ERROR on binding");
  }

	while(1)
	{	
    // Listen for connections.  Up to 5 connections can be in the queue at a time.
	  listen(listenSocket, 5);

		sizeOfClientInfo = sizeof(clientAddress); 
		
    // Accept the connection request, and create a connection socket
    connectionSocket = accept(listenSocket, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);

    // If the client is not successfully connected to the server, print an error message to the screen
		if (connectionSocket < 0) printErrorMessage("ERROR on accept");

		// Create child processes
		pid = fork();

		switch (pid)
    {
			// If the forking process was not successful, print an error message on the screen
			case -1:
			{
				printErrorMessage("ERROR with fork\n");
			}
      // If the forking process was successful
			case 0:
			{
				// Get the message from the client and display it
				memset(buffer, '\0', sizeof(buffer));

				charsWritten = 0;

				// Read the client's message from the socket
				while(charsWritten == 0)
					charsWritten = recv(connectionSocket, buffer, sizeof(buffer)-1, 0);
			
				// If the client's message was not successfully written to the socket, send an error message to the client
				if(strcmp(buffer, "enc_client") != 0)
				{
					charsWritten = send(connectionSocket, "ERROR writing to socket", 2, 0);
					exit(2);
				}
				else
				{
					// Clear out the buffer array
					memset(buffer, '\0', sizeof(buffer));

					// If the server received the message from the client, the server will send a success message back to the client
					charsWritten = send(connectionSocket, "I am the server, and I got your message", 3, 0);

					charsWritten = 0;

					// Receive the size of the file
					while(charsWritten == 0)
						charsWritten = recv(connectionSocket, buffer, sizeof(buffer)-1, 0);

					// Convert the file size from a string to an integer		
					int fileSize = atoi(buffer);
					
					charsWritten = 0;
			
					memset(buffer, '\0', sizeof(buffer));	
				
					while(charsWritten < fileSize)
					{					
						// Receive the message from the client
						charsWritten += recv(connectionSocket, buffer, sizeof(buffer)-1, 0);
						
						// Append the data in the buffer to the message array
						strcat(message, buffer);
					}
					// Clear out the buffer array
					memset(buffer, '\0', sizeof(buffer));
				
        	charsWritten = 0;
					
					while(charsWritten < fileSize)
					{		
            // Receive the key from the client		
						charsWritten += recv(connectionSocket, buffer, sizeof(buffer)-1, 0);
						
            // Append the data in the buffer to the key array
            strcat(key, buffer);
					}

					// Clear out the buffer array
					memset(buffer, '\0', sizeof(buffer));
		
					// Call the encrypt function to encrypt the message
					encrypt(message, key);
									
					charsWritten = 0;

					while(charsWritten < fileSize)
          { 
            // Send the encrypted message back to the client 
						charsWritten += send(connectionSocket, message, sizeof(message), 0);
					}

					exit(0);
				}
			}
      // Parent process will wait for child process to complete
			while (pid > 0)
      { 	
				pid = waitpid(-1, &status, WNOHANG);
		  }
		}
		// Close the connection socket
		close(connectionSocket);
	}
	// Close the listening socket
	close(listenSocket); 
	return 0;
}

// The function chtoi converts a character to its corresponding integer value
int chtoi(char character){
	if (character == ' '){
		return 26;
	}
	else {
		return (character - 'A');
	}
	return 0;
}

// The function itoch converts the integer value of a character to the corresponding character
char itoch(int i)
{
	if (i == 26){
		return ' ';
	}
	else 
	{
		return (i + 'A');
	}
}	

// This function encrypts a message
void encrypt(char message[], char key[])
{ 
	int idx=0;
  char character;
	char result;

	while (message[idx] != '\n')
	{
			// Assign the character in the current index of the message array to the variable character
	  		character = message[idx];

			// Convert the characters in the current index of the message and key arrays to their corresponding integer values, and then calculate the sum between the two integer values.  Next, calculate sum mod 27, and then store the final result.
	  		result = (chtoi(message[idx]) + chtoi(key[idx])) % 27;
			
			// Convert the integer result to its corresponding character, and store it in the current index of the message array
	  		message[idx] = itoch(result);
			// Increment the index
      idx++;
	}
	// Assign a null terminator to the last element of the message array
	message[idx] = '\0';
	return;
}

// This function prints a specific error message on the screen
void printErrorMessage(const char* errorMessage)
{
  perror(errorMessage);                   
  exit(1);
}