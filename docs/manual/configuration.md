# Konfiguration

Dieses Kapitel nennt die wichtigsten Optionen fuer den macOS/ARD-Fork. Die
vollstaendige Syntax steht weiterhin in den Manpages unter `docs/man/`.

## xrdp

Die zentrale Konfiguration ist `xrdp.ini`. Im Quellbaum ist die Vorlage unter
`xrdp/xrdp.ini.in` gepflegt. Installierte Pfade haengen von `--prefix`,
`--sysconfdir`, `--with-sysconfsubdir` und `--enable-strict-locations` ab.

Wichtige Configure-Pfade werden am Ende von `./configure` ausgegeben:

- `sysconfdir+subdir`
- `pamconfdir`
- `localstatedir`
- `runstatedir`
- `socketdir`

## macOS-Frameworks

Die Option:

```sh
--enable-macos-frameworks=auto|yes|no
```

steuert die Nutzung von `Security.framework` und `CoreFoundation`.

- `auto`: auf Darwin aktivieren, wenn linkbar.
- `yes`: auf Darwin erzwingen und bei fehlenden Frameworks abbrechen.
- `no`: Framework-Helfer deaktivieren.

OpenSSL bleibt fuer TLS-Handshakes und PEM-Dateien verantwortlich.

## X11

Die Option:

```sh
--disable-x11
```

deaktiviert X11-abhaengige Module, Werkzeuge und Tests. Sie ist fuer
macOS-Builds ohne XQuartz/X11 gedacht. Funktionen, die X11 benoetigen, werden
dann nicht gebaut.

## NeutrinoRDP-Zertifikate

Der sichere Default ist:

```ini
#neutrinordp.ignore_certificate=false
```

Nur fuer bewusst akzeptierte Legacy-, Labor- oder Uebergangsumgebungen darf ein
Admin explizit deaktivieren:

```ini
neutrinordp.ignore_certificate=true
```

Diese Einstellung deaktiviert die Zielzertifikatsvalidierung fuer
NeutrinoRDP-Verbindungen und erzeugt eine Warnung im Log. Sie darf nicht als
allgemeine Komfortoption verwendet werden.

## PAM auf macOS

macOS-Authentifizierung bleibt PAM-basiert. Die passende Vorlage liegt unter:

```text
instfiles/pam.d/xrdp-sesman.macos
```

Diese nutzt Apple-PAM-Bausteine wie OpenDirectory-Integration. Authorization
Services und LocalAuthentication werden in diesem Fork nicht als Ersatz fuer
sesman-Passwortpruefungen verwendet.
