# Sicherheit

Dieser Fork verbessert macOS-Kompatibilitaet, ohne Developer-ID-Signierung,
Notarisierung oder entitlement-pflichtige Funktionen vorauszusetzen.

## TLS und Trust-Modell

OpenSSL bleibt die TLS-Engine. Das betrifft:

- TLS-Handshakes
- PEM-Zertifikate
- private Schluessel
- bestehende xrdp-TLS-Pfade

Auf macOS koennen `Security.framework` und `CoreFoundation` zusaetzlich fuer
Trust-Diagnostik und ausgehende Zertifikatsbewertung genutzt werden. Dadurch
wird keine Apple-Developer-Account-Abhaengigkeit eingefuehrt.

## Inbound-TLS fuer xrdp

Bei eingehenden xrdp-Verbindungen blockiert die macOS-Trust-Diagnostik in v1
keine selbstsignierten Serverzertifikate. Sie dient der Sichtbarkeit fuer
Operatoren. Die eigentliche TLS-Verarbeitung bleibt im bestehenden OpenSSL-Pfad.

## Outbound-TLS fuer NeutrinoRDP

NeutrinoRDP validiert Zielzertifikate standardmaessig. Der Admin-Opt-out:

```ini
neutrinordp.ignore_certificate=true
```

ist absichtlich explizit und unsicher. Er ist nur fuer Umgebungen gedacht, in
denen die Zertifikatsrisiken separat bewertet und akzeptiert wurden.

Wenn eine FreeRDP-Version nicht die benoetigten Zertifikatsdaten fuer eine
macOS-Trust-Bewertung bereitstellt, darf daraus kein stilles Accept-All
entstehen. Entweder bleibt FreeRDPs eigene Validierung wirksam, oder die
Kompatibilitaetsschicht verhaelt sich konservativ.

## Authentifizierung

macOS-Logins laufen ueber PAM. OpenDirectory wird ueber Apples PAM-Module
eingebunden. Dieser Ansatz passt zum bestehenden sesman-Modell und vermeidet
interaktive lokale Authentifizierungsfluesse, die fuer headless RDP-Sessions
nicht geeignet sind.

Nicht Teil von v1:

- Developer-ID-Signierung
- Notarisierung
- entitlement-pflichtige Features
- LocalAuthentication als Passwortersatz
- Authorization Services als sesman-Login-Backend

## Betriebsregel

Keine sicherheitsrelevante Aenderung sollte stillschweigend aktiviert werden.
Konfigurationsaenderungen muessen nachvollziehbar sein, und Reboots oder
Service-Restarts duerfen nur nach ausdruecklicher Operator-Bestaetigung
erfolgen.
