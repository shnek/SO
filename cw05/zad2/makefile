all:
	gcc rdfifo.c -Wall -o rdfifo
	gcc wrfifo.c -Wall -o wrfifo
	@echo ''
	@echo '	First run server ./rdfifo <file path>'
	@echo '	On no filepath default is /tmp/fifoTest'
	@echo '	Then in other terminal run ./wrfifo <file path>'
	@echo '	Replace <file path> with /tmp/fifoTest on default (must have args)'
	@echo ''

clean:
	rm -f rdfifo
	rm -f wrfifo