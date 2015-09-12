all:
	gcc program.c -Wall -o program
	@echo ''
	@echo '	Program creates 2 testDirs, you may want add more'
	@echo '	Program works as "ls -l | grep ^d | wc -l"'
	@echo '	Run: ./program'
	@echo
	mkdir testDir1
	mkdir testDir2

clean:
	rm -f program
	rmdir testDir1
	rmdir testDir2
