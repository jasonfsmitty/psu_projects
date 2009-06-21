
TARGET=p3

SRCS = \
	proj3.cpp \
	texture.c

CPP_OBJS = $(SRCS:.cpp=.o)
OBJS     = $(CPP_OBJS:.c=.o)

CFLAGS  = -D_THREAD_SAFE -g -Wall -Winline -D_GNU_SOURCE
LDFLAGS = -lglut

default: $(TARGET)

clean:
	rm -rf *.o $(TARGET)

.c.o:
	gcc -c $(CFLAGS) $< -o $@

.cpp.o:
	g++ -c $(CFLAGS) $< -o $@

$(TARGET): $(OBJS)
	gcc $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

