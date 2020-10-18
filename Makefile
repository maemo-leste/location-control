.POSIX:

HILDON_CONTROL_PANEL_LIBDIR  = $(shell pkg-config hildon-control-panel --variable=plugindir)
HILDON_CONTROL_PANEL_DATADIR = $(shell pkg-config hildon-control-panel --variable=plugindesktopentrydir)

#SUBDIRS = src data schemas
SUBDIRS = src data

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

install uninstall clean:
	for i in $(SUBDIRS); do $(MAKE) -C $$i $@; done

.PHONY: all clean install $(SUBDIRS)
