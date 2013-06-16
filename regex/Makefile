library_NAME := regex
library_C_SRCS := $(wildcard *.c)
library_H_FILES := $(wildcard *.h)
library_C_OBJS := ${library_C_SRCS:.c=.o}
library_OBJS := $(library_C_OBJS)
library_INCLUDE_DIRS :=
library_LIBRARY_DIRS :=
library_LIBRARIES :=

CPPFLAGS += -Wall $(foreach includedir,$(library_INCLUDE_DIRS),-I$(includedir))
LDFLAGS += $(foreach librarydir,$(library_LIBRARY_DIRS),-L$(librarydir))
LDFLAGS += $(foreach library,$(library_LIBRARIES),-l$(library))

all: lib$(library_NAME).a

lib$(library_NAME).a: $(library_OBJS)
	ar rcs -o lib$(library_NAME).a $(library_OBJS)
	@- mkdir -p ../libs/
	@- cp -u lib$(library_NAME).a ../libs/lib$(library_NAME).a
	@- mkdir -p ../libs/include/
	@- cp -u $(library_H_FILES) ../libs/include/$(library_H_FILES)

%.o : %.c
	$(CC) -M $(CPPFLAGS) -o $*.P $<
	@cp $*.P $*.d; \
			sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
				-e '/^$$/ d' -e 's/$$/ :/' < $*.P >> $*.d; \
			rm -f $*.P
	$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	@- $(RM) lib$(library_NAME).a
	@- $(RM) $(library_OBJS)

distclean: clean
	@- $(RM)  ../libs/lib$(library_NAME)a
	@- $(RM) ../libs/include/$(library_H_FILES)

