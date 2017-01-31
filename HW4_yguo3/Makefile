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

HFILES 	= Matrix.${H} Vector.${H} Utility.${H} Camera.${H} Particle.${H} gauss.${H} Strut.${H} Face.${H}
OFILES 	= Matrix.o Vector.o Utility.o Camera.o Particle.o gauss.o Strut.o Face.o
PROJECT = SpringyMeshV2

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

Particle.o: Particle.${C} Particle.${H}
	${CC} $(CFLAGS) -c Particle.${C}

Strut.o: Strut.${C} Strut.${H}
	${CC} $(CFLAGS) -c Strut.${C}

Face.o: Face.${C} Face.${H}
	${CC} $(CFLAGS) -c Face.${C}

debug:
	make 'DFLAGS = /usr/lib/debug/malloc.o'

clean:
	rm -f *.o *~ ${PROJECT}
