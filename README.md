# agte

agte (A Graphical Text Editor) is meant to be an alternative to nano. It's written in C and meant to be as light and portable as possible.

## Installation

Use the provided .deb or .rpm package in the releases seciton to install agte. Follow your GNU/Linux distribution's package manager usage as normal. 

Unfortunately, other package formats are not offered yet and users are expected to compile from source.

## Usage

```bash
agte filename.fileextension
```
(giving an extension is optional and you can have a plain text document with no extension at all if you just provide a name)

It will create the file in your current directory, or open it if the file already exists.

Indicators located at the right panel will show if the file exists, if its up to date or not and if the Caps Lock is engaged or not.

In the current version agte does not check if the file you are opening is a valid text format, if a folder is provided, it will act as normal but saving will be impossible.

This is undefined behavior so please be cautious on providing a valid file.

## Controls

Arrow keys move the cursor to their respective directions.

Page Up moves the cursor to the beginning of the file.
Page Down moves the cursor to the end of the file.

All movement keys combined with the "Shift" key will move your selection area with the cursor. "Shift" needs to be held down for adjustments and the selection area will reset if any input is given or a movement key is pressed without holding "Shift".

CTRL + C copies the selected area or the current line to the clipboard.
CTRL + X cuts the selected area or the current line to the clipboard.
CTRL + V pastes the clipboard on the cursor's current location.

if no selection is provided, these controls will work on the current line as whole ('\n' to '\n'). In other words, it falls back/defaults the line selection.

CTRL + S saves the document.

In the current version there is no "save as" capability so make sure you always have a copy of the document if you wish to keep it.

Tab indents 2 characters deep.

## Compilation

```bash
-I. libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11
```

I compile agte with the shown flags, I do not distribute raylib, so please generate your own libraylib.a file if you wish to make your own portable binary with links or package it for your format of choice.

## Contributing

Feel free to use the issues section, and even fork agte. For further communication, "bkeskinsoftware@gmail.com".

Are you willing to package agte for your package manager of choice? Let me know about it!

## License

[AGPL-3.0](https://choosealicense.com/licenses/agpl-3.0/)

## Known limitations of agte as a footnote
* Window size is set at 1280x720.
* No "valid format" check present.
* No full UTF support has been implemented.
* No "undo" and "redo" functions.
* Although it is a standalone GUI program, it is still meant to be launched in the terminal.
* Poor Caps Lock logic that only shows if the state has been changed, doesn't check the actual position. Kept that way to ensure OS Agnostic nature.

> These limitations are to be tackled in following versions along with a dedicated file explorer and a superior layout handling system, perhaps even a terminal emulator. Although lack of UTF-8 support is a great limitation for a text editor, it is a change that requires going back to the drawing board. The whole project needs an actual redesign to be quite honest, and I'm working on it. Decisions have to be made around the intended platforms so stuff like actually fetching the Caps Lock position and more become possible. Community feedback is extremely valuable and will be taken into account.

Do you have any ideas or would you like to help in any way? Please let me know via e-mail.
