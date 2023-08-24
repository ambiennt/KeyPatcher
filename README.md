# KeyPatcher

### A programmatic approach to fixing MCBE's broken Xbox Live authentication
- [Why is this needed?](https://blog.tobiasgrether.com/2023/08/23/how-microsoft-broke-minecraft.html)
- Patch any applicable Minecraft executable with an updated public Xbox Live authentication key (with one caveat)
- Supported executables include:
  - Minecraft for Windows (`Minecraft.Windows.exe`)
  - Bedrock Dedicated Server for Windows (`bedrock_server.exe`)
  - Bedrock Dedicated Server for Windows - modded (`bedrock_server_mod.exe`)
  - Bedrock Dedicated Server for Linux (`bedrock_server`)
  - Minecraft for Android (`libminecraftpe.so`)

### Limitations with patching appx files
- Appx is a file package format used by UWP apps, including Minecraft for Windows
- The default installation directory where the `Minecraft.Windows.exe` executable is stored is heavily (and obnoxiously) protected by Windows
- One problem lies within the inability of 3rd party programs to write anywhere inside the WindowsApps folder by default; this makes the patching process fairly inconvenient
- Additionally, even if write restrictions were bypassed, applying changes to the contents of an appx requires sideloading, or else the app will fail to run due to mismatching hashes
- By default, the contents of Minecraft appx files are installed in `%ProgramFiles%\WindowsApps\Microsoft.MinecraftUWP_*_8wekyb3d8bbwe`, where `*` will vary depending on your installed Minecraft version
- For example, if you have the 64 bit version of Minecraft v1.16.40 installed, the directory would be `%ProgramFiles%\WindowsApps\Microsoft.MinecraftUWP_1.16.4002.0_x64__8wekyb3d8bbwe`
- Note that you will be denied viewing access if you have not explicitly taken ownership of the WindowsApps folder, but you don't need to do that in order to use the patcher

### Workarounds
- 3rd party tools, such as [mc-w10-version-launcher](https://github.com/MCMrARM/mc-w10-version-launcher) can be used to sideload appx files (which may or may not have modified contents)
- This tutorial assumes you are familiar with sideloading appx files and/or using 3rd party version managers for Minecraft for Windows
- I recommend using mc-w10-version-launcher as I believe it is the most straightforward and reliable
- By using this launcher (and more generally speaking, sideloading Minecraft for Windows), your installation path is in a write-accessible directory and changes can be easily applied without hassle
- The location of the `Minecraft.Windows.exe` executable in this custom installation path is the same path you will specify when running the patcher

### How to use
- Make sure the Minecraft executable you wish to patch is closed beforehand
- Download the latest release of the patcher [here](https://github.com/ambiennt/KeyPatcher/releases/)
- If you are patching Minecraft for Windows, follow the steps described in the [Workarounds header](https://github.com/ambiennt/KeyPatcher#workarounds)
- You may need to run the patcher with administrator permissions
- Follow the prompts in the console as required

### License
- GPLv3
- However, you are not permitted to put this tool behind some sort of paywall, such as a 3rd party link shortener or other paid product

### Other remarks
- I am aware that the process to apply a seemingly simple patch to Minecraft for Windows is quite complex for the majority of users who do not use version managers, but this is largely due to Microsoft's restrictive UWP application ecosystem
- There may be better ways to streamline this entire process, so contributions are welcome and encouraged