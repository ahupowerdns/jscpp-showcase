CPPFLAGS += -Iext -Wall -pthread


all: jsonhw

mongoose.o: ext/mongoose.c
	gcc $(CPPFLAGS) -c $< -o $@

jsonhw: jsonhw.o mongoose.o
	g++ jsonhw.o mongoose.o -o $@ -ldl -pthread