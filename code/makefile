CC=g++
OPTIONS=-O0 -Wall

INCDIR=./inc
SRCDIR=./src
OBJDIR=./obj
LIBDIR=./lib

INCS=$(addprefix -I, $(INCDIR))
SRCS=$(wildcard $(SRCDIR)/*.cpp)
OBJS=$(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

COMMAND = $(CC) $(OPTIONS) $(INCS)

TARGET=Chip8Emulator

all: $(TARGET)

$(TARGET): $(OBJS)
	$(COMMAND) -o $@ $(OBJS)

$(OBJS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	$(COMMAND) -c $< -o $@

clean:
	rm -f $(OBJS)
	rm -f $(TARGET)
