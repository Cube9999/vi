# @(#) vi.mk: 1.16 10/15/83
ROOT	=
install: 
	sh makevi
#
# executed by makevi
#
all: cmd

cmd:
	@echo "\n\t>Making commands."
	cd misc; make all; cd ..

# make for Vax 11/750
src750:
	@echo "\n\t> Compiling ex source."
	cd vax ; make -f makefile.usg all ; cd ..
	@echo "\n\t> Installing ex object."
	cd vax ; make install -f makefile.usg; cd ..

# make for Vax 11/780
src780:
	@echo "\n\t> Compiling ex source."
	cd vax ; make -f makefile.usg all ; cd ..
	@echo "\n\t> Installing ex object."
	cd vax ; make install -f makefile.usg; cd ..

# make for National 32016
src32016:
	@echo "\n\t> Compiling ex source."
	cd vax ; make -f makefile.usg all ; cd ..
	@echo "\n\t> Installing ex object."
	cd vax ; make install -f makefile.usg; cd ..

# make for Unix 370
src370:
	@echo "\n\t> Compiling ex source."
	cd vax ; make  -f makefile.370 all ; cd ..
	@echo "\n\t> Installing ex object."
	cd vax ; make install -f makefile.370; cd ..

# make for 3B-20S
src20S:
	@echo "\n\t> Compiling ex source."
	cd vax ; make  -f makefile.usg all ; cd ..
	@echo "\n\t> Installing ex object."
	cd vax ; make install -f makefile.usg ; cd ..

# make for 3B5
src3B5:
	@echo "\n\t> Compiling ex source."
	cd vax ; make  -f makefile.usg all ; cd ..
	@echo "\n\t> Installing ex object."
	cd vax ; make install -f makefile.usg ; cd ..

# make for Cory Pdp/11-70
srcc70:
	@echo "\n\t> Compiling ex source."
	cd vax ; make -f makefile.c70 all ; cd ..
	@echo "\n\t> Installing ex object."
	cd vax ; make install -f makefile.c70; cd ..

# make for Pdp/11-70
src70:
	@echo "\n\t> Compiling ex source."
	cd pdp11 ; make -f makefile.usg all ; cd ..
	@echo "\n\t> Installing ex object."
	cd pdp11 ; make install -f makefile.usg ; cd ..
	cd pdp11 ; make installutil -f makefile.usg; cd ..

#
# Cleanup procedures
#
clobber: clean

clean:
	cd misc ; make clean ; cd ..

	-if vax || ns32000; \
	then \
		cd vax; \
		make  -f makefile.usg clean ; \
	elif pdp11; \
	then \
		cd pdp11; \
		make -f makefile.usg clean ;\
	elif u3b; \
	then \
		cd vax; \
		make -f makefile.usg clean ;\
	elif u3b5; \
	then \
		cd vax; \
		make -f makefile.usg clean ;\
	fi

