CC=gcc
LibName=PersonList
Remove= rm -f *.o
all:
	make static
	make dynamic

static: $(LibName).o
	ar crs lib$(LibName).a $(LibName).o
	$(Remove)

dynamic: $(LibName).c
	$(CC) -fPIC -c -Wall $(LibName).c
	$(CC) -shared -fPIC -o lib$(LibName).so $(LibName).o
	$(Remove)

clear:
	$(Remove) *.a *.so