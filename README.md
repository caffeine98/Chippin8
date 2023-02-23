# Chippin8

Chippin8 is an emulator for a CHIP-8 system, which is an interpreted programming language and virtual machine that was used in early microcomputers such as COSMAC VIP. The simplicity of the virtual machine makes it a great introduction for designing emulators. 

This emulator is capable of running CHIP-8 ROMs (.ch8 files) and uses the keyboard for input. CHIP-8 uses a hex keyboard layout, so 16 keys in total. This emulator uses the following:
```
1 | 2 | 3 | 4
Q | W | E | R
A | S | D | F
Z | X | C | V
```

# Dependencies

Chippin8 uses the SDL2 library to render graphics to the screen, handle keyboard inputs, as well as set the frame rate of the display. 

# Build

Use the Chippin8.sln file to build the project in Visual Studio. Make sure you have SDL2 installed and that the include and library paths are set appropriately in your Project Settings. You can find the built executable in ./x64/Debug/Chippin8.exe.

# Usage

Run the program through the Terminal/Powershell and provide the path of a CHIP-8 ROM file as its argument. Optionally, you can set the display scale size and the cycle delay as addidional arguments.
```
./<Chippin8>.exe <ROM_file>.ch8 [Video Scale (number)] [Cycle Delay (number)]
```
# Screenshots
![screenshotIBM](https://user-images.githubusercontent.com/49334026/220876075-e9735ca0-f091-4bb0-99e1-3cd08d86bb45.png)
![screenshotSoccer](https://user-images.githubusercontent.com/49334026/220876088-5b0be5c8-c3e2-46a6-8058-012084dd78da.png)


# References

I used a variety of sources and leanred a lot while doing this project. Here are some references I used to learn and understand the system and write the emulator
- https://en.wikipedia.org/wiki/CHIP-8
- https://austinmorlan.com/posts/chip8_emulator/
- https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
- https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
- https://www.freecodecamp.org/news/creating-your-very-own-chip-8-emulator/ (This is written in Javascript, but the content applies to any language)
- http://emulator101.com/
