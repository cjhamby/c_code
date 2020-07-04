/* Program 3 - schedule.c
 * Christopher Hamby
 *
 * 1-Dec-2017
 *
 * defines the functions outlined in the schedule header file
 * 
 */

#include "schedule.h"
#include <stdlib.h>
#include <string.h>


/*my defined functions */
int timeCompare(Time t1, Time t2);				/*verified */
int timeConflict(struct iNode* r1, struct iNode* r2);	/*verified */
int isBefore(struct iNode *r1, struct iNode *r2);		/*verified */
int isAfter(struct iNode *r1, struct iNode *r2);		/*verified */
int touching(struct iNode* r1, struct iNode* r2);

void fprintTime(Time t, FILE* stream);
void fprintInterval(struct iNode* range, FILE* stream);

struct iNode* insert(struct iNode *new, struct iNode *list);
struct iNode* insertIdle(struct iNode *new, struct iNode *start);
struct iNode* removeIdle(struct iNode *new, struct iNode *start);
struct iNode* merge(struct iNode* r1, struct iNode *r2);




/* allocate and return a new schedule */
Schedule createSchedule(Time start, Time end){
	/*allocate space for the new schedule */
	Schedule newGuy = (Schedule)malloc(sizeof(struct schedule));
	
	/*note that the schedule structure only contains space for a pointer */
	/*so we have to allocate space for the actual iNode as well */
	/*make the idle interval*/
	struct iNode* idleGuy = (struct iNode*)malloc(sizeof(struct iNode));
	idleGuy->interval.start = start;
	idleGuy->interval.end = end;

	/*even though it has no items yet, we have to allocate the first busy node*/
	/*struct iNode busyGuy = (struct iNode*)malloc(sizeof(struct iNode));*/
	/*assign values to the schedule struct*/
	newGuy->idle = idleGuy;
	newGuy->start = start;
	newGuy->end = end;

	/*return a pointer to the new struct schedule */
	return newGuy;
}




/* check whether a desired period of time is available */
/* return 1 if busy, 0 if not busy */
int isBusy(Schedule s, Time start, Time end) {
	struct iNode *busyNode = s->busy;		/*the top item on the busy interval list , maybe NULL */

	/*newNode contains the desired time range we want to reserve */
	struct iNode* newNode = (struct iNode *)malloc(sizeof(struct iNode));
	newNode->interval.start = start;
	newNode->interval.end = end;


	while(busyNode != NULL) {				/*compare this range to the desired range */
		if(timeConflict(busyNode, newNode)) {	/*and check for a conflict between them */			
			return 1;		
		}
		busyNode = busyNode->next;		/*check every busy interval for a time conflict*/
	}
return 0;
}



/* mark a particular time interval as busy, for specified owner */
/* return 1 if success, 0 if could not reserve */
int reserve(Schedule s, const char *name, Time start, Time end) {
	struct iNode *reservation = (struct iNode *)malloc(sizeof(struct iNode));	/*the new reservation to add */
	/*assign values to the new reservation */
	reservation -> interval.start = start;	
	reservation -> interval.end = end;
	strcpy(reservation -> interval.owner, name);

	if(isBusy(s, start, end))	return 0;			/*checks for time conflicts with existing reservations */	
	if(timeCompare(start, s->start) == -1)	return 0;	/*can't reserve before the earliest time */			
	if(timeCompare(end, s->end) == 1)	return 0;	/*can't reserve after the latest time */

	
	s->busy = insert(reservation, s->busy);		/*add the new reservation to the list of busy intervals */
	s->idle = removeIdle(reservation, s->idle);	/*and take the time away from the idle list*/
	return 1;
}



/* free up a previously-reserved interval */
/* merge contiguous idle intervals */
/* return 1 if success, 0 if matching interval not found */
int cancel(Schedule s, const char *name, Time start) {
	struct iNode *prior = s->busy;	/*the interval before the removed one */
	struct iNode *old = s->busy;		/*the interval to remove */

	/*check that reservations exist */
	if(prior == NULL)	return 0;	
	
	/*check first reservation */
	if(!timeCompare(old->interval.start, start) && !strcmp(name, old->interval.owner)) { 		
		s->busy=old->next;						/* remove an interval by "skipping" over it */
		insertIdle(old, s->idle);						/* add the time slot back to idle */
		return 1;								/* report a success */
	}
	
	/*check next reservations */
	old = old->next;
	while(old!=NULL) {
		if(!timeCompare(old->interval.start, start)&& !strcmp(name, old->interval.owner)) {	
												/*we found the reservation to cancel */	
			prior->next = old->next;				/*skip over the removed interval */
			insertIdle(old, s->idle);					/*add the time back to idle */
			return 1;							/*report a success */
		}
		
		prior = old;		/*increment and repeat */
		old = old->next;	/*until we find the matching reservation */
	}
	return 0;	/*didn't find the matching interval */
}




/*print all free and busy intervals to specified stream*/
void printSchedule(Schedule s, FILE* stream) {
	Time currentTime = s->start;
	struct iNode *currentIdle = s->idle;
	struct iNode *currentBusy = s->busy;
	while(timeCompare(currentTime, s->end) < 0) {
		if(currentIdle != NULL) {
			/*ignore idle intervals that are 0 minutes long */
			if(timeCompare(currentIdle->interval.start, currentIdle->interval.end)==0) 
				currentIdle= currentIdle->next;
			else if(timeCompare(currentTime, currentIdle->interval.start)==0) {  /*Idle interval */
				fprintf(stream, "IDLE ");
				fprintInterval(currentIdle, stream);
				fprintf(stream, "\n");
				currentTime = currentIdle->interval.end;
				currentIdle = currentIdle->next;
			}
			
		}
		if(currentBusy != NULL) {
			if(timeCompare(currentTime, currentBusy->interval.start)==0) {	/*Busy interval */
				fprintf(stream, "BUSY ");
				fprintInterval(currentBusy, stream);
				fprintf(stream, " %s\n", currentBusy->interval.owner);
				currentTime = currentBusy->interval.end;
				currentBusy = currentBusy->next;
			}
		}
	}
}





/*add a new interval to the busy list 
  return a pointer to the beginning of the new list
  REQUIREMENTS:	-the list remains sorted
  				-the new interval doesn't conflict with existing reservations
  CONDITIONS:	-start is the beginning of the busy list
				-new is the node to be added
*/
struct iNode* insert(struct iNode *new, struct iNode *start) {
	struct iNode* prior = start;	/*compare these to the new interval to determine proper placement */
	struct iNode* latter;		/*prior is earlier than new, latter is later than  new*/
	int comp1 = 0;
	int comp2 = 0;

	/*if the list has no items, then "new" becomes the top item on the list*/
	if(prior == NULL)	return new;
	
	if(isBefore(new, prior)) { 	/*check if the new reservation is the earliest */
		new->next = prior;		/*make the new start point to the prior start */
		return new;			/*and return the new start of the list */
	}
		

	/*if there are more than one element in the list already, we need to make sure that 
	  the new reservation is in between the two reservations which we insert it between */
	latter = prior->next;
	while(latter != NULL) {			
		comp1 = isAfter(new, prior);
		comp2 = isBefore(new, latter); 
		
		if(comp1 && comp2) {		/*we found where to put the new interval */
			prior->next = new;
			new->next = latter;
			return start;
		}
		prior = latter;			/*increment and repeat */
		latter = latter->next;	/*until we find where to put the new interval */
	}
	
	prior->next = new;			/*if we reach the end of the loop, the new reservation is the latest */
	return start;
}








/*	when we add a new reservation, we have to remove that time range from the idle list 
	find the idle interval which contains [i.e. conflicts with] the reservation 
	and split it into two parts */
struct iNode* removeIdle(struct iNode *new, struct iNode *start) {
	struct iNode *iRange1 = start;	/*this will eventually be the part of the range before new*/
	struct iNode *iRange2 = (struct iNode*) malloc(sizeof(struct iNode)); 	/*the part after new*/

	while(!timeConflict(iRange1, new)) 
		iRange1 = iRange1->next;					/*find where new belongs */
	
	
	iRange2->interval.start = new->interval.end;		/*split the interval where new divides it*/
	iRange2->interval.end = iRange1->interval.end;		/*iRange2 comes after new */
	iRange2->next = iRange1->next;
	iRange1->interval.end = new->interval.start;		/*iRange1 comes before new*/
	iRange1->next = iRange2;
	
	return start;
}



/*when a reservation is cancelled, we have to add that time to the idle list
  find where the reservation cut the idle list
  and repair the hole
  
  new points to the time interval node to be added to the idle list
  
  REQUIREMENTS: 	-the time interval of "new" shares no content with other intervals on idle
  			 	-it may share a start or end time though
  			 	-the idle list is already sorted
  */
struct iNode *insertIdle(struct iNode *new, struct iNode *start) {
	struct iNode *prior = start;
	struct iNode *latter = start;
	int comp1;
	int comp2;

	if (prior == NULL) 					/*indicates that new is the only idle interval */
		return new;
		
	latter = prior->next;
	if (latter == NULL) {				/* indicates that there is only one interval in the idle list */
		comp1 = isBefore(prior, new);		/*prior comes before new */
		if(comp1==2) 						/*the two intervals "touch" */
			return merge(prior, new);
		if(comp1==1) {						/*the two intervals don't touch */
			prior->next = new;					/*add new to the end of the list */
			return prior;
		}	
		else{							/*new comes before prior */
			if(isAfter(prior, new) == 2) 			/*the two intervals touch */
				return merge(new, prior);		/*merge them together */
			
			else {							/*the two intervals don't touch*/
				new->next = prior;				/*put new at the beginning of the list*/
				return new;
			}
		}
	}
	

	comp1 = isBefore(new, prior);		/*indicates that there are 2+ idle intervals */
	if(comp1 == 2) {						/*new comes first in the list, and touches prior*/
		return merge(new, prior);			/*the merged combo becomes the top of the list */
	}
	if(comp1 == 1) {						/*new comes first in the list, doesn't touch prior */
		new->next = prior;					/*new becomes the top of the list */
		return new;
	}
									/*new doesn't come first in the list*/
	while(latter != NULL) {					/*we have to find the intervals which new fits between */	
		comp1 = isBefore(new, latter);			/*does new come before latter? 	*/
		comp2 = isAfter(new, prior);				/*does new come after prior?		*/
		if (comp1 && comp2) {						/* yes */
			if(comp1 == 2 && comp2 == 2) {				/*new touches both prior and latter */
				new = merge(new, latter);
				prior = merge(prior, new);
				return start;
			}
			else if(comp1 == 2) {						/*new touches latter only*/
				new = merge(new, latter);				
				prior->next = new;
				return start;
			}
			else if(comp2 == 2) {						/*new touches prior only*/
				new->next = latter;
				prior = merge(prior, new);
				return start;
			}			
			else{									/*the intervals don't touch at all */
				prior->next = new;
				new->next = latter;
				return start;
			}						
			prior->next = new;						/* no */
			new->next= latter;							/* increment, repeat */
		}	
		prior = latter;				/*increment and repeat */
		latter = prior->next;
	}
	


	/* if we reach this point, it means that prior is the end of the list */
	comp1 = isBefore(prior, new);
	if(comp1 == 2)
		prior = merge(prior, new);
	else
		prior->next = new;
	return start;
}




/*prints a properly formatted time	(hh:mm) */
/*an important caveat is that the inputs are backwards from standard fprintf */
/*sorry about that */
void fprintTime(Time t, FILE* stream) {
	if(t.hr < 10)			/*ensure proper formatting of the hour */
		fprintf(stream, "0");
	fprintf(stream, "%d:", t.hr);
	if(t.min<=10)
		fprintf(stream, "0");
	fprintf(stream, "%d", t.min);
}



/*prints the time interval from a given node pointer	*/
/*the line break can be removed if necessary 	*/
void fprintInterval(struct iNode* range, FILE* stream) {
	fprintTime(range->interval.start, stream);
	fprintf(stream, " - ");
	fprintTime(range->interval.end, stream);
}





/*return -1 if t1 is earlier than t2
  return 1 if t1 is later than t2
  return 0 if the times are the same
 */
int timeCompare(Time t1, Time t2) {
	if(t1.hr < t2.hr)	return -1;
	if(t1.hr > t2.hr)	return 1;
	if(t1.min < t2.min)	return -1;
	if(t1.min > t2.min) return 1;
	return 0;
}





/*	return 0 if r1 is NOT before r2	
	return 1 if r1 is before r2
	return 2 if r1 is before r2 and they share a time value ("touch")
	return -1 if either r2 is NULL
*/
int isBefore(struct iNode *r1, struct iNode *r2) {
	int temp = 0;
	if(r1==NULL || r2 ==NULL)	return -1;

	temp= timeCompare(r1->interval.end, r2->interval.start);
	if(temp < 0) return 1;
	if(temp == 0) return 2;
	return 0;
}

/*	see isBefore(...)	*/
int isAfter(struct iNode *r1, struct iNode *r2) {
	return isBefore(r2, r1);	
}




/*	return 0 if the time slot is free
	return 1 if there is a time conflict
	return -1 if either Time is NULL
*/
int timeConflict(struct iNode * r1, struct iNode * r2) {
	if(r1 == NULL || r2 == NULL) return -1;
	return(!isBefore(r1, r2) && !isAfter(r1, r2));
}


/*	returns 1 if the intervals share a common time */
int touching(struct iNode* r1, struct iNode* r2) {
	return (!timeCompare(r1->interval.end, r2->interval.start) || !timeCompare(r1->interval.start, r2->interval.end));
}



/*	merge two intervals together
	returns a pointer to the new interval
	REQUIREMENTS:	r1 touches r2
*/
struct iNode* merge(struct iNode* r1, struct iNode *r2) {
	if(isAfter(r1, r2)>0)
		return merge(r2, r1);
	if(!touching(r1, r2))		/*we can't merge two intervals that don't touch */
		return r1;		/*just return the first one*/
	r1->interval.end = r2->interval.end;
	r1->next = r2->next;
	return r1;
}





