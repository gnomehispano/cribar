bin_PROGRAMS = cribar

cribar_SOURCES = \
	cribar-application.c \
	cribar-application.h \
	cribar-application-window.c \
	cribar-application-window.h \
	cribar-main.c \
	$(NULL)

cribar_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""${datadir}/locale"\" \
	$(AM_CPPFLAGS) \
	$(NULL)

cribar_CFLAGS = \
	$(WARN_CFLAGS) \
	$(CRIBAR_MODULES_CFLAGS) \
	$(AM_CFLAGS) \
	$(NULL)

cribar_LDFLAGS =

cribar_LDADD = \
	$(CRIBAR_MODULES_LIBS) \
	$(NULL)

-include $(top_srcdir)/git.mk
