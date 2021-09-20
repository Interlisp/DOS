
### Running Medley 2.0 under FreeDOS 1.3RC4 

## Goal/Objective

Get the DOS version of Medley 2.0 running under FreeDOS and possibly DOSBox. Using DOSBox would allow for use on Android and iOS mobile devices.


## Required Software
1. DOS Medley [DOS Medley Repository](https://github.com/Interlisp/DOS)
   Clone with: ````git clone https://githib.com/Interlisp/DOS.git```` 
   or open in web browser and download ZIP archive
2. FreeDOS [FreeDOS 1.3RC4 download](https://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/distributions/1.3/previews/1.3-rc4/)
   I would recommend  FD13-FULLUSB13.zip.
3. Microsoft Mouse driver [MS Mouse driver](https://winworldpc.com/download/48c2ba4f-c38f-4b5e-11c3-a4c28d587054)
   I couldn't get the CuteMouse driver supplied with FreeDOS to work.
   I used version 8.2 of the Microsoft mouse driver, but can't recall whete I got if from. Maybe the later versions 9 and 11 will also work.


## Hardware used

An ASUS eeePC 701 upgraded to 2G memory (shouldn't matter). CPU: 
Intel Celeron @ 600MHz.

## Installation

1. Copy the Medley repository to the DOS machine e.g. under C:\\TEMP\\DOS-MASTER
2. execute the following DOS commands to unload the CuteMouse driver
   and to load the MS mouse driver.

   ````
      C:\> CTMOUSE /U
      C:\> C:\MOUSE\MOUSE.COM 

   ````
3. execute the following DOS commands to copy Medley files to
   the target directory:
   ````

      C:\> mkdir C:\MEDLEY 
      C:\> copy /B C:\\TEMP\\DOS-MASTER\\DOS486\\MEDLEY.EXE C:\\MEDLEY
      C:\> copy /B C:\\TEMP\\DOS-MASTER\\DOS486\\EMUL.EXE C:\\MEDLEY
      C:\> copy /B C:\\TEMP\\DOS-MASTER\\DOS4GW.EXE C:\\MEDLEY
      C:\> copy /B C:\\TEMP\\DOS-MASTER\\LISP.SYS C:\\MEDLEY
      C:\> copy /B/S C:\\TEMP\\DOS-MASTER\\FONTS C:\\MEDLEY
      C:\> copy /B/S C:\\TEMP\\DOS-MASTER\\LIBRARY C:\\MEDLEY
   ````


## Running Medley

Run

````
      C:\> cd \\MEDLEY
      C:\> MEDLEY LISP.SYS -m 31 -vga
````

Then type "555\<RETURN\>" or some other text. You will then be greeted 
by an XCL EXEC. I don't know why you have to type this.


## Open Points

* The contents of the original disks are under ...\\floppy, but 
  they are password protected. If anybody knows the proper passphrase, 
  please let me know.
* I couldn't find other SYSOUTs than LISP.SYS or any greet or init files.
  There is a FULL.SYS in one of the ZIP archives, though.
* MEDLEY.EXE complains, when more than 32MB memory are requested, but Medley 2.0
  should be able to use 64MB?
* I couldn't get Medley to run under DOSBox yet.

