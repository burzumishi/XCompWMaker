PLUGIN = ccm-freeze
PLUGIN_UI = ccm-freeze.ui freeze.svg
PLUGIN_SCHEMA = ccm-freeze.schema-key.in

PLUGIN_SOURCES = ccm-freeze.c ccm-freeze.h

include $(top_srcdir)/build/plugin.am

plugin_LTLIBRARIES = libccm_freeze.la 

libccm_freeze_la_SOURCES = $(PLUGIN_SOURCES)

libccm_freeze_la_LDFLAGS =  $(PLUGIN_LDFLAGS)

libccm_freeze_la_LIBADD =  $(PLUGIN_LIBADD)