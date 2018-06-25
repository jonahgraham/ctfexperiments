Example of using Babeltrace
====

Steps to run examples:

- ``mkdir -p /scratch/tracecompass/git``
- `cd /scratch/tracecompass/git`
- `git clone git@github.com:efficios/babeltrace.git `
- `cd babeltrace`
- Get the Work In Progress for babeltrace 2.0 from the author working on mingw support
  - `git remote add mjeanson git@github.com:mjeanson/babeltrace.git`
  - `git fetch mjeanson`
  - `git checkout port-staging`
  - As the this branch is a WIP, I have captured the version I used as https://github.com/jonahgraham/babeltrace/tree/ctfexperiments, to use it:
  - `git remote add jonahgraham git@github.com:jonahgraham/babeltrace.git`
  - `git fetch jonahgraham`
  - `git checkout ctfexperiements`
-  Install dependencies listed in README, (for Windows, see below)
- `./bootstrap`
- `./configure --prefix=/scratch/tracecompass/babeltrace/babeltrace-port-staging`
- `make`
  - On Windows this failed for me in `CCLD babeltrace.exe` with errors like `undefined reference to 'libiconv'`, this is the workaround I used
  - `cd cli`
  - `make V=1 LIBS= babeltrace.exe`
  - `cd ..`
  - `make`
- `make install`
- `cd /scratch/tracecompass/git`
- `git clone --this repo-- # if you haven't already`
- `cd --this repo--`
- `cd writer`
- update BABELTRACE in Makefile to prefix passed to configure
- `make # build the example`
- `make run # runs creation of a simple CTF file in /tmp/xyz`
  - On Windows the /tmp/xyz will be on the current drive, e.g. d:\tmp\xyz
- Open the CTF in Tracecompass


Windows
=======

To get a sane Windows development environment for CTF I recommend:

- Get http://repo.msys2.org/distrib/x86_64/msys2-x86_64-20161025.exe (or latest from http://www.msys2.org/) and install
- Start MSYS2 MSYS (from installer or start menu)
- `pacman -Syu # update core packages`
- close shell if instructed to, see instructions in shell
- start MSYS2 MSYS again (from start menu)
- `pacman -Su # update remaining packages`
- `pacman -S base-devel # choose all to get all normal dev tools`
- `pacman -S mingw-w64-i686-gcc # 32-bit windows cross compiler`
- `pacman -S mingw-w64-x86_64-gcc # 64-bit windows cross compiler`
- `pacman -S mingw-w64-i686-gtk2 # 32-bit gtk library (for glib)`
- `pacman -S mingw-w64-x86_64-gtk2 # 64-bit gtk library (for glib)`
- `pacman -S mingw-w64-i686-popt # 32-bit libpopt`
- `pacman -S mingw-w64-x86_64-popt # 64-bit libpopt`
- `pacman -S automake autoconf pkg-config gettext autogen libtool python3 bison flex`
- MSYS2 MinGW 64-bit (from start menu) -- this is the shell to build from (use 32-bit if needed)
- `gcc --version -v # show details about compiler being used`

