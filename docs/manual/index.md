# Admin-Handbuch fuer den xrdp macOS/ARD-Fork

Dieses Handbuch richtet sich an Administratoren und Operatoren, die diesen
xrdp-Fork auf macOS bauen, pruefen und betreiben. Der Fokus liegt auf den
fork-spezifischen Erweiterungen fuer macOS-Kompatibilitaet, Apple
Security.framework, X11-freie Builds, NeutrinoRDP und sichere
Zertifikats-Defaults.

Das Handbuch ersetzt nicht die allgemeine xrdp-Upstream-Dokumentation. Es
beschreibt die lokalen Abweichungen und die empfohlenen Betriebsablaeufe fuer
diesen Fork.

## Kapitel

- [macOS-Build](macos-build.md): Voraussetzungen, Submodule,
  Configure-Profile und Build-Kommandos.
- [Konfiguration](configuration.md): relevante Optionen fuer xrdp, sesman und
  NeutrinoRDP.
- [Sicherheit](security.md): Zertifikatsvalidierung, macOS-Trust-Logik,
  PAM/OpenDirectory und bewusst nicht genutzte Apple-Account-Features.
- [Tests](testing.md): lokale Testmatrix, `make check`, `make distcheck` und
  FreeRDP-Verhalten.
- [Betrieb](operations.md): sichere Pruefungen, Log-Hinweise und
  Aenderungsablaeufe ohne automatische Neustarts.
- [Session-Transfer](session-transfer.md): Handover fuer laufende
  Remote-Desktop-Sessions, Port-Besitz und naechste sichere Schritte.
- [Troubleshooting](troubleshooting.md): typische Fehlerbilder und
  gezielte Diagnosepfade.

## Grundsaetze

- Zertifikatsvalidierung bleibt standardmaessig aktiv.
- Unsichere Kompatibilitaetsoptionen muessen explizit gesetzt werden.
- OpenSSL bleibt die TLS-Engine; macOS-Frameworks ergaenzen Trust-Logik und
  Diagnostik.
- X11-freie Builds deaktivieren betroffene Funktionen bewusst zur Build-Zeit.
- Kein Reboot, kein Service-Restart und keine Systemaenderung ohne
  ausdrueckliche Operator-Bestaetigung.
