CXXFLAGS = -std=c++11 -g -Wall
OBJS = name_walk.o

run.out: $(OBJS)
	g++ $(OBJS) -o run.out

clean:
	rm -rf run.out $(OBJS)

full:
	make clean
	make
