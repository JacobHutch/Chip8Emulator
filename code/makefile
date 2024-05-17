INCDIR := ./inc
SRCDIR := ./src
OBJDIR := ./obj
EXTDIR := ./ext

GLADDIR := $(EXTDIR)/glad
GLMDIR := $(EXTDIR)/glm
GLFWDIR := $(EXTDIR)/glfw

DEPSRCS := $(GLADDIR)/src/glad.c
DEPLIBS := -L$(GLFWDIR)/lib-mingw-w64 -lglfw3 -lgdi32 -static-libstdc++ -static-libgcc

COMPILER := x86_64-w64-mingw32-g++
INCLUDES := -I$(INCDIR) -I$(GLADDIR)/include -I$(GLMDIR) -I$(GLFWDIR)/include
OPTIONS := -std=c++17 -Wall -Werror $(INCLUDES) -g

SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

COMMAND := $(COMPILER) $(OPTIONS)

TARGET := Chip8Emulator.exe



all: $(TARGET)



$(TARGET): $(OBJS)
	$(COMMAND) -o $@ $(OBJS) $(DEPSRCS) $(DEPLIBS)


$(OBJS): $(INCDIR)/*.h
$(OBJS): $(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(COMMAND) -c $< -o $@



clean:
	rm -f $(OBJDIR)/*
	rm -f $(TARGET)



reset: clean all
