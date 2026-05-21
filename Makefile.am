ACLOCAL_AMFLAGS = -I m4
AM_DISTCHECK_CONFIGURE_FLAGS = \
  --without-systemdsystemunitdir \
  --enable-strict-locations \
  --enable-tests

EXTRA_DIST = \
  COPYING \
  README.md \
  NEWS.md \
  astyle_config.as \
  bootstrap \
  coding_style.md \
  m4 \
  vrplayer

if XRDP_NEUTRINORDP
NEUTRINORDPDIR = neutrinordp
else
NEUTRINORDPDIR =
endif

if XRDP_X11
VNCDIR = vnc
WAITFORXDIR = waitforx
GENKEYMAPDIR = genkeymap
else
VNCDIR =
WAITFORXDIR =
GENKEYMAPDIR =
endif

if XRDP_XRDPVR
XRDPVRDIR = xrdpvr
else
XRDPVRDIR =
endif

if XRDP_PAINTER
PAINTERDIR = libpainter
else
PAINTERDIR =
endif

if XRDP_RFXCODEC
RFXCODECDIR = librfxcodec
else
RFXCODECDIR =
endif

if XRDP_ULALACA
ULALACADIR = ulalaca
else
ULALACADIR =
endif

if XRDP_ACCEL
ACCELDIR = xrdp_accel_assist
else
ACCELDIR =
endif

# This should not be dictionary order but build order
SUBDIRS = \
  third_party \
  third_party/tomlc99 \
  common \
  $(VNCDIR) \
  xup \
  mc \
  $(NEUTRINORDPDIR) \
  libipm \
  libxrdp \
  $(PAINTERDIR) \
  $(RFXCODECDIR) \
  sesman \
  xrdp \
  fontutils \
  keygen \
  $(WAITFORXDIR) \
  docs \
  instfiles \
  $(GENKEYMAPDIR) \
  xrdpapi \
  pkgconfig \
  $(XRDPVRDIR) \
  $(ULALACADIR) \
  tests \
  tools \
  $(ACCELDIR)

distclean-local:
	-rm -f xrdp_configure_options.h
