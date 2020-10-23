.POSIX:

HILDON_CONTROL_PANEL_DATADIR = $(shell pkg-config hildon-control-panel --variable=plugindesktopentrydir)

all:

install:
	mkdir -p $(DESTDIR)$(HILDON_CONTROL_PANEL_DATADIR)
	cp -f location-cp.desktop $(DESTDIR)$(HILDON_CONTROL_PANEL_DATADIR)

uninstall:
	rm -f $(DESTDIR)$(HILDON_CONTROL_PANEL_DATADIR)/location-cp.desktop

clean:
