# EagleEye — Integration Guide

Overview
--------
EagleEye is a lightweight Windows service that helps ensure your application binary hasn't been tampered with. This document explains how to install, configure, and integrate EagleEye into your application's installer and runtime so you can receive violation notifications reliably.
EagleEye is designed to work with your software regardless of what frameworks/libraries you use as long as you integrate EagleEye correctly.

Goals
- Install EagleEye as a Windows service during your application's installation (installer must run elevated).
- Establish and maintain a persistent named-pipe connection with EagleEye.
- Request a token at startup to learn about any offline violations.
- Remain connected so EagleEye can push violation details in real time.

Prerequisites
- Installer must run with Administrator privileges.
- EagleEye app directory must be avaialble in your software developer.
- Target machine: Windows (service + named pipes supported).

Installing the service
----------------------
Install and start EagleEye as part of your application's installer. Run these commands from an elevated prompt (or from your installer process running as admin):

1. Create the service
- Replace absolute-path with the full path to EagleEye.exe (including quotes if the path contains spaces):

```powershell
sc create "EagleEye service" binPath="absolute-path to EagleEye.exe" start=auto DisplayName="EagleEye service"
```

2. Start the service:

```powershell
sc start "EagleEye service"
```

Notes
- The installer must run elevated (Administrator). If you call these commands programmatically, ensure the process has the necessary privileges.
- We recommend to not change the service name and display name.

Verify installation and running state
-------------------------------------
You must verify that the service was created successfully and is in the RUNNING state.

Using sc:

```powershell
sc query "EagleEye service"
```

Expected relevant output includes:
- STATE : 4 RUNNING

Using PowerShell:

```powershell
Get-Service -Name "EagleEye service" | Format-List Name,Status
```

Expected:
- Status : Running

Named-pipe communication
------------------------
EagleEye exposes a single named pipe for integration:

Pipe path: \\\\.\\pipe\\eagleeye

- Connect to the pipe and keep the connection open for the lifetime of your app. Do not disconnect after the initial exchange: EagleEye may send asynchronous violation notifications while your app is running.
- Communication format: JSON messages over the named pipe.
- Message framing: treat JSON messages as newline-terminated.

Workflow at application startup
-------------------------------
1. Connect to \\\\.\\pipe\\eagleeye and maintain a persistent connection.
3. Immediately after a successful pipe connection, request a token to learn if a violation occurred while your app was offline.

Token request (JSON)
- Send this JSON object (replace app_id value as described below):

```json
{
  "cmd": "token_request",
  "app_id": "com.vendor.ExecutableName"
}
```

Rules for app_id
- Use an app identifier where the final segment is your executable filename (without path).
- The last segment is case-insensitive. Example: if your executable is "MyApp.exe", a valid app_id could be "com.vendor.myapp" or "com.vendor.MyApp".

Expected responses
- No violation:

```json
{
  "allowed": true
}
```

- Violation detected:

```json
{
  "allowed": false,
  "details": "Description about the detected violation"
}
```

Keep the pipe connection open so that EagleEye can send further messages (e.g., new violations, updates). The service may send messages at any time.

Example integration snippets
----------------------------
We wrote a simple game for you to look at and see how EagleEye is integrated.
Look in *examples* folder

Best practices
--------------
- Installer must be elevated. Validate elevation before attempting to create/start the service.
- Always verify service creation and running state and handle failures (retry, log, surface to user).
- Keep the named-pipe connection open for the application’s lifetime.
- Use robust JSON parsing and validate fields (e.g., confirm presence and type of "allowed").
- Implement a reconnection strategy: if the pipe connection is lost, attempt to reconnect with exponential backoff. After reconnecting, re-send the token_request.
- EagleEye sends details about the violation. Feel free to do anything with this details.

Security considerations
-----------------------
- Every fixed number of seconds of your choosing check programmatically if EagleEye is running, cheaters might stop the service. So you should terminate your application accordingly.
- You should terminate your application upon receieveng violation message from EagleEye

Troubleshooting
---------------
EagleEye logs everything at: **C:/ProgramData/EagleEye/logs**, in case of EagleEye malfunction, send us the logs file. For contact information, look at the top-level readme file.
- If you cannot connect to \\\\.\\pipe\\eagleeye:
  - Confirm EagleEye service is running.
  - Confirm the service created the pipe(check EagleEye logs).
  - Ensure no firewall or OS policy is blocking IPC(named pipes are local but some policies govern IPC).

Example install + verify script (PowerShell)
------------------------------------------------------

```powershell
# Run as admin
$exePath = "C:\Program Files\YourApp\EagleEye.exe"
sc.exe create "EagleEye service" binPath="\"$exePath\"" start=auto DisplayName="EagleEye service"
Start-Sleep -Seconds 1
sc.exe start "EagleEye service"
Start-Sleep -Seconds 2
sc.exe query "EagleEye service"
```

Contact & Support
-----------------
If you have questions about integration behavior or need examples for different languages, open an integration support request with EagleEye support including:
- OS version
- Path to EagleEye.exe used during installation
- Logs and exact JSON exchanges (scrub sensitive data)

License & distribution
----------------------
Follow your organization's policies for bundling and distributing EagleEye with your product.

---