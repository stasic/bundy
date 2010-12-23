SUBDIRS = . tests

AM_CPPFLAGS = -I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib
AM_CPPFLAGS += $(BOOST_INCLUDES)
AM_CPPFLAGS += -I$(top_srcdir)/src/lib/dns -I$(top_builddir)/src/lib/dns

AM_CXXFLAGS = $(B10_CXXFLAGS)

CLEANFILES = *.gcno *.gcda

# This is a wrapper library solely used for b10-auth.  The ASIO header files
# have some code fragments that would hit gcc's unused-parameter warning,
# which would make the build fail with -Werror (our default setting).
lib_LTLIBRARIES = libasiolink.la
libasiolink_la_SOURCES = asiolink.cc asiolink.h
libasiolink_la_SOURCES += iosocket.cc iosocket.h
libasiolink_la_SOURCES += iomessage.h
libasiolink_la_SOURCES += ioaddress.cc ioaddress.h
libasiolink_la_SOURCES += ioendpoint.cc ioendpoint.h
libasiolink_la_SOURCES += udpdns.cc internal/udpdns.h
libasiolink_la_SOURCES += tcpdns.cc internal/tcpdns.h
libasiolink_la_SOURCES += internal/coroutine.h
# Note: the ordering matters: -Wno-... must follow -Wextra (defined in
# B10_CXXFLAGS)
libasiolink_la_CXXFLAGS = $(AM_CXXFLAGS)
if USE_GXX
libasiolink_la_CXXFLAGS += -Wno-unused-parameter
endif
if USE_CLANGPP
# Same for clang++, but we need to turn off -Werror completely.
libasiolink_la_CXXFLAGS += -Wno-error
endif
libasiolink_la_CPPFLAGS = $(AM_CPPFLAGS)
libasiolink_la_LIBADD = $(top_builddir)/src/lib/log/liblog.la