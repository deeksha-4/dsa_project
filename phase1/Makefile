#------------------------
# Do not modify this file
#------------------------

all: trader iml

trader: trader.cpp receiver.h
	g++ -std=c++20 trader.cpp -o trader

iml: iml.cpp
	g++ -std=c++20 iml.cpp -o iml

runpart1: iml trader
	./trader 1 > output.txt &
	sleep 1
	./iml inputs/data1.txt
	diff outputs/out1.txt output.txt

runpart2: iml trader
	./trader 2 > output.txt &
	sleep 1
	./iml inputs/data2.txt
	diff outputs/out2.txt output.txt

runpart3: iml trader
	./trader 3 > output.txt &
	sleep 1
	./iml inputs/data3.txt
	diff outputs/out3.txt output.txt

clean:
	rm -rf output.txt trader iml
	rm -rf *~

clean_part1:
	rm -rf output.txt trader iml
	rm -rf *~
	g++ -std=c++20 trader.cpp -o trader
	g++ -std=c++20 iml.cpp -o iml
	./trader 1 > output.txt &
	sleep 1
	./iml inputs/part12.txt
	diff outputs/outpart2.txt output.txt

clean_part2:
	rm -rf output.txt trader iml
	rm -rf *~
	g++ -std=c++20 trader.cpp -o trader
	g++ -std=c++20 iml.cpp -o iml
	./trader 2 > output.txt &
	sleep 1
	./iml inputs/data2.txt
	diff outputs/out2.txt output.txt

clean_part3:
	rm -rf output.txt trader iml
	rm -rf *~
	g++ -std=c++20 trader.cpp -o trader
	g++ -std=c++20 iml.cpp -o iml
	./trader 3 > output.txt &
	sleep 1
	./iml inputs/data3.txt
	diff outputs/out3.txt output.txt