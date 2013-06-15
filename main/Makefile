program_NAME := main
program_C_SRCS := $(wildcard *.c)
program_H_SRCS := $(wildcard *.h)

program_C_OBJS := ${program_C_SRCS:.c=.o}
program_OBJS := $(program_C_OBJS)

program_INCLUDE_DIRS := ../libs/include
program_LIBRARY_DIRS := ../libs/
program_LIBRARIES := regex


CPPFLAGS += -Wall -ggdb $(foreach includedir,$(program_INCLUDE_DIRS),-I$(includedir))
# Static libraries
LDFLAGS +=
LDFLAGS += $(foreach librarydir,$(program_LIBRARY_DIRS),-L$(librarydir))
LDFLAGS += $(foreach library,$(program_LIBRARIES),-l$(library))


.PHONY: all clean distclean

all: $(program_NAME)

clean:
	@- $(RM) ../bin/$(program_NAME)
	@- $(RM) $(program_OBJS)
	@- $(RM) *.d
	@- $(RM) $(program_NAME)

$(program_NAME): $(program_OBJS)
	$(CC) $(program_OBJS) -o $(program_NAME) -static $(LDFLAGS)
	@- mkdir -p ../bin/
	cp -u $(program_NAME) ../bin/$(program_NAME)

%.o : %.c
	$(CC) -M $(CPPFLAGS) $(CFLAGS) -o $*.P $<
	@cp $*.P $*.d; \
			sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
				-e '/^$$/ d' -e 's/$$/ :/' < $*.P >> $*.d; \
			rm -f $*.P
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

-include $(program_OBJS:.o=.d)
