#----------------------------------------------
#  Makefile for compiling C++ programs of BouncingBall
#  Student Name: Yang Guo
#----------------------------------------------
#
#  Programmer: Donald House
#  Date: March 8, 1999
#
#  Copyright (C) - Donald H. House. 2005
#

CC      = g++
C	= cpp
H	= h
CFLAGS 	= -g

ifeq ("$(shell uname)", "Darwin")
  LDFLAGS     = -framework Foundation -framework GLUT -framework OpenGL -lm
else
  ifeq ("$(shell uname)", "Linux")
    LDFLAGS     = -lglut -lGL -lm -L/usr/local/lib -lGLU
  endif
endif

HFILES 	= Matrix.${H} Vector.${H} Utility.${H} Camera.${H} gauss.${H} Quaternion.${H} RBState.${H} RBStateDot.${H}
OFILES 	= Matrix.o Vector.o Utility.o Camera.o  gauss.o Quaternion.o RBState.o RBStateDot.o
PROJECT = RigidBody

${PROJECT}:	${PROJECT}.o $(OFILES)
	${CC} $(CFLAGS) -o ${PROJECT} ${PROJECT}.o $(OFILES) $(LDFLAGS)

${PROJECT}.o: ${PROJECT}.${C} $(HFILES)
	${CC} $(CFLAGS) -c ${PROJECT}.${C}

Camera.o: Camera.${C} Camera.${H} Matrix.${H} Vector.${H} Utility.${H} 
	${CC} $(CFLAGS) -c Camera.${C}

Matrix.o: Matrix.${C} Matrix.${H} Vector.${H} Utility.${H} 
	${CC} $(CFLAGS) -c Matrix.${C}

Vector.o: Vector.${C} Vector.${H} Utility.${H} 
	${CC} $(CFLAGS) -c Vector.${C}

Utility.o: Utility.${C} Utility.${H}
	${CC} $(CFLAGS) -c Utility.${C}

gauss.o: gauss.${C} gauss.${H}
	${CC} $(CFLAGS) -c gauss.${C}

Quaternion.o: Quaternion.${C} Quaternion.${H}
	${CC} $(CFLAGS) -c Quaternion.${C}

RBState.o: RBState.${C} RBState.${H}
	${CC} $(CFLAGS) -c RBState.${C}

RBStateDot.o: RBStateDot.${C} RBStateDot.${H}
	${CC} $(CFLAGS) -c RBStateDot.${C}	


debug:
	make 'DFLAGS = /usr/lib/debug/malloc.o'

clean:
	rm -f *.o *~ ${PROJECT}
