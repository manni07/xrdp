# Tests

Dieses Kapitel beschreibt die lokale Teststrategie fuer den macOS/ARD-Fork.

## Lokale Matrix

Das Hilfsskript:

```sh
scripts/macos-test-matrix.sh
```

prueft die Grundvoraussetzungen und fuehrt die wichtigsten macOS-Buildprofile
aus:

- macOS-Frameworks mit X11
- macOS-Frameworks ohne X11
- optional NeutrinoRDP, wenn ein kompatibles `freerdp.pc` verfuegbar ist

Das Skript erwartet `check` und `cmocka` via `pkg-config`.

## Manuelle Pruefung

Empfohlenes Standardprofil:

```sh
./bootstrap
./configure --enable-tests --enable-macos-frameworks=yes
make
make check
```

X11-freies Profil:

```sh
./configure --enable-tests --enable-macos-frameworks=yes --disable-x11
make
make check
```

Distributionstest:

```sh
make distcheck
```

`make distcheck` prueft, dass die Distribution gebaut, installiert, getestet
und wieder bereinigt werden kann. Dokumentationsdateien unter `docs/manual/`
muessen deshalb ueber Autotools in `EXTRA_DIST` eingebunden sein.

## FreeRDP-Verhalten

Wenn `pkg-config` nur `freerdp5` findet, wird die NeutrinoRDP-Matrix
uebersprungen. Der Grund ist die noch nicht portierte FreeRDP-5-Settings-API.

Mit:

```sh
RUN_NEUTRINORDP=1 scripts/macos-test-matrix.sh
```

wird dieser Skip als Fehler behandelt. Das ist sinnvoll, wenn eine Umgebung
explizit NeutrinoRDP verifizieren soll.

## Warnungsfreiheit

Fuer macOS-Builds sollte nach Configure, Build und Test kein Treffer fuer diese
Muster uebrig bleiben:

```sh
rg -n "warning:|ld: warning|libtool: warning|-single_module|-dylib_file|complete static linking" <logdateien>
```

Ein leerer Trefferbericht bedeutet, dass die bekannten Darwin/libtool-Warnungen
nicht erneut auftreten.
