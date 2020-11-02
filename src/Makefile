.POSIX:

NAME = liblocation_applet
LIB = $(NAME:=.so)
OBJ = $(NAME:=.o)

PKG_FLAGS = $(shell pkg-config gtk+-2.0 hildon-control-panel libosso gconf-2.0 --libs --cflags)
CCFLAGS = -shared
LDFLAGS = -Wl,--as-needed

HILDON_CONTROL_PANEL_LIBDIR = $(shell pkg-config hildon-control-panel --variable=pluginlibdir)
HILDON_CONTROL_PANEL_DATADIR = $(shell pkg-config hildon-control-panel --variable=plugindesktopentrydir)

all: $(LIB)

$(LIB): $(OBJ)
	$(CC) $(CCFLAGS) $(LDFLAGS) $(PKG_FLAGS) $(OBJ) -o $@

.c.o:
	$(CC) -fPIC $(CCFLAGS) $(PKG_FLAGS) -c $<

install:
	mkdir -p $(DESTDIR)$(HILDON_CONTROL_PANEL_LIBDIR)
	cp -f $(LIB) $(DESTDIR)$(HILDON_CONTROL_PANEL_LIBDIR)
	chmod 644 $(DESTDIR)$(HILDON_CONTROL_PANEL_LIBDIR)/$(LIB)
	mkdir -p $(DESTDIR)/usr/share/gconf/schemas
	cp -f location-cp.schemas $(DESTDIR)/usr/share/gconf/schemas/
	mkdir -p $(DESTDIR)$(HILDON_CONTROL_PANEL_DATADIR)
	cp -f location-cp.desktop $(DESTDIR)$(HILDON_CONTROL_PANEL_DATADIR)

uninstall:
	rm -f $(DESTDIR)$(HILDON_CONTROL_PANEL_LIBDIR)/$(LIB)
	rm -f $(DESTDIR)/usr/share/gconf/schemas/location-cp.schemas
	rm -f $(DESTDIR)$(HILDON_CONTROL_PANEL_DATADIR)/locaion-cp.desktop

clean:
	rm -f $(OBJ) $(LIB)
