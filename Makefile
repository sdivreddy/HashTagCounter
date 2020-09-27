hashtagcounter: hashtagcounter.o
	g++ hashtagcounter.o -o hashtagcounter

hashtagcounter.o: hashTagCounter.cpp
	g++ -c hashTagCounter.cpp -o hashtagcounter.o

clean:
	rm *.o hashtagcounter
