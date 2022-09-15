lib_LTLIBRARIES = gruesomeKillerFlag.la

gruesomeKillerFlag_la_SOURCES = gruesomeKillerFlag.cpp
gruesomeKillerFlag_la_CPPFLAGS= -I$(top_srcdir)/include -I$(top_srcdir)/plugins/plugin_utils
gruesomeKillerFlag_la_LDFLAGS = -module -avoid-version -shared
gruesomeKillerFlag_la_LIBADD = $(top_builddir)/plugins/plugin_utils/libplugin_utils.la

AM_CPPFLAGS = $(CONF_CPPFLAGS)
AM_CFLAGS = $(CONF_CFLAGS)
AM_CXXFLAGS = $(CONF_CXXFLAGS)

EXTRA_DIST = \
	README.gruesomeKillerFlag.txt \
	gruesomeKillerFlag.sln \
	gruesomeKillerFlag.vcxproj	\
	gruesomeKillerFlag.vcxproj.filters

MAINTAINERCLEANFILES =	\
	Makefile.in
