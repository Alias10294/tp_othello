## Requirements
To use this project, users must acquire a compiler that supports at least C++23 as well as the Meson build system.

## Installation
### Use as a cloned repository
To install Heim as a cloned repository, follow these commands :
```
> cd [directory of your choice]
> git clone https://github.com/Alias10294/tp_othello.git
> meson setup build
```

Once the project is set up, the main executable can be compiled using this command:
```
> meson compile -C build
```
... and executed with this command:
```
> ./build/tp_othello
```