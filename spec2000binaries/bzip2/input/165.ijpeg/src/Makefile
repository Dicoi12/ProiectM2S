###############################################################################
# SPECtools98 
#   Version 0.3
#   This makefile should NOT be changed other than to customize for a 
#   particular benchmark.  Users may overide variables by placing them
#   in the Makefile.spec file.  Or by using the "runspec" program to 
#   build the Makefile.spec file for them.
###############################################################################
#	BENCHMARK	-- standard definitions for this benchmark

NUMBER    = 165
NAME      = ijpeg
EXEBASE   = ijpeg
SOURCES   = libpbm1.c libpbm2.c libpbm3.c libpbm4.c libpbm5.c \
	    libpgm1.c libpgm2.c libppm1.c libppm2.c libppm3.c \
	    libppm4.c libppm5.c spec_image.c spec_jmemdst.c \
	    spec_jmemsrc.c spec_main.c rdppm.c wrppm.c rdgif.c \
	    wrgif.c rdtarga.c wrtarga.c rdbmp.c wrbmp.c jcapi.c \
	    jcparam.c jdatadst.c jcmaster.c jcmarker.c jcmainct.c \
	    jcprepct.c jccoefct.c jccolor.c jcsample.c jchuff.c \
	    jcdctmgr.c jfdctfst.c jfdctflt.c jfdctint.c jdapi.c \
	    jdatasrc.c jdmaster.c jdmarker.c jdmainct.c jdcoefct.c \
	    jdpostct.c jddctmgr.c jidctfst.c jidctflt.c jidctint.c \
	    jidctred.c jdhuff.c jdsample.c jdcolor.c jquant1.c \
	    jquant2.c jdmerge.c jcomapi.c jutils.c jerror.c \
	    jmemmgr.c jmemnobs.c

# Include standard template for this suite
-include ../../../../Makefile.defaults
-include ../../../Makefile.defaults
-include ../../Makefile.defaults
-include ../Makefile.defaults

nomakefiles:
	@echo "*** Whoops!  I didn't find a Makefile.defaults file."
	@echo "*** Your distribution is probably corrupt."
	@echo
