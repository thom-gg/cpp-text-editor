# cpp-text-editor


Writing a basic text editor in C++, using as little libraries as possible. Using Qt to have a window system with input listeners that works out of the box, but not using any other built-in functions from Qt.

The point was to focus on optimizing performances, by using data structures like a gap buffer, and more globally to play around with C++.



## Usage
Compile using:
```
mkdir build/
cd build/
qmake ..
make
```

Then start the program:
```
./textEditor
```