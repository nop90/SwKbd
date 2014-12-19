SwKbd
=====

3DS Custom Software Keyboard Module - by NOP90 - 2014

This is a custom software keyboard I coded in my 3DS file manager project SDiSS.

3DS has a standard swkbd module, but using it is still problematic, so I wrote my own.

The advantage of this module is that it's skinnable.

Commands
=====

A: ends input
B: delete a char (backspace)
X: clear buffer
Left and Right arrows: move cursor
Touchpad: use the keyboard

Sources
=====

the module is composed of 4 files
- swkbd.c (main code)
- swkbd.h (header)
- kbd_default.h (embedded default bmp with a basic keyboard layout)
- swkbfont.h (embedded font)
 
The others source file are for the demo app. 

How to use
=====

the code is provided with a very simple application that shows most of the functionalities. There are no comments, but I think that usage is self explanatory. If not, contact Nop90 on gbatemp.net 

To try the demo app, put in the 3DS SD root keyboard.bmp and MarioBG.BMP provided in the sdmc dir, otherwise default kbd layout and plain black background for top screen will be displayed.

You can try the compiled file with an emulator too. I use 3DMoo, but the touchpad is not well emulated yet.

I tested on a real 3DS only the CIA file with a CFW enviroment.

Note:
The calling app has to initialize 3DS services and the swkbd module.

Dependancies
=====

The module includes only standard C libs and ctrulib. Compile the sources with devkit pro and the last version of ctrulib.

The code has an embedded font (fixed size 12x16 pixel) and an embedded image of a basic keyboard layout.

ToDo (very low priority)
=====

- option to disable a list of keys (i.e. for invalid chars when getting the input for renaming  a file)
- adding sounds on key press

