.POSIX:

NAME = liblocation_applet
LIB = $(NAME:=.so)
OBJ = $(NAME:=.o)

PKG_FLAGS = $(shell pkg-config gtk+-2.0 hildon-control-panel libosso gconf-2.0 --libs --cflags)
CCFLAGS = -shared
LDFLAGS = -Wl,--as-needed

HILDON_CONTROL_PANEL_LIBDIR = $(shell pkg-config hildon-control-panel --variable=pluginlibdir)

all: $(LIB)

$(LIB): $(OBJ)
	$(CC) $(CCFLAGS) $(LDFLAGS) $(PKG_FLAGS) $(OBJ) -o $@

.c.o:
	$(CC) -fPIC $(CCFLAGS) $(PKG_FLAGS) -c $<

install:
	mkdir -p $(DESTDIR)$(HILDON_CONTROL_PANEL_LIBDIR)
	cp -f $(LIB) $(DESTDIR)$(HILDON_CONTROL_PANEL_LIBDIR)
	chmod 644 $(DESTDIR)$(HILDON_CONTROL_PANEL_LIBDIR)/$(LIB)

uninstall:
	rm -f $(DESTDIR)$(HILDON_CONTROL_PANEL_LIBDIR)/$(LIB)

clean:
	rm -f $(OBJ) $(LIB)
