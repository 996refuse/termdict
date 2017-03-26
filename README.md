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
sudo make install

# enjoy it!
termdict | while read -r line; do yd "$line" ; done
```
`
# todo

* ~~hide the window~~
* timeout callback
* ~~pipeline friendly~~
* remove chaos code
* correct warning

# WTFPL

http://www.wtfpl.net/
