/* header file for room functions and types */
/* ECE 209 - Fall 2017                      */

#ifndef ROOMS_H
#define ROOMS_H

#include "schedule.h"
#include <stdio.h>

/* information about a single reservation */
/* const pointer means that owner of this struct
 * is not allowed to change the interval's data */
struct reservation {
    int roomNumber;
    const struct iData * interval;
};

/* build a list of reservations */
struct rNode {
    struct reservation res;
    struct rNode *next;
};

/* allocate storage for desired number of rooms,
 * and initialize room schedules
 * (all rooms have the same schedule)
 */
void setupRooms(int nrooms, Time open, Time close);

/* function to return number of rooms --
 * must match value used in setupRooms call */
int numRooms();

/* reserve a time in the first available room */
/* return 1 if success, 0 if time not available in any room */
int makeReservation(const char* name, Time start, Time end);

/* delete an existing reservation in a particular room */
/* return 1 if success, 0 if matching reservation not found */
int cancelReservation(int room, const char* name, Time start);

/* create and return a list of reservations with the
 * specified owner, NULL (empty list) if no reservations */
struct rNode * findReservations(const char * name);

/* print reservations from a list, using specified stream */
void printReservations(const struct rNode* list, FILE* stream);

/* print schedule for every room, using specified stream */
void printRoomSchedules(FILE* stream);

#endif
