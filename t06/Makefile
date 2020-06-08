SHELL= /bin/bash
FLAGS= -Wall -Werror -fsanitize=address -g
OBJ = serialize.o deserialize.o helpers.o
DEPENDENCIES = helpers.h
.PHONY: clean all basic_test

basic_test: deserialize serialize
	@result=`(./serialize < test.dat) | ./deserialize`;\
	exit_state="$$?";\
	if [ "$$exit_state" == 0 ]; then\
		echo Basic test passed;\
	else\
		echo Basic test failed;\
	fi;

serialize : serialize.o helpers.o
	gcc ${FLAGS} -o $@ $^ 

deserialize : deserialize.o helpers.o
	gcc ${FLAGS} -o $@ $^

all : basic_test 

%.o : %.c ${DEPENDENCIES}
	gcc ${FLAGS} -c $<

clean :
	rm -f *.o serialize deserialize
