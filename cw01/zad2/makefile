CC=gcc
LibName=PersonList
Remove= rm -f *.o

all:
	make static
	make dynamic
	make dynamicLoadedLibrary

static: $(LibName)Test.o ../zad1/lib$(LibName).a
	$(CC) $(LibName)Test.o -static -o testingStatic -L../zad1/ -l$(LibName) 
	$(Remove)

dynamic: $(LibName)Test.o ../zad1/lib$(LibName).so
	$(CC) $(LibName)Test.o -o testingDynamic -L../zad1/ -I. -Wl,-rpath=../zad1/ -l$(LibName) 	
	$(Remove)

dynamicLoadedLibrary: $(LibName)Test.c ../zad1/lib$(LibName).so
	$(CC) -D dynamicLoadedMacro $(LibName)Test.c -o testingDynamicLoaded -Wl,-rpath=../zad1/ -ldl

clear: 
	$(Remove) testing*

../zad1/lib$(LibName).a:
	@echo "\n\tPlease run 'make static' in zad1 directory first!\n"

../zad1/lib$(LibName).so:
	@echo "\n\tPlease run 'make dynamic' in zad1 directory first!\n"