ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/x86_64/amd64/start/bsp_specs

noinst_PROGRAMS =

project_lib_DATA = linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/x86_64/amd64/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/x86_64/amd64/start/start.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/x86_64/amd64/start/setvec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c
# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/x86_64/amd64/clock/ckinit.c
# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/x86_64/amd64/console/console.c
# timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/x86_64/amd64/btimer/btimer.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/x86_64/amd64/btimer/timerisr.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/x86_64/shared/cache/cache.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/x86_64/amd64/headers.am
