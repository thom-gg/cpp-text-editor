# cpp-text-editor
<img width="711" height="204" alt="image" src="https://github.com/user-attachments/assets/91ea4597-f13b-4d85-9e19-01e948db7ca7" />


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
