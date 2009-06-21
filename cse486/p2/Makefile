#---------------------------------------------------
#  Uses GLUT 3.6
#  Original makefile from Paul Plaussman (CSE 418)
#  Modified by Jason F Smith
#  Penn State University
#  October 10, 2000
#---------------------------------------------------
TARGET = p2
SRCS =	$(TARGET).cpp

RM = -rm -rf

CC = g++

# For now, use GLUT installed in cg418 directory
GLUT_DIR      = /home/abyss.3/cg418/glut-3.6
GL_INC        = /usr/openwin/include
GLUT_LIB_DIR  = $(GLUT_DIR)/lib/glut/
GLUT_INC      = $(GLUT_DIR)/include/
LD_LIBS   = -L$(GLUT_LIB_DIR) -lglut -lGLU -lGL -lXmu -lXext -lX11 -lm
CFLAGS    = -I$(GLUT_INC) -I$(GL_INC) -g -c

OBJS =	$(SRCS:.cpp=.o)

.c.o:
	($(CC) $(CFLAGS)  $<)

$(TARGET): $(OBJS)
	$(CC) -o $@ myString.cpp $(OBJS) $(LD_LIBS)

default : $(TARGET)

clean:
	-$(RM) $(OBJS) $(TARGET) *~

