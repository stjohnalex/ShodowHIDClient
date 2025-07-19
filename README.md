<p align="center">
  <img src="userspace/wusbip/resources/USBip.svg" width="256" alt="USBip logo"/>
</p>

[![latest release](https://img.shields.io/github/v/release/vadimgrn/usbip-win2?include_prereleases)](https://github.com/vadimgrn/usbip-win2/releases/latest) [![release date](https://img.shields.io/github/release-date-pre/vadimgrn/usbip-win2)](https://github.com/vadimgrn/usbip-win2/releases/latest) [![downloads](https://img.shields.io/github/downloads-pre/vadimgrn/usbip-win2/latest/total)](https://github.com/vadimgrn/usbip-win2/releases/latest) [![commits since](https://img.shields.io/github/commits-since/vadimgrn/usbip-win2/latest/develop?include_prereleases "commits since")](https://github.com/vadimgrn/usbip-win2/commits/develop) [![commit activity](https://img.shields.io/github/commit-activity/m/vadimgrn/usbip-win2/develop "commit activity")](https://github.com/vadimgrn/usbip-win2/commits/develop) [![license](https://img.shields.io/github/license/vadimgrn/usbip-win2)](https://github.com/vadimgrn/usbip-win2/blob/master/LICENSE.txt)

# USB/IP Client for Windows
- Fully compatible with [USB/IP protocol](https://www.kernel.org/doc/html/latest/usb/usbip_protocol.html)
- Works with Linux USB/IP server at least for kernels 4.19 - 6.11
- **[WHLK](https://en.wikipedia.org/wiki/Windows_Hardware_Lab_Kit) certified drivers**
  - Download [Microsoft Hardware Certification Report](https://partner.microsoft.com/en-us/dashboard/hardware/Driver/DownloadCertificationReport/82862560/14228871579043985/1152921505699289069)
  - Certification was made possible thanks to [Open Source Codesigning Initiative](https://github.com/OSSign)
- **Create a [restore point](https://github.com/vadimgrn/usbip-win2/tree/master?tab=readme-ov-file#install-usbip)** before installing USBip
- [Devices](https://github.com/vadimgrn/usbip-win2/wiki#ude-driver-list-of-devices-known-to-work) that work (the list is incomplete)

## Requirements
- Windows 10 x64 Version [1903](https://en.wikipedia.org/wiki/Windows_10,_version_1903) (OS build 18362) and later
- Windows 11 ARM64
- USB/IP server must support protocol v.1.1.1

## Key features
- [UDE](https://learn.microsoft.com/en-us/windows-hardware/drivers/usbcon/developing-windows-drivers-for-emulated-usb-host-controllers-and-devices) driver is an USB/IP client
- A device-specific upper filter driver usbip2_filter is used as companion for UDE driver
- [Winsock Kernel NPI](https://docs.microsoft.com/en-us/windows-hardware/drivers/network/introduction-to-winsock-kernel) is used
  - The driver establishes TCP/IP connection with a server and does data exchange
  - This implies low latency and high throughput, absence of frequent CPU context switching and a lot of syscalls
- [Zero copy](https://en.wikipedia.org/wiki/Zero-copy) of transfer buffers is implemented for network send and receive operations
  - [Memory Descriptor List](https://docs.microsoft.com/en-us/windows-hardware/drivers/kernel/using-mdls) is used to send multiple buffers in a single call ([vectored I/O](https://en.wikipedia.org/wiki/Vectored_I/O))
  - [WskSend](https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/wsk/nc-wsk-pfn_wsk_send) reads data from URB transfer buffer
  - [WskReceive](https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/wsk/nc-wsk-pfn_wsk_receive) writes data to URB transfer buffer
- A dedicated thread is created for each virtual device to receive data from a server

## Differences with [cezanne/usbip-win](https://github.com/cezanne/usbip-win)
- The 2-Clause BSD License since release 0.9.7.0
- WHLK certified drivers
- Brand new UDE driver, not inherited from the parent repo
- Full-featured GUI app
- Userspace code is fully rewritten (libusbip and usbip utility)
- SDK for third party developers (libusbip public API)
- InnoSetup installer is used for installation of drivers and userspace stuff
- Windows 10 version 1903 or later is required
- C++ 20 is used for all projects
- Visual Studio 2022 is used
- Server (stub driver) is removed
- x64/arm64 builds only

## Build

### Build Tools
- [Visual Studio 2022](https://learn.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk#download-icon-for-visual-studio-step-1-install-visual-studio-2022)
- SDK/WDK installation is not required, [NuGet](https://learn.microsoft.com/en-us/windows-hardware/drivers/install-the-wdk-using-nuget#how-to-install-wdk-nuget) will install them automatically

### Build Visual Studio solution
- Install git
  - Run Visual Studio Installer
  - Select "Individual components" tab
  - Type "git" in search box
  - Check "Git for Windows"
  - Install selected items
- Clone project using git or download an archive from github and extract source code
- Run `bootstrap.bat`
- Open `usbip_win2.sln`
- Set certificate driver signing for `package` project
  - Right-click on the `Project > Properties > Driver Signing > Test Certificate`
  - Enter `usbip.pfx` (password: usbip)
- Build the solution
- All output files are created under {x64,ARM64}/{Debug,Release} folders.

## Setup USB/IP server on Ubuntu Linux
- Install required packages
  - Linux `sudo apt install linux-tools-generic linux-cloud-tools-generic`
  - Raspberry Pi `sudo apt install usbip hwdata usbutils`
- Load modules and run the daemon
```
sudo modprobe -a usbip-core usbip-host
sudo usbipd -D
```
- List available USB devices
  - `usbip list -l`
```
 - busid 3-2 (1005:b113)
   Apacer Technology, Inc. : Handy Steno/AH123 / Handy Steno 2.0/HT203 (1005:b113)
 - busid 3-3.2 (07ca:513b)
   AVerMedia Technologies, Inc. : unknown product (07ca:513b)
```
- Bind desired USB device
  - `sudo usbip bind -b 3-2`
```
usbip: info: bind device on busid 3-2: complete
```
- Your device 3-2 now can be used by usbip client

## Setup USB/IP on Windows

### Enable Windows Test Signing Mode if drivers are not signed by Microsoft
- `bcdedit.exe /set testsigning on`
- Reboot the system to apply
- **Do not disable testsigning if USBip has test-signed drivers**, otherwise all USB devices will not work

### Install USB/IP
- **Create a [restore point](https://support.microsoft.com/en-us/windows/create-a-system-restore-point-77e02e2a-3298-c869-9974-ef5658ea3be9)** to undo possible system crashes
  - In the search box on the taskbar, type 'Create a restore point', and select it from the list of results
  - On the System Protection tab in System Properties
    - Make sure system drive protection is enabled
    - Select Create
    - Type a description for the restore point, and then select Create
- Download and run an installer from
  - [Releases](https://github.com/vadimgrn/usbip-win2/releases)
    - Drivers are signed by Microsoft, other binaries (*.exe, *.dll) are test-signed by project's certificate
  - [OSSign](https://github.com/OSSign/vadimgrn--usbip-win2/releases)
    - Drivers and all binaries, including the installer, are signed by Microsoft
    - Use it if officially signed binaries are a mandatory requirement
- Some antivirus programs issue false positives for InnoSetup installer
- **All USB Hub 3.0 devices will be restarted during an installation**
  - This means that all USB devices will stop working for a short time and then start working again.
  - Make sure you don't interrupt your important workflow, such as a video call using a USB webcam, an audio call using a USB headset, copying from/to USB drive, etc.

### Use usbip.exe to attach remote device(s)
- Query available USB devices on the server
  - `usbip.exe list -r <usbip server ip>`
```
Exportable USB devices
======================
 - 192.168.1.9
        3-2: unknown vendor : unknown product (1005:b113)
           : /sys/devices/pci0000:00/0000:00:14.0/usb3/3-2
           : (Defined at Interface level) (00/00/00)
```
- Attach desired remote USB device using its busid
  - `usbip.exe attach -r <usbip server ip> -b 3-2`
```
successfully attached to port 1
```
- New USB device should appear in the system, use it as usual
- Detach the remote USB device using its usb port, pass `-all` to detach all remote devices
  - `usbip.exe detach -p 1`
```
port 1 is successfully detached
```
### Uninstallation of USB/IP
- Uninstall USB/IP app
- Disable test signing if it was enabled during the installation
    - `bcdedit.exe /set testsigning off`
  - Reboot the system to apply
- If the uninstaller is corrupted, run these commands as Administrator
- **If you copy commands to a .bat file, use %%P and %%~nxP in FOR statement**
```
SET HWID=ROOT\USBIP_WIN2\UDE
set APPDIR=C:\Program Files\USBip

"%APPDIR%\devnode.exe" remove %HWID% root
rem alternative command since Windows 11, version 21H2
rem pnputil.exe /remove-device /deviceid %HWID% /subtree

rem WARNING: use %%P and %%~nxP if you run this command in a .bat file
FOR /f %P IN ('findstr /M /L /Q:u "usbip2_filter usbip2_ude" C:\WINDOWS\INF\oem*.inf') DO pnputil.exe /delete-driver %~nxP /uninstall

rd /S /Q "%APPDIR%"
```
## Obtaining USB/IP logs on Windows
- WPP Software Tracing is used
- Use the tools for software tracing, such as TraceView, Tracelog, Tracefmt, and Tracepdb to configure, start, and stop tracing sessions and to display and filter trace messages
- These tools are included in the [Windows Driver Kit](https://learn.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk#download-icon-step-3-install-windows-11-version-22h2-wdk)
- **Pick "Select Components/Program DataBase files" during USBip installation**
- Start log sessions for drivers (run commands as Administrator)
```
rem change to your WDK version
set PATH=%PATH%;C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64

set NAME=usbip

tracelog.exe -stop %NAME%-flt
tracelog.exe -stop %NAME%-ude

del /F %NAME%-*.*
tracepdb.exe -f "C:\Program Files\USBip\*.pdb" -s -p %TEMP%\%NAME%

tracelog.exe -start %NAME%-flt -guid #90c336ed-69fb-43d6-b800-1552d72d200b -f %NAME%-flt.etl -flag 0x3 -level 5
tracelog.exe -start %NAME%-ude -guid #ed18c9c5-8322-48ae-bf78-d01d898a1562 -f %NAME%-ude.etl -flag 0xF -level 5
```
- Reproduce the issue
- Stop log sessions and get plain text logs (run commands as Administrator)
- **If you copy commands to a .bat file, double '%' in TRACE_FORMAT_PREFIX**
```
rem change to your WDK version
set PATH=%PATH%;C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64

set NAME=usbip
set TRACE_FORMAT_PREFIX=[%9]%3!04x! %!LEVEL! %!FUNC!:

tracelog.exe -stop %NAME%-flt
tracelog.exe -stop %NAME%-ude

tracefmt.exe -nosummary -p %TEMP%\%NAME% -o %NAME%-flt.txt %NAME%-flt.etl
tracefmt.exe -nosummary -p %TEMP%\%NAME% -o %NAME%-ude.txt %NAME%-ude.etl

rem sed -i "s/TRACE_LEVEL_CRITICAL/CRT/;s/TRACE_LEVEL_ERROR/ERR/;s/TRACE_LEVEL_WARNING/WRN/;s/TRACE_LEVEL_INFORMATION/INF/;s/TRACE_LEVEL_VERBOSE/VRB/" %NAME%-*.txt
rem sed -i "s/`anonymous namespace':://" %NAME%-*.txt
rem del /F sed*
```

## Debugging [BSOD](https://en.wikipedia.org/wiki/Blue_screen_of_death)
- Enable kernel memory dump
  - Open "System Properties" dialog box
  - Select "Advanced" tab
  - Click on "Settings" in "Startup and Recovery"
  - "System failure", "Write debugging information", pick "Automatic Memory Dump" or "Kernel Memory Dump"
  - Check "Overwrite any existing file"
- Start WPP tracing session for drivers as described in the previous topic
- When BSOD has occured
  - Reboot PC if automatic reboot is not set
  - Run Windows debugger WinDbg.exe as Administrator
  - Press Ctrl+D to open crash dump in C:\Windows
  - Run following commands and copy the output
```
.sympath+ C:\Program Files\USBip
!wmitrace.searchpath +%TEMP%\usbip
!analyze -v
!wdfkd.wdfsearchpath %TEMP%\usbip
!wdfkd.wdfsettraceprefix [%9]%3!04x! %!LEVEL! %!FUNC!:
!wdfkd.wdflogdump usbip2_ude -d
!wdfkd.wdflogdump usbip2_ude -f
```

## Obtaining USB/IP log on Linux
```
sudo killall usbipd
sudo modprobe -r usbip-host usbip-vudc vhci-hcd usbip-core
sudo modprobe usbip-core usbip_debug_flag=0xFFFFFFFF
sudo modprobe -a usbip-host usbip-vudc vhci-hcd
sudo usbipd -D
dmesg --follow | tee ~/usbip.log
```

## Testing the driver
- [Driver Verifier](https://docs.microsoft.com/en-us/windows-hardware/drivers/devtest/driver-verifier) is used for testing
- Run verifier.exe as Administrator
- Enable testing
```
verifier /rc 1 2 4 5 6 8 9 12 18 34  10 11 14 15 16 17 20 24 26 33 35 36 /driver usbip2_filter.sys usbip2_ude.sys
```
- Be aware that rule class 26 "Code integrity checking" forces to use NonPagedPoolNx instead of NonPagedPool
- Query driver statistics
```
verifier /query
```
- Disable testing
```
verifier /reset
```
- To run Static Driver Verifier, set "Treat Warnings As Errors" to "No" for libdrv, usbip2_filter, usbip2_ude projects

### If you like this project
[![paypal](whlk/jpg/paypal_donate_qrcode.png)](https://www.paypal.com/donate/?hosted_button_id=YNZ9GUFWFYHFW)

[![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donate_LG.gif)](https://www.paypal.com/donate/?hosted_button_id=YNZ9GUFWFYHFW)
