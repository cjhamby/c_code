/* header file for Schedule and other types */
/* ECE 209 - Fall 2017                      */

#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <stdio.h>

#define NAME_LENGTH 20

/* time of day, assume 24-hour clock */
struct stime {
    int hr;  /* 0 - 23 */
    int min; /* 0 - 59 */
};
typedef struct stime Time;

/* information about a time interval */
struct iData {
    Time start;  /* beginning of interval */
    Time end;    /* end of interval */
    char owner[NAME_LENGTH+1];  /* ignore if not busy */
};

/* list node for interval */
struct iNode {
    struct iData interval;
    struct iNode * next;
};

/* entire schedule for one room */
struct schedule {
    struct iNode *idle;  /* list of idle intervals */
    struct iNode *busy;  /* list of busy intervals */
    Time start;  /* earliest time */
    Time end;    /* latest time */
};
typedef struct schedule * Schedule;

/* functions related to a schedule */

/* allocate and return a new schedule */
Schedule createSchedule(Time start, Time end);

/* check whether a desired period of time is available */
/* return 1 if busy, 0 if not busy */
int isBusy(Schedule s, Time start, Time end);

/* mark a particular time interval as busy, for specified owner */
/* return 1 if success, 0 if could not reserve */
int reserve(Schedule s, const char *name, Time start, Time end);

/* free up a previously-reserved interval */
/* merge contiguous idle intervals */
/* return 1 if success, 0 if matching interval not found */
int cancel(Schedule s, const char *name, Time start);

/* print all free and busy intervals to specified stream */
void printSchedule(Schedule s, FILE* stream);

#endif
