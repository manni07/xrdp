# macOS-Build

Dieses Kapitel beschreibt den lokalen Build auf macOS. Zielplattform ist macOS
13 oder neuer mit Xcode Command Line Tools und Homebrew.

## Voraussetzungen

Installiere die benoetigten Build- und Testwerkzeuge ueber Homebrew:

```sh
brew install autoconf automake libtool pkg-config openssl check cmocka
```

Die Xcode Command Line Tools muessen konfiguriert sein:

```sh
xcode-select -p
```

Wenn der Befehl keinen Pfad ausgibt, ist die lokale Developer-Toolchain nicht
bereit. Fuer die in diesem Fork verwendeten Frameworks ist kein Apple Developer
Account erforderlich.

## Repository und Submodule

Beim Klonen muessen die Submodule verfuegbar sein:

```sh
git clone --recursive https://github.com/manni07/xrdp.git
cd xrdp
```

Wenn das Repository bereits existiert:

```sh
git submodule update --init --recursive
```

In diesem Fork koennen `libpainter` und `librfxcodec` auf Fork-Commits zeigen,
damit macOS-spezifische Build-Warnungsfixes reproduzierbar bleiben.

## Standardprofil mit macOS-Frameworks

Das empfohlene lokale Buildprofil aktiviert Tests und erzwingt die
macOS-Framework-Helfer:

```sh
./bootstrap
./configure --enable-tests --enable-macos-frameworks=yes
make
make check
```

`--enable-macos-frameworks=yes` verlangt, dass `Security.framework` und
`CoreFoundation` linkbar sind. Der Default `auto` aktiviert diese Helfer auf
Darwin, wenn sie verfuegbar sind.

## X11-freier Build

Fuer macOS-Umgebungen ohne X11-Abhaengigkeiten:

```sh
./configure --enable-tests --enable-macos-frameworks=yes --disable-x11
make
make check
```

`--disable-x11` deaktiviert X11-abhaengige Komponenten bewusst zur Build-Zeit.
Das ist kein Stub-Modus; betroffene Funktionen werden nicht halb aktiviert.

## NeutrinoRDP-Profil

NeutrinoRDP bleibt optional:

```sh
./configure --enable-tests --enable-neutrinordp --enable-macos-frameworks=yes
make
make check
```

Die aktuelle NeutrinoRDP-Integration erwartet ein kompatibles legacy
`freerdp.pc`. Homebrew-FreeRDP kann stattdessen `freerdp5.pc` bereitstellen;
dieser Fall wird als nicht kompatibel erkannt, bis eine dedizierte FreeRDP-5-
Portierung existiert.
