CPPFLAGS += -MMD -Iext -Wall -pthread
all: jsonhw

-include *.d

clean: 
	rm -f *.o jsonhw *~

mongoose.o: ext/mongoose.c
	gcc $(CPPFLAGS) -c $< -o $@

jsonhw: jsonhw.o jsonhelpers.o mongoose.o
	g++ jsonhw.o jsonhelpers.o mongoose.o -o $@ -ldl -pthread
