Instructions for Creating a GCC RX Project for Envision Kit
----------------------------------------------------------

When creating a new project follow this procedure:
--------------------------------------------------

File|New|C/C++ Project

Click Renesas RX

Select GCC for Renesas RX C/C++ Executable Project, Next

Choose a project name, Next

Language: C

Toolchain: GCC for Renesas RX

Target Device: RX600|RX65N|RX65N - 144 pin|R5NED565xFB_DUAL

Tick Create Hardware Debug Configuration

From drop-down choose E2 Lite (RX)

Next, Next

Tick 'Make the double data type 64-bits wide' if required. Next

In 'Select Library Source:' select Newlib

Next, Finish


Setting stack size to 1024, after creating the project
------------------------------------------------------

Open linker_script.ld under generate folder

Go to Graphical Editor

Click on right arrow in RAM block 

In .ustack Output Section change Virtual Memory Address and Load Memory Address of stack from 0x200 to 0x500

In .data Output Section change Virtual Memory Address to 4 more than value above, i.e. 0x504

In project's Properties|C/C++ Build/Settings/Debug change value of 'Warn if stack size exceeds' to 1000.


Setting C variant to ISO C99
----------------------------

Project|Properties

C/C++ Build|Settings|Compiler|Source

In 'Language standard' choose 'ISO C99 (-std=c99)'


Debugging
---------

Open Dubug Configurations (Run|Debug Configurations...)

Open the debug configuration that project generation created for you under Renesas GDB Hardware Debugging drop down called "<your project nam> HardwareDebug"

Click Debugger tab

Click Connection Settings

Scroll down to Power settings and open the group if necessary

Set 'Power The Target from The Emulator (MAX 200mA)' to 'No'

Start debugging
