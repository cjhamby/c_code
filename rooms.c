/* Program 3 - rooms.c
 * Christopher Hamby
 *
 * 1-Dec-2017
 *
 * defines the functions outlined in the rooms header file
 * 
 */

#include "rooms.h"
#include <stdlib.h>
#include <string.h>
Schedule* mySchedules;		/*points to the top of the Schedule list */
struct rNode* myReservations;	/*points to the list of all reservations*/
struct rNode* lastRes;			/*points to the bottom of that list*/
int nRooms;



/* allocate storage for desired number of rooms,
 * and initialize room schedules
 * (all rooms have the same schedule)
 */
void setupRooms(int nrooms, Time open, Time close){
	int i;
	myReservations = (struct rNode*)malloc(sizeof(struct rNode));
	lastRes = myReservations;
	
	nRooms = nrooms;									/*assign the value to the global variable */
	mySchedules = (Schedule *)malloc(sizeof(Schedule)* nrooms); 	/* an array of pointers to pointers */
	myReservations = (struct rNode*)malloc(sizeof(struct rNode));	/* always points to the top of the list */
	for(i=0; i<nrooms; i++) {
		mySchedules[i] = createSchedule(open, close);		/* point to a new Schedule pointer */
	}
}




int numRooms(){
	return nRooms;
}




/* reserve a time in the first available room */
/* return the room number if successful, 0 if time not available in any room */
int makeReservation(const char* name, Time start, Time end) {
	int i;
	struct rNode* newRes = (struct rNode*)malloc(sizeof(struct rNode));
	struct iData * newDeets = (struct iData*)malloc(sizeof(struct iData));
	for(i=0; i<nRooms; i++)  {
		if(reserve(mySchedules[i], name, start, end)) {
			newRes->res.roomNumber = (i+1);		/*store the reservation to the res list */
			newDeets->start = start;				/*set some values in a new iData struct*/
			newDeets->end = end;
			strcpy(newDeets->owner, name);
			newRes->res.interval = newDeets;		/*store the iData struct to the reservation */

			/*add the new reservation to myReservations list */
			if(myReservations->res.interval == NULL) {		/*this is the first reservation*/
				myReservations = newRes;
				lastRes = myReservations;
			}
			else {
				lastRes->next = newRes;			/*add newRes to the bottom of the list */
				lastRes = newRes;
			}
			return (i+1);							/*room number = array index+1 */
		}
	
	}	
	return 0;
}


/* delete an existing reservation in a particular room */
/* return 1 if success, 0 if matching reservation not found */
int cancelReservation(int room, const char* name, Time start){
	struct rNode* scanner = myReservations;
	struct rNode* nextRes = scanner;
	if(scanner->res.interval == NULL)	return 0;	/*no reservations even exist! */
	
	if(cancel(mySchedules[room-1],name,start)) {	/*we removed the res from that room's schedule  */
		nextRes = scanner->next;
		if(nextRes == NULL) {		/*we can assume that the cancelled res. was the only 1 */
			scanner->res.interval = NULL;
			return 1;
		}
		
		if(scanner->res.roomNumber == room)	/*removing the first reservation on the list */
			if(scanner->res.interval->start.hr == start.hr)
				if(scanner->res.interval->start.min == start.min)
					if(!strcmp(nextRes->res.interval->owner, name)) {
						myReservations = myReservations->next;

						return 1;
					}		

		while(nextRes->res.interval != NULL) {	/*we have to remove the res. from the res. list, too */
			if(nextRes->res.roomNumber == room)
				if(nextRes->res.interval->start.hr == start.hr) 
					if(nextRes->res.interval->start.min == start.min) 
						if(!strcmp(nextRes->res.interval->owner, name)) {
							scanner->next = nextRes->next;
							return 1;
						}
			scanner = nextRes;			/*increment and repeat*/
			nextRes = nextRes->next;
		}
	}
	return 0;
}





/* create and return a list of reservations with the
 * specified owner, NULL (empty list) if no reservations */
struct rNode * findReservations(const char * name){
	struct rNode *myResList =(struct rNode*)malloc(sizeof(struct rNode));	/*the top of the res. list */
	struct rNode *bottomRes = myResList;								/*the bottom of the res. list */
	struct rNode *checkRes = myReservations;							/*the res. we're checking */
	struct rNode *copyRes = (struct rNode*)malloc(sizeof(struct rNode));
	
	if(checkRes->res.interval == NULL)
		return NULL;
	while(checkRes!=NULL) {
		if(!strcmp(checkRes->res.interval->owner, name)) {			/*it's a match! */
			copyRes = (struct rNode*)malloc(sizeof(struct rNode));	/*make a copy to add to the res. list*/
			copyRes -> res = checkRes->res;						/*we don't want to change original */	
			if(myResList->res.interval == NULL) {			/*this is first reservation on the new list */
				myResList=copyRes;			/*so we make it the beginning of the list */
				bottomRes = myResList;
			}
			else {
				bottomRes->next=copyRes;		/*not the first reservation on the list */
				bottomRes = bottomRes->next;	/*so we add it to the bottom */
			}
		}
		checkRes = checkRes->next;				/*check every reservation made */		
	}
	if(myResList->res.interval ==NULL)			/*no reservations were added to the list */
		return NULL;							/*indicates that none were found for the name */
	return myResList;
}


/* print reservations from a list, using specified stream */
/* list points to the top of a busy list (idle intervals' ower fields aren't cleared) */
void printReservations(const struct rNode* list, FILE* stream) {
	const struct rNode* temp = list;	
	while(temp !=NULL) {
		fprintf(stdout, "ROOM %d:", temp->res.roomNumber);		/*print the room number */
		if(temp->res.interval->start.hr <10)					/*print the time range */
			fprintf(stdout, "0");								/*properly formatted, of course */
		fprintf(stdout, "%d:", temp->res.interval->start.hr);
		if(temp->res.interval->start.min <10)
			fprintf(stdout, "0");
		fprintf(stdout, "%d - ", temp->res.interval->start.min);
		if(temp->res.interval->end.hr <10)
			fprintf(stdout, "0");
		fprintf(stdout, "%d:", temp->res.interval->end.hr);
		if(temp->res.interval->end.min <10)
			fprintf(stdout, "0");
		fprintf(stdout, "%d ", temp->res.interval->end.min);
		fprintf(stdout, "%s\n", temp->res.interval->owner);		/*print the room owner*/

		
		temp = temp->next;
	}
	
}

/* print schedule for every room, using specified stream */
void printRoomSchedules(FILE* stream) {
	int i;
	for(i=0; i<nRooms; i++) {
		printf("\nRoom %d\n", (i+1));
		printSchedule(mySchedules[i], stream);
	}	
}
