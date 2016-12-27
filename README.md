# termdict
a very tiny light small X dictionary lookup program can run in terminal.

# dependency

xlib
youdao

# install

```bash
pip install youdao

git clone https://github.com/muchrooms/termdict.git
make
./obj/termdict | while read -r line; do yd "$line" ; done
```
`
# todo

* hide the window
* timeout callback
* ~~pipeline friendly~~

# WTFPL

```
            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
                    Version 2, December 2004

 Copyright (C) 2004 Sam Hocevar <sam@hocevar.net>

 Everyone is permitted to copy and distribute verbatim or modified
 copies of this license document, and changing it is allowed as long
 as the name is changed.

            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

  0. You just DO WHAT THE FUCK YOU WANT TO.
```
