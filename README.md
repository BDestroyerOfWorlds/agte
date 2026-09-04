# agte

agte (A Graphical Text Editor) is meant to be an alternative to nano. It's written in C and meant to be as light and portable as possible

## Installation

Use the provided .deb or .rpm (Coming soon!) package in the releases seciton to install agte.

Follow your GNU/Linux distribution's package manager usage as normal. 

## Usage

```bash
agte filename.fileextension
```
It will create the file in your current directory, or open it if the file already exists.

Indicators located at the right panel will show if the file exists, if its up to date or not and if the Caps Lock is on or off.

In the current version agte does not check if the file you are opening is a valid text format, if a folder is provided, it will act as normal but saving will be impossible.

This is undefined behavior so please be cautious on providing a valid file.

## Controls

Arrow keys move the cursor to their respective directions.

Page Up moves the cursor to the beginning of the file.
Page Down moves the cursor to the end of the file.

CTRL + C copies the current line to the clipboard.
CTRL + X cuts the current line to the clipboard.
CTRL + V pastes the clipboard on the cursor's current location.

CTRL + S saves the document.

Tab indents 2 characters deep.

In the current version there is no "save as" capability so make sure you always have a copy of the document.

In all honesty having a backup is highly advised, agte is not a production grade program, at least not yet.

## Compilation

```bash
-I. libraylib.a -lGL -lm -lpthread -ldl -lrt -lX11
```

I compile agte with the shown flags, I do not distribute raylib, so please generate your own libraylib.a file.

## Contributing

Feel free to use the issues section, and even fork agte. For further communication, an e-mail address is provided in the LICENSE.txt file.

## License

[AGPL-3.0](https://choosealicense.com/licenses/agpl-3.0/)
