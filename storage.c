#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage.h"

/* 
  definition of storage cell structure ----
  members :
  int building : building number of the destination
  int room : room number of the destination
  int cnt : number of packages in the cell
  char passwd[] : password setting (4 characters)
  char *context : package context (message string)
*/

typedef struct {
	int building;
	int room;
	int cnt;
	char passwd[PASSWD_LEN+1];
	
	char *context;
} storage_t;


static storage_t** deliverySystem; 			//deliverySystem
static int storedCnt = 0;					//number of cells occupied
static int systemSize[2] = {0, 0};  		//row/column of the delivery system
static char masterPassword[PASSWD_LEN+1];	//master password




// ------- inner functions ---------------

//print the inside context of a specific cell
//int x, int y : cell to print the context
static void printStorageInside(int x, int y){
	printf("\n------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n");
	if (deliverySystem[x][y].cnt > 0)
		printf("<<<<<<<<<<<<<<<<<<<<<<<< : %s >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n", deliverySystem[x][y].context);
	else
		printf("<<<<<<<<<<<<<<<<<<<<<<<< empty >>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
		
	printf("------------------------------------------------------------------------\n");
	printf("------------------------------------------------------------------------\n\n");
}

//initialize the storage
//set all the member variable as an initial value
//and allocate memory to the context pointer
//int x, int y : cell coordinate to be initialized
static void initStorage(int x, int y) {
	storage_t storage;		// make struct storage
	storage.building = 0;
	storage.room = 0;
	storage.cnt = 0;		// initialize variables to 0
	deliverySystem[x][y] = storage;		//initialize storage
}

//get password input and check if it is correct for the cell (x,y)
//int x, int y : cell for password check
//return : 0 - password is matching, -1 - password is not matching
static int inputPasswd(int x, int y) {
	char passwd[PASSWD_LEN+1];
	printf(" Input password for (%i, %i) storage : ",x ,y); 
	scanf("%4s", &passwd); 
	fflush(stdin);		//receive a password from the user
	
	if(strcmp(deliverySystem[x][y].passwd, passwd) == 0) 
		return 0;		// when deliverySystem[x][y].passwd and passwd are same
	
	else 
		return -1;		// when deliverySystem[x][y].passwd and passwd are not same
}


// ------- API function for main.c file ---------------

//backup the delivery system context to the file system
//char* filepath : filepath and name to write
//return : 0 - backup was successfully done, -1 - failed to backup
int str_backupSystem(char* filepath) {
	FILE *PF = fopen(filepath, "w");  //open file
	int x, y;
	
	fprintf(PF, "%i %i\n", systemSize[0], systemSize[1]);				//print row/column of the delivery system at file
	fprintf(PF, "%s\n", masterPassword);								//print masterPassword

	for(x=0; x<systemSize[0]; x++) 										//print deliverySystem's information (building, room, password and context) at file
	{
		for(y=0; y<systemSize[1]; y++) 									
		{		
			if (deliverySystem[x][y].cnt > 0) 							//when package exist, print deliverySystem's information
			{			
				fprintf(PF, "%d %d %d %d %s %s\n", x, y, deliverySystem[x][y].building, deliverySystem[x][y].room, deliverySystem[x][y].passwd, deliverySystem[x][y].context);
			}
		}
	}

	fclose(PF);		//close PF file

	return 0;
}


//create delivery system on the double pointer deliverySystem
//char* filepath : filepath and name to read config parameters (row, column, master password, past contexts of the delivery system
//return : 0 - successfully created, -1 - failed to create the system
int str_createSystem(char* filepath) {
	int i;
	int x, y;	

	FILE *PF = fopen(filepath, "r");	
		
	if(PF != NULL) 	// if file succeed to open 
	{
		fscanf(PF, "%d %d", &systemSize[0], &systemSize[1]);
		fscanf(PF, "%s", &masterPassword);	

		deliverySystem = (storage_t **) malloc(systemSize[0] *sizeof(storage_t *)); 	//allocate memory of row
		
		for(i=0; i<systemSize[0]; i++) 
		{
			deliverySystem[i] = (storage_t *) malloc(systemSize[1] * sizeof(storage_t));	//allocate memory of column
		}
		
		for (x = 0; x < systemSize[0]; x++) 
		{
			for (y = 0; y < systemSize[1]; y++) 
			{
				initStorage(x, y); 		//initialize storage
			}
		}

		while(feof(PF) == 0) 
		{
			storage_t storage = { .building = 0, .room = 0, .cnt = 0, .passwd = { "" }, .context = (char*)malloc(sizeof(char) * (MAX_MSG_SIZE + 1)) };
			int x = -1;
			int y = -1;
			// reset value
			
			fscanf(PF, "%d %d %d %d %s %s", &x, &y, &storage.building, &storage.room, &storage.passwd, storage.context);

			if (x >= 0 && y >= 0) 
			{
				storage.cnt = strlen(storage.context); 		// string length input to storage.cnt
				deliverySystem[x][y] = storage;				
				storedCnt++;				
			}
		}

		fclose(PF);

	} 
	else 
		return -1;


	return 0;
}

//free the memory of the deliverySystem 
void str_freeSystem(void) {
	int x;
	for(x=0; x<systemSize[0]; x++) 
	{
		free(deliverySystem[x]);
	}
	// repeatedly free memory
	free(deliverySystem);
}



//print the current state of the whole delivery system (which cells are occupied and the destination of the each occupied cells)
void str_printStorageStatus(void) {
	int i, j;
	printf("----------------------------- Delivery Storage System Status (%i occupied out of %i )-----------------------------\n\n", storedCnt, systemSize[0]*systemSize[1]);
	
	printf("\t");
	for (j=0;j<systemSize[1];j++)
	{
		printf(" %i\t\t",j);
	}
	printf("\n-----------------------------------------------------------------------------------------------------------------\n");
	
	for (i=0;i<systemSize[0];i++)
	{
		printf("%i|\t",i);
		for (j=0;j<systemSize[1];j++)
		{
			if (deliverySystem[i][j].cnt > 0)
			{
				printf("%i,%i\t|\t", deliverySystem[i][j].building, deliverySystem[i][j].room);
			}
			else
			{
				printf(" -  \t|\t");
			}
		}
		printf("\n");
	}
	printf("--------------------------------------- Delivery Storage System Status --------------------------------------------\n\n");
}


//check if the input cell (x,y) is valid and whether it is occupied or not
int str_checkStorage(int x, int y) {
	if (x < 0 || x >= systemSize[0])
	{
		return -1;
	}
	
	if (y < 0 || y >= systemSize[1])
	{
		return -1;
	}
	
	return deliverySystem[x][y].cnt;	
}


//put a package (msg) to the cell
//input parameters
//int x, int y : coordinate of the cell to put the package
//int nBuilding, int nRoom : building and room numbers of the destination
//char msg[] : package context (message string)
//char passwd[] : password string (4 characters)
//return : 0 - successfully put the package, -1 - failed to put
int str_pushToStorage(int x, int y, int nBuilding, int nRoom, char msg[MAX_MSG_SIZE+1], char passwd[PASSWD_LEN+1]) {
	
	storage_t storage = {0,0,0};
	initStorage(x, y);
	deliverySystem[x][y].building = nBuilding;
	deliverySystem[x][y].room = nRoom;
	strcpy(deliverySystem[x][y].passwd, passwd);		//duplicate passwd to deliverysystem's passwd 
	deliverySystem[x][y].context = msg;
	deliverySystem[x][y].cnt = sizeof(deliverySystem[x][y].context);

	if(deliverySystem[x][y].cnt <= 0) 
		return -1;		//If fail to push 

	
	storedCnt++; 	//If you succeed in putting, +1 to storedCnt

	
	return 0;
}



//extract the package context with password checking
//after password checking, then put the msg string on the screen and re-initialize the storage
//int x, int y : coordinate of the cell to extract
//return : 0 - successfully extracted, -1 = failed to extract
int str_extractStorage(int x, int y) {
	if(inputPasswd(x,y) == 0)		// If input password is correct
	{
		printStorageInside(x,y);
		initStorage(x,y);
		storedCnt--;
	} 
	else
		return -1;		//If input password is not correct

	return 0;
}

//find my package from the storage
//print all the cells (x,y) which has my package
//int nBuilding, int nRoom : my building/room numbers
//return : number of packages that the storage system has
int str_findStorage(int nBuilding, int nRoom) {
	int x;
	int y;
	int cnt = 0;		//initialize cnt value
	
	for(x=0; x<systemSize[0]; x++)
	{
		for(y=0; y<systemSize[1]; y++) 
		{
			if(deliverySystem[x][y].building == nBuilding && deliverySystem[x][y].room == nRoom) 
			{
				printf(" -----------> Found a package in (%d, %d)\n", x, y);		// print row & column value of storage 
				cnt++;		// if finding package succeed, cnt plus 1
			}
		}
	}
	
	return cnt;
}
