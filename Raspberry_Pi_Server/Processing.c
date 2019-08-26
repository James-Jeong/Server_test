#include "Processing.h"

void* Processing(void* question){ // Total string splited by a space character
	char* response = (char*)malloc(sizeof(char) * BUFSIZE); // confirm message
	char** temp = (char**)malloc(sizeof(char) * BUFSIZE * BUFSIZE); // divided string about sensor value
	int count = 0;
	//printf("%s\n", (char*)question);
	
	char* quest = (char*)malloc(sizeof(char) * BUFSIZE);
	quest = (char*)question;
	char* ptr = strtok(quest, " ");
	
	while(ptr != NULL){
		temp[count] = ptr;
		//printf("%s\n", ptr);
		ptr = strtok(NULL, " ");
		count++;
	}
	// Split total string for handling data
	
	for(int i = 0; i < count; i++)
		printf("%s\n", temp[i]);
	// temp[0] : temper
	// temp[1] : humidity
	// temp[2] : UV_index
	// temp[3] : Air_quality
	// temp[4] : CO2
	// temp[5] : Micro_dust
	
	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// @@@@@@@@@@ Data Processing @@@@@@@@@@
	
	printf("%s\n", temp[0]); // temp[0] : temper
	printf("%s\n", temp[1]); // temp[1] : humidity
	printf("%s\n", temp[2]); // temp[2] : UV_index
	printf("%s\n", temp[3]); // temp[3] : Air_quality
	printf("%s\n", temp[4]); // temp[4] : CO2
	
	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	// Send a message to Arduino
	response = "Server received DATA\n";
	return (void*)response;
}
