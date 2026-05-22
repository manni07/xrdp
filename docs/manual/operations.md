# Betrieb

Dieses Kapitel beschreibt sichere Betriebsablaeufe fuer Operatoren. Es enthaelt
bewusst keine unbedingten Kommandos zum Neustarten von Diensten oder Systemen.

## Vor Aenderungen

Vor jeder Aenderung sollten diese Punkte feststehen:

- Welche Konfigurationsdatei wird geaendert?
- Welche Sitzung oder welcher Dienst ist betroffen?
- Wie wird die Aenderung validiert?
- Wer bestaetigt einen eventuell notwendigen Service-Restart?
- Wie wird zur vorherigen Konfiguration zurueckgekehrt?

## Pfade pruefen

Die tatsaechlichen Installationspfade haengen von Configure-Optionen ab. Nach
`./configure` werden die relevanten Pfade ausgegeben, darunter:

- `sysconfdir+subdir`
- `pamconfdir`
- `localstatedir`
- `runstatedir`
- `socketdir`

Bei Problemen zuerst diese Ausgabe mit der erwarteten Installation vergleichen.

## Lokale Installation pruefen

Die lokale macOS-Installation dieses Forks verwendet volle Pfade. Verlasse dich
nicht darauf, dass `/usr/local/sbin` im `PATH` liegt.

Vor dem Start:

```sh
/usr/local/sbin/xrdp --version
/usr/local/sbin/xrdp-sesman --version
test -f /etc/xrdp/xrdp.ini
test -f /etc/xrdp/sesman.ini
test -f /etc/pam.d/xrdp-sesman
test -f /etc/xrdp/cert.pem
test -f /etc/xrdp/key.pem
lsof -nP -iTCP:3389 -sTCP:LISTEN
```

Der letzte Befehl darf vor dem Start keine Ausgabe erzeugen. Wenn bereits ein
Listener auf TCP 3389 existiert, zuerst klaeren, welcher Prozess den Port
belegt. Auf macOS kann fuer root-eigene Listener `sudo lsof` erforderlich sein.

## Manueller Start

Starte zuerst den Session-Manager und danach den RDP-Listener:

```sh
sudo /usr/local/sbin/xrdp-sesman --config /etc/xrdp/sesman.ini
sudo /usr/local/sbin/xrdp --config /etc/xrdp/xrdp.ini
```

Die Kommandos starten die Prozesse als Daemons. Es wird keine Launchd-Plist
installiert und kein Autostart eingerichtet.

## Status pruefen

Nach dem Start:

```sh
sudo lsof -nP -iTCP:3389 -sTCP:LISTEN
netstat -anv -p tcp | rg "\\.3389|Local Address|Proto"
ps aux | rg "xrdp|xrdp-sesman"
ls -l /var/run/xrdp.pid /var/run/xrdp-sesman.pid
ls -l /var/log/xrdp.log /var/log/xrdp-sesman.log
```

`xrdp` muss auf TCP 3389 lauschen. `xrdp-sesman` muss als eigener Prozess
laufen. Die PID-Dateien liegen bei dieser Installation unter `/var/run`.

## Nutzung im LAN

Ermittle die lokale IP-Adresse des Macs:

```sh
ipconfig getifaddr en0
ipconfig getifaddr en1
```

`en0` ist haeufig WLAN oder Ethernet, je nach Mac und Adapter. Verwende die
Adresse, die zu deinem aktiven LAN passt.

Verbinde dich mit einem RDP-Client von einem anderen Geraet im selben LAN auf:

```text
<Mac-IP>:3389
```

Melde dich mit einem macOS-Benutzer an, der lokal ueber PAM/OpenDirectory
authentifiziert werden kann. Wenn die Verbindung den Listener erreicht, die
Anmeldung aber fehlschlaegt, zuerst `/var/log/xrdp-sesman.log` pruefen.

Die Logdateien gehoeren root und muessen auf macOS normalerweise mit `sudo`
gelesen werden:

```sh
sudo tail -n 80 /var/log/xrdp.log
sudo tail -n 80 /var/log/xrdp-sesman.log
```

## Remotr statt xrdp

Wenn `Remotr.app` als RDP-Server verwendet wird, darf `xrdp` nicht parallel
auf Port `3389` laufen. Vor dem Start von Remotr:

```sh
ps aux | rg -i "Remotr|xrdp|xrdp-sesman"
netstat -anv -p tcp | rg "\\.3389|Local Address|Proto"
```

Wenn `xrdp` den Port belegt, zuerst kontrolliert stoppen:

```sh
sudo /usr/local/sbin/xrdp --kill
sudo /usr/local/sbin/xrdp-sesman --kill
```

Danach kann Remotr ueber die App gestartet werden. Der erwartete Listener ist
dann ebenfalls `3389`, aber mit Prozess `Remotr` statt `xrdp`.

## Sicheres Stoppen

Stoppen ist ein separater Betriebseingriff und muss vorher ausdruecklich
bestaetigt werden. Wenn bestaetigt:

```sh
sudo /usr/local/sbin/xrdp --kill
sudo /usr/local/sbin/xrdp-sesman --kill
```

Danach pruefen:

```sh
lsof -nP -iTCP:3389 -sTCP:LISTEN
ps aux | rg "xrdp|xrdp-sesman"
```

## Logs und Diagnose

Typische Diagnosefragen:

- Ist Zertifikatsvalidierung aktiv oder wurde
  `neutrinordp.ignore_certificate=true` gesetzt?
- Wurde mit `--disable-x11` gebaut und fehlt deshalb eine X11-abhaengige
  Funktion bewusst?
- Sind `Security.framework` und `CoreFoundation` im Build aktiviert?
- Stimmen PAM-Datei und `pamconfdir` zusammen?
- Verwendet NeutrinoRDP eine kompatible FreeRDP-Version?

## Konfigurationsaenderungen

Konfigurationsaenderungen sollten klein, nachvollziehbar und reversibel sein.
Nach einer Aenderung zuerst statische Pruefungen, Build-/Testpruefungen oder
gezielte Logpruefungen ausfuehren, bevor ein Dienst neu geladen oder neu
gestartet wird.

Wenn ein Service-Restart erforderlich ist, muss er vorher ausdruecklich durch
den Operator bestaetigt werden. Dasselbe gilt fuer Reboots oder andere
Systemeingriffe.

## Sicherheitsoptionen

Der Opt-out:

```ini
neutrinordp.ignore_certificate=true
```

ist ein Ausnahmefall. Vor dem Einsatz sollte dokumentiert sein, welches Ziel,
welche Laufzeit und welches Risiko akzeptiert wurden.
