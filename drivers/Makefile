drivers := rpi3

.PHONY: all $(drivers) clean

all: $(drivers)

$(drivers):
	$(MAKE) --directory=$@

clean: 
	for d in $(drivers); \
		do				\
			$(MAKE) --directory=$$d clean; \
		done


include ../include.mk
