# EagleEye
## Anti-tampering/Anti-cheat service application

EagleEye is a Windows, macOS, Linux service/deamon aimed to protect desktop applications and games against tampering.

# Features
* Protects your software files from being modified using hashing.
* Detects if your software is running in a debugger.
* Realtime DLL injection detection.
* IPC to communicate with your software.

EagleEye is developed as a service/daemon to always be running in the background with no user interactions. And to not show annoying UAC popup to ask for privileges. And to make integration easier. 
On Windows: It communicates with your software using a named-pipe.

Documentation is available in **docs** folder.

## Tech
EagleEye uses several projects to work
* Qt
* OpenSSL

## Contact
We're always happy to help.

**Discord:** `shehab6600`

## LICENSE

Apache License v2.0
