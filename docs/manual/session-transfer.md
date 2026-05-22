# Session-Transfer-Protokoll

Dieses Kapitel definiert einen klaren Handover fuer laufende Remote-Desktop-
Arbeiten auf diesem Mac. Ziel ist, dass ein neuer Operator oder eine neue
Support-Session den Zustand ohne Ratespiel uebernehmen kann.

## Aktueller Soll-Zustand

Stand dieses Fork-Workflows:

- `Remotr.app` ist der aktive RDP-Server.
- `xrdp` und `xrdp-sesman` bleiben gestoppt, solange Remotr auf Port `3389`
  genutzt wird.
- macOS Screen Sharing / Remote Management kann weiterhin auf Port `5900`
  aktiv sein.
- Der letzte xrdp-Befund war kein Authentifizierungsfehler mehr, sondern ein
  fehlender lokaler Xorg/xorgxrdp-Unterbau fuer echte xrdp-Xorg-Sessions auf
  macOS.

## Pflichtangaben bei einer Uebergabe

Jede Session-Uebergabe soll diese Punkte explizit enthalten:

- Welcher Dienst soll Port `3389` besitzen: `Remotr` oder `xrdp`.
- Welche App oder welcher Client wurde zuletzt getestet.
- Welche IP-Adresse wird aktuell verwendet:
  - LAN-IP
  - Tailscale-IP
- Ob ScreenCapture-, Accessibility- und gegebenenfalls Login-/Lizenzdialoge
  fuer Remotr bereits bestaetigt wurden.
- Ob Aenderungen an `/etc/xrdp/sesman.ini` vorgenommen wurden.
- Ob ein Dienst zuletzt gestartet, gestoppt oder neu gestartet wurde.

## Schneller Zustandscheck

Vor jeder Fortsetzung:

```sh
ps aux | rg -i "Remotr|xrdp|xrdp-sesman"
netstat -anv -p tcp | rg "\\.3389|\\.5900|Local Address|Proto"
tailscale ip -4
```

Interpretation:

- `Remotr` auf `3389`: Remotr ist aktiv, xrdp darf nicht parallel starten.
- `xrdp` auf `3389`: xrdp ist aktiv, Remotr darf den Port nicht uebernehmen.
- `launchd` auf `5900`: macOS Screen Sharing oder Remote Management ist aktiv.

## Uebergabe auf Remotr

Wenn Remotr der aktive Dienst bleiben soll:

```sh
open -a /Applications/Remotr.app
ps aux | rg -i "Remotr"
netstat -anv -p tcp | rg "\\.3389|Local Address|Proto"
nc -vz -w 2 127.0.0.1 3389
```

Erwartung:

- Remotr-Prozess sichtbar
- `3389` im Listener
- lokaler TCP-Test erfolgreich

Tailscale-Pruefung:

```sh
tailscale ip -4
nc -vz -w 3 <tailscale-ip> 3389
```

## Uebergabe auf xrdp

Wenn xrdp wieder uebernommen werden soll, muss zuerst sichergestellt werden,
dass Remotr nicht mehr auf `3389` lauscht.

Pruefung:

```sh
ps aux | rg -i "Remotr|xrdp|xrdp-sesman"
netstat -anv -p tcp | rg "\\.3389|Local Address|Proto"
```

Wichtige letzte Diagnose:

- `SessionSockdirGroup=wheel` ist der korrekte macOS-Wert.
- Der fruehere Wert `root` war falsch und blockierte Session-Sockets.
- Der verbleibende xrdp-Blocker ist `Xorg`:
  - `Xorg` nicht im Daemon-`PATH`
  - keine installierte `xrdp/xorg.conf`
  - keine `xorgxrdp`-Module

## Stop-Kriterien

Eine Session darf nicht blind weitergefuehrt werden, wenn einer dieser Punkte
zutrifft:

- `3389` ist belegt, aber der Besitzer ist unklar.
- Der aktive Dienst wurde geaendert, aber die Ports wurden nicht nachgeprueft.
- Root-geschuetzte Logs wurden nicht frisch gelesen, obwohl ein neuer Fehler
  aufgetreten ist.
- Ein Neustart waere noetig, aber es liegt keine ausdrueckliche Bestaetigung
  vor.

## Handover-Notizvorlage

```text
Aktiver Dienst:
Getesteter Client:
LAN-IP:
Tailscale-IP:
Port 3389 Besitzer:
Port 5900 Besitzer:
Letzte erfolgreiche Pruefung:
Letzter Fehler:
Letzte Konfigurationsaenderung:
Naechster sichere Schritt:
```
