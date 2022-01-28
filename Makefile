CC = gcc
CXX = g++
ECHO = echo
RM = rm -f

TERM = "\"S2019\""

CFLAGS = -ggdb -funroll-loops -DTERM=$(TERM)
CXXFLAGS = -ggdb -funroll-loops -DTERM=$(TERM)

LDFLAGS = -lncurses 

BIN = dungeon
OBJS = dungeon.o pathfind.o heap.o monster.o mparser.o oparser.o dice.o objects.o equipment_io.o combat.o spells.o 
all: $(BIN) etags

$(BIN): $(OBJS)
	@$(ECHO) Linking $@
	@$(CXX) $^ -o $@ $(LDFLAGS)

-include $(OBJS:.o=.d)

%.o: %.c
	@$(ECHO) Compiling $<
	@$(CC) $(CFLAGS) -MMD -MF $*.d -c $<

%.o: %.cpp
	@$(ECHO) Compiling $<
	@$(CXX) $(CXXFLAGS) -MMD -MF $*.d -c $<

.PHONY: all clean clobber etags

clean:
	@$(ECHO) Removing all generated files
	@$(RM) *.o $(BIN) *.d TAGS core vgcore.* gmon.out

clobber: clean
	@$(ECHO) Removing backup files
	@$(RM) *~ \#* *pgm

etags:
	@$(ECHO) Updating TAGS
	@etags *.[ch]
