## Windows

1) Install Qt 6.7.3 and Qt Creator using the [qt-online-installer](https://www.qt.io/download-qt-installer)
2) Clone the repo `git clone https://github.com/Factor-64/ra2snes.git --resursive`
3) Compile rcheevos using MingW64 (either the one built into Qt or using your installation)
    ```
    cd ra2snes/rcheevos/test
    make ARCH=x64 BUILD=c89 CC=gcc test
    ```
6) Open the CMakeFile as a project in Qt Creator.
7) Build and Run

## Linux
