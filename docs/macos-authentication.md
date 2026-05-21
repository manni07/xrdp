# macOS authentication and trust notes

This fork keeps macOS support free of Apple Developer Account requirements.
Developer ID signing, notarization and entitlement-gated features are outside
this scope.

## Authentication

xrdp authentication on macOS remains PAM-based. The repository already includes
`instfiles/pam.d/xrdp-sesman.macos`, which uses Apple-provided PAM modules such
as OpenDirectory and launchd integration.

Alternatives reviewed for this refactor:

- OpenDirectory through PAM: preferred for v1, as it fits the current sesman
  authentication model and does not require app entitlements.
- Authorization Services: not used in v1. It is better suited to privileged
  authorization flows than headless remote session password checks.
- LocalAuthentication: not used in v1. It is intended for interactive local
  biometric/passcode prompts and is not a drop-in replacement for xrdp logins.

## Certificate trust

OpenSSL remains the TLS engine. On macOS, `Security.framework` and
`CoreFoundation` can be enabled for certificate trust diagnostics and outbound
certificate evaluation where the available API exposes certificate data.

For inbound xrdp TLS, macOS trust evaluation is diagnostic only. A self-signed
server certificate logs a warning but does not block the TLS handshake.

For NeutrinoRDP outbound TLS, certificate validation remains enabled by default.
The admin-only escape hatch is:

```ini
neutrinordp.ignore_certificate=true
```

Using this option disables target certificate validation and should only be used
for explicitly accepted legacy or lab environments.

## Build prerequisites on macOS

The local macOS test environment expects Homebrew and Xcode Command Line Tools.
For tests, install:

```sh
brew install check cmocka
```

For NeutrinoRDP verification, FreeRDP is also required:

```sh
brew install freerdp
```

If Homebrew installs FreeRDP without a `freerdp.pc` visible to `pkg-config`,
set `PKG_CONFIG_PATH` to the FreeRDP package config directory before running
`./configure --enable-neutrinordp`.

Homebrew FreeRDP 3.x currently exports `freerdp5.pc`. The existing
NeutrinoRDP module still uses the legacy direct settings API and therefore
requires a compatible `freerdp.pc` package until a dedicated FreeRDP 5 port is
implemented.

Do not reboot or restart the machine as part of this setup without explicit
operator confirmation.
