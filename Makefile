TARGET = ogl

FLAGS = -O3 -march=native -std=c++14 -DTEXTURES
LIBS = -lglfw -framework OpenGL -lpng
CC = g++
LD = g++
LDFLAGS =

SRCDIR   = src
OBJDIR   = src

SOURCES  := $(wildcard $(SRCDIR)/*.cc)
OBJS  := $(SOURCES:$(SRCDIR)/%.cc=$(OBJDIR)/%.o)

all: $(TARGET)
 
$(TARGET): $(OBJS)
	@echo LD $<
	@$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OBJS): $(OBJDIR)/%.o : $(SRCDIR)/%.cc
	@echo CC $<
	@$(CC) $(FLAGS) -c -o $@ $<

clean::
	@if [ -n "$(OBJS)" ]; then rm -f $(OBJS); fi
	@if [ -f core ]; then rm -f core; fi
	@rm -f tags *.linkinfo

mrproper::
	@if [ -n "$(TARGET)" ]; then rm -f $(TARGET); fi
	@if [ -n "$(OBJS)" ]; then rm -f $(OBJS); fi
	@if [ -f core ]; then rm -f core; fi
	@rm -f tags *.linkinfo