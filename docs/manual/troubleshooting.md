# Troubleshooting

Dieses Kapitel sammelt haeufige Fehlerbilder im macOS/ARD-Fork.

## Xcode Command Line Tools fehlen

Symptom:

```text
xcode-select: error
```

Pruefung:

```sh
xcode-select -p
```

Erst wenn ein gueltiger Developer-Pfad konfiguriert ist, sind Compiler und
Apple-Systemframeworks fuer den Build nutzbar.

## `check` oder `cmocka` fehlt

Symptom:

```text
missing pkg-config package: check
missing pkg-config package: cmocka
```

Pruefung:

```sh
pkg-config --exists check
pkg-config --exists cmocka
```

Installation:

```sh
brew install check cmocka
```

## FreeRDP 5 statt legacy `freerdp.pc`

Symptom:

```text
Homebrew FreeRDP was found as freerdp5
```

Die aktuelle NeutrinoRDP-Integration erwartet ein kompatibles `freerdp.pc`.
Homebrew kann `freerdp5.pc` bereitstellen, dessen API nicht automatisch
kompatibel ist. Bis zur FreeRDP-5-Portierung bleibt die NeutrinoRDP-Matrix in
diesem Fall ein dokumentierter Skip.

## `xrdp` wird im Terminal nicht gefunden

Symptom:

```text
zsh: command not found: xrdp
```

Die lokale Installation legt die Daemons unter `/usr/local/sbin` ab. Dieser
Pfad ist auf macOS nicht zwingend im `PATH`. Verwende volle Pfade:

```sh
/usr/local/sbin/xrdp --version
/usr/local/sbin/xrdp-sesman --version
```

## Port 3389 ist bereits belegt

Symptom:

```text
It looks like xrdp is already running
```

oder der Start von `xrdp` bindet den Port nicht. Pruefung:

```sh
sudo lsof -nP -iTCP:3389 -sTCP:LISTEN
netstat -anv -p tcp | rg "\\.3389|Local Address|Proto"
ps aux | rg "xrdp|xrdp-sesman"
```

Wenn ein anderer Prozess den Port belegt, nicht blind stoppen. Erst klaeren, ob
es ein vorhandener xrdp-Daemon, ein anderer RDP-Dienst oder ein Testprozess ist.

Ein realer Konflikt in diesem Fork-Setup ist `Remotr.app` gegen `xrdp`, weil
beide standardmaessig Port `3389` verwenden.

Pruefung:

```sh
ps aux | rg -i "Remotr|xrdp|xrdp-sesman"
netstat -anv -p tcp | rg "\\.3389|Local Address|Proto"
```

## Login klappt, Session startet aber nicht

Wenn die Anmeldung akzeptiert wird, danach aber `Can't create session for user`
erscheint, zuerst die root-geschuetzten Logs lesen:

```sh
sudo tail -n 120 /var/log/xrdp-sesman.log
sudo tail -n 160 /var/log/xrdp.log
```

Bekannte Fehlerkette auf diesem Mac:

- frueher: `SessionSockdirGroup=root` war auf macOS falsch
- korrigiert: `SessionSockdirGroup=wheel`
- verbleibender Blocker: `Xorg` kann nicht gestartet werden

Typische Belege:

```text
Error calling exec (excutable: Xorg ...)
X server failed to start
```

Ursache im aktuellen Stand:

- `/opt/X11/bin/Xorg` existiert, aber `param=Xorg` ist im Daemon-`PATH` nicht
  aufloesbar
- `xrdp/xorg.conf` fehlt
- `xorgxrdp`-Module fehlen
- `Xvnc` ist nicht installiert

## Keine LAN-Verbindung trotz Listener

Wenn `lsof` einen Listener auf TCP 3389 zeigt, ein anderes Geraet im LAN aber
nicht verbinden kann, pruefe zuerst IP-Adresse und Firewall-Status:

```sh
ipconfig getifaddr en0
ipconfig getifaddr en1
sudo /usr/libexec/ApplicationFirewall/socketfilterfw --getglobalstate
sudo /usr/libexec/ApplicationFirewall/socketfilterfw --listapps
```

Firewall-Regeln werden in diesem Handbuch nicht automatisch geaendert. Eine
Freigabe fuer xrdp-Binaries ist ein separater Admin-Eingriff und sollte erst
nach ausdruecklicher Bestaetigung erfolgen.

## Logs fehlen oder bleiben leer

Die Standard-Logpfade dieser Installation sind:

```text
/var/log/xrdp.log
/var/log/xrdp-sesman.log
```

Pruefung:

```sh
ls -l /var/log/xrdp.log /var/log/xrdp-sesman.log
tail -n 80 /var/log/xrdp.log
tail -n 80 /var/log/xrdp-sesman.log
```

Wenn ein Daemon nicht startet, zuerst die passende Logdatei und dann die
PID-Dateien unter `/var/run` pruefen.

## X11-freier Build vermisst Funktionen

Wenn mit:

```sh
--disable-x11
```

gebaut wurde, sind X11-abhaengige Komponenten nicht verfuegbar. Das ist
erwartetes Verhalten. Fuer Funktionen, die X11 benoetigen, muss ein Build mit
X11-Abhaengigkeiten verwendet werden.

## X11- oder XQuartz-Sitzung startet nicht

Die lokalen Session-Typen `Xorg` und `Xvnc` haengen von passenden X11- bzw.
VNC-Komponenten ab. Pruefe die installierten Session-Abschnitte:

```sh
rg -n "^\\[|^name=|^lib=|^param=" /etc/xrdp/xrdp.ini /etc/xrdp/sesman.ini
```

Wenn der Build mit `--disable-x11` erstellt wurde, sind X11-abhaengige
Funktionen bewusst nicht verfuegbar. Wenn X11 genutzt werden soll, muss die
macOS-Umgebung passende X11-Komponenten bereitstellen, zum Beispiel XQuartz.

## macOS/libtool-Warnungen tauchen wieder auf

Bekannte Warnungsmuster:

```text
-single_module is obsolete
-dylib_file is deprecated
complete static linking is impossible
```

Empfohlene Diagnose:

```sh
./bootstrap
./configure --enable-tests --enable-macos-frameworks=yes
make V=1
make check
rg -n "warning:|ld: warning|libtool: warning|-single_module|-dylib_file|complete static linking" <logdateien>
```

Wenn diese Muster erneut erscheinen, zuerst pruefen, ob `bootstrap` ausgefuehrt
wurde und ob die Submodule `libpainter` und `librfxcodec` auf den erwarteten
Fork-Commits stehen.

## `make distcheck` findet Manual-Dateien nicht

Wenn `make distcheck` ueber fehlende Manual-Dateien stolpert, ist meist die
Autotools-Distribution unvollstaendig. Pruefen:

```sh
find docs/manual -maxdepth 1 -type f | sort
```

und sicherstellen, dass alle Kapitel in `docs/manual/Makefile.am` unter
`EXTRA_DIST` stehen.
