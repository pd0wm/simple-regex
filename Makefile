TARGETS = regex main

.PHONY: all $(TARGETS)
all: $(TARGETS)

clean:
	$(foreach dir,$(TARGETS), $(MAKE) -C $(dir) clean;)

$(TARGETS):
	$(MAKE) -C $@;
