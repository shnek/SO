#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<sys/times.h>
#include<unistd.h>

#ifndef dynamicLoadedMacro
#include"../zad1/PersonList.h"
#endif

#ifdef dynamicLoadedMacro
#include<dlfcn.h>
struct List *List;
void *Library;
struct List * (*createList)();
void (*addPerson)(struct List *, char*, char*, char*, char*, int first, int last);
void (*sortPersonList)(struct List *);
void (*deletePerson)(struct List *, char*, char*);
const char *error;
#endif

int main(){
	struct tms tStruct;
	clock_t startTime = clock();
	
	printf("Start time\n");
	printf("real: \t%f\n", ((double)(clock() - startTime)/(double)(CLOCKS_PER_SEC)));
	times(&tStruct);
	printf("user: \t%f\n", ((double)(tStruct.tms_utime) / (double)(sysconf(_SC_CLK_TCK))));
	printf("sys: \t%f\n\n", ((double)(tStruct.tms_stime) / (double)(sysconf(_SC_CLK_TCK))));

#ifdef dynamicLoadedMacro
	Library = dlopen("libPersonList.so", RTLD_LAZY);
	error = dlerror();
	if(error) return 1;
	createList = dlsym(Library, "createList");
	error = dlerror();
	if(error) return 2;
	addPerson = dlsym(Library, "addPerson");
	error = dlerror();
	if(error) return 3;
	sortPersonList = dlsym(Library, "sortPersonList");
	error = dlerror();
	if(error) return 4;
	deletePerson = dlsym(Library, "deletePerson");
	error = dlerror();
	if(error) return 5;
#endif

	struct List *list;
	list = createList();

	printf("Created structure time\n");
	printf("real: \t%f\n", ((double)(clock() - startTime)/(double)(CLOCKS_PER_SEC)));
	times(&tStruct);
	printf("user: \t%f\n", ((double)(tStruct.tms_utime) / (double)(sysconf(_SC_CLK_TCK))));
	printf("sys: \t%f\n\n", ((double)(tStruct.tms_stime) / (double)(sysconf(_SC_CLK_TCK))));
	int i = 0;
	for(i = 0; i < 1000; i++){
	addPerson(list, "Janek", "Kowalski", NULL, NULL, 19940811, 0);
	addPerson(list, "Wojtek", "Majewski", NULL, NULL, 19841103, 0);
	addPerson(list, "Kuba", "Wojewodzki", NULL, NULL, 20010818, 0);
	addPerson(list, "Michal", "Wisniweski", NULL, NULL, 19970409, 0);
	addPerson(list, "Romuald", "Wielki", NULL, NULL, 19680402, 0);
	addPerson(list, "Wojtek", "Kowalski", NULL, NULL, 19940611, 0);
	addPerson(list, "Piotrek", "Majewski", NULL, NULL, 19841104, 0);
	addPerson(list, "Grzesiek", "Wojewodzki", NULL, NULL, 20010819, 0);
	addPerson(list, "Dawid", "Wisniweski", NULL, NULL, 19970402, 0);
	addPerson(list, "Jurek", "Wielki", NULL, NULL, 19680404, 0);
	addPerson(list, "Patryk", "Kowalski", NULL, NULL, 19940810, 0);
	addPerson(list, "Wiktor", "Majewski", NULL, NULL, 19841129, 0);
	addPerson(list, "Martyna", "Wojewodzki", NULL, NULL, 20010830, 0);
	addPerson(list, "Georgia", "Wisniweski", NULL, NULL, 19970414, 0);
	addPerson(list, "Hubert", "Wielki", NULL, NULL, 196804015, 0);
	addPerson(list, "Genowefa", "Kowalski", NULL, NULL, 19940813, 0);
	addPerson(list, "Bratyslawa", "Majewski", NULL, NULL, 19841123, 0);
	addPerson(list, "Indiana", "Wojewodzki", NULL, NULL, 20010810, 0);
	addPerson(list, "Ryszard", "Wisniweski", NULL, NULL, 19970401, 0);
	addPerson(list, "Zygmunt", "Wielki", NULL, NULL, 19680403, 0);
	addPerson(list, "Victor", "Kowalski", NULL, NULL, 19940831, 0);
	addPerson(list, "Oscar", "Majewski", NULL, NULL, 19841107, 0);
	addPerson(list, "Oskar", "Wojewodzki", NULL, NULL, 20000818, 0);
	addPerson(list, "Witold", "Wisniweski", NULL, NULL, 19870409, 0);
	addPerson(list, "Nikodem", "Wielki", NULL, NULL, 19650402, 0);
	}	
	
	printf("Inserted data of people\n");
	printf("real: \t%f\n", ((double)(clock() - startTime)/(double)(CLOCKS_PER_SEC)));
	times(&tStruct);
	printf("user: \t%f\n", ((double)(tStruct.tms_utime) / (double)(sysconf(_SC_CLK_TCK))));
	printf("sys: \t%f\n\n", ((double)(tStruct.tms_stime) / (double)(sysconf(_SC_CLK_TCK))));
	
	sortPersonList(list);


	printf("After sorting people\n");
	printf("real: \t%f\n", ((double)(clock() - startTime)/(double)(CLOCKS_PER_SEC)));
	times(&tStruct);
	printf("user: \t%f\n", ((double)(tStruct.tms_utime) / (double)(sysconf(_SC_CLK_TCK))));
	printf("sys: \t%f\n\n", ((double)(tStruct.tms_stime) / (double)(sysconf(_SC_CLK_TCK))));

	for(i = 0; i < 500; i++){
		deletePerson(list, "Janek", "Kowalski");
	}	
#ifdef dynamicLoadedMacro
	dlclose(Library);
#endif
	printf("After deleting 500 people (end time)\n");
	printf("real: \t%f\n", ((double)(clock() - startTime)/(double)(CLOCKS_PER_SEC)));
	times(&tStruct);
	printf("user: \t%f\n", ((double)(tStruct.tms_utime) / (double)(sysconf(_SC_CLK_TCK))));
	printf("sys: \t%f\n\n", ((double)(tStruct.tms_stime) / (double)(sysconf(_SC_CLK_TCK))));

	return 0;
} 