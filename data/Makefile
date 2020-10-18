.POSIX:

HILDON_CONTROL_PANEL_DATADIR = $(shell pkg-config hildon-control-panel --variable=plugindesktopentrydir)

all:

install:
	mkdir -p $(DESTDIR)$(HILDON_CONTROL_PANEL_DATADIR)
	cp -f cplocation.desktop $(DESTDIR)$(HILDON_CONTROL_PANEL_DATADIR)

uninstall:
	rm -f $(DESTDIR)$(HILDON_CONTROL_PANEL_DATADIR)/cplocation.desktop

clean:
