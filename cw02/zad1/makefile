SysGen = systemFuncGenerator
LibGen = libFuncGenerator

make:
	gcc $(LibGen).c -Wall -o $(LibGen)
	gcc $(SysGen).c -o $(SysGen)
	gcc libBubbleSort.c -Wall -o libBubble
	gcc sysBubbleSort.c -o sysBubble

test:
	make
	./$(SysGen) test1.txt 4 2048
	cp test1.txt test2.txt
	./libBubble test1.txt 4
	./sysBubble test2.txt 4

	./$(SysGen) test1.txt 512 2048
	cp test1.txt test2.txt
	./libBubble test1.txt 512
	./sysBubble test2.txt 512 

	./$(SysGen) test1.txt 4092 2048
	cp test1.txt test2.txt
	./libBubble test1.txt 4092
	./sysBubble test2.txt 4092

	./$(SysGen) test1.txt 8192 2048
	cp test1.txt test2.txt
	./libBubble test1.txt 8192
	./sysBubble test2.txt 8192

clean:
	rm $(LibGen) $(SysGen) libBubble sysBubble