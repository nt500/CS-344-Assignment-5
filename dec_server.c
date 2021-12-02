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
char itoch (int);
void decrypt(char [], char []);
void printErrorMessage(const char*);

int main(int argc, char *argv[])
{
  // Define necessary variables, arrays, and structures
	int listenSocket;
  int connectionSocket;
  int port;
  int charsWritten;
  int charsSent;
  int status;
  char buffer[1024];
  char* encryptedMessage[100000];
	char message[100000];
	char key[100000];
	socklen_t sizeOfClientInfo;
	struct sockaddr_in serverAddress, clientAddress;
	pid_t pid;

  // Check usage and args
	if (argc < 2) 
  { 
    fprintf(stderr, "USAGE: %s port\n", argv[0]); exit(1); 
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
	if (listenSocket < 0)
  {
    printErrorMessage("ERROR opening socket");
  }

	// If server address did not bind to the socket, print an error message on the screen
	if (bind(listenSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
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
		if (connectionSocket < 0)
    {
      printErrorMessage("ERROR on accept");
    }

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
				// Clear out the buffer array
				memset(buffer, '\0', sizeof(buffer));

				charsWritten = 0;

				// Read the client's message from the socket
				while(charsWritten == 0)
					charsWritten = recv(connectionSocket, buffer, sizeof(buffer)-1, 0);
				// If the client's message was not successfully read from the socket, send an error message to the client
				if (charsWritten < 0) printErrorMessage("ERROR reading from socket");
				
				// If the client's message was not successfully written to the socket, send an error message to the client
				if(strcmp(buffer, "dec_client") != 0)
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
					int size = atoi(buffer);
					
					charsWritten = 0;
					charsSent = 0;
					
					while(charsWritten < size)
					{
						// Receive the message from the client
						charsSent += recv(connectionSocket, buffer, sizeof(buffer)-1, 0);
						charsWritten += charsSent;
						charsSent = 0;

						// Append the data in the buffer to the message array
						strcat(message, buffer);
				
					}
					// Clear out the buffer array
					memset(buffer, '\0', sizeof(buffer));
					charsWritten = 0;
					charsSent = 0;

					while(charsWritten < size)
          {
						// Receive the key from the client
            charsSent = recv(connectionSocket, buffer, sizeof(buffer)-1, 0);
						
            charsWritten += charsSent;
						charsSent = 0;
						
            // Append the data in the buffer to the key array
            strcat(key, buffer);		
					}
					
				
					// Clear out the buffer array
					memset(buffer, '\0', sizeof(buffer));

					// Call the decrypt function to decrypt the message
					decrypt(message, key);
					
					charsWritten = 0;

					while(charsWritten < size)
          {
            // Send the decrypted message back to the client
            charsWritten += send(connectionSocket, message, sizeof(message), 0);
          }
					
					exit(0);
				}
			}
			// Parent process will wait for child process to complete
			default:
			{
				pid_t actualpid = waitpid(pid, &status, WNOHANG);
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
char itoch(int i){

	if (i == 26){
		return ' ';
	}
	else 
	{
		return (i + 'A');
	}
}	

// This function decrypts the message
void decrypt(char message[], char key[])
{
	int idx=0;
	char result;

	while (message[idx] != '\n')
	{
		// At each index in the message and character arrays, calculate the difference between the integer value of the character in the message array, and the integer value of the character in the key array.  Store the difference in the variable result.
		result = chtoi(message[idx]) - chtoi(key[idx]);

      // If the difference between the two integer values is negative, we will not be able to decrypt, so we have to add 27 to the difference
	  	if (result < 0)
      {
			  result += 27;
      }
	  	// Convert the integer value in the variable result to a character, and assign the character to the current index in the message array
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