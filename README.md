# cpp-text-editor
<img width="711" height="204" alt="image" src="https://github.com/user-attachments/assets/91ea4597-f13b-4d85-9e19-01e948db7ca7" />


Writing a basic text editor in C++, using as little libraries as possible. Using Qt to have a window system with input listeners that works out of the box, but not using any other built-in functions from Qt.

The point was to focus on optimizing performances, by using data structures like a gap buffer, and more globally to play around with C++.



# Usage
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

# Features
## Memory management
To handle the text content of the editor, I implemented a gap buffer, which is a data structure made for quick insertion / deletion of characters inside a string.

It works by leaving a gap in the buffer (captain obvious), that does not represent the actual content but just some unused memory area that can be used on the spot when needed.

When inserting characters, the gap may ran out of space, therefore it'll need to allocate more memory, but for most interactions this won't happen so average time complexity for insertion or deletion is O(1).

More info: [https://en.wikipedia.org/wiki/Gap_buffer](https://en.wikipedia.org/wiki/Gap_buffer)

## Other
Implemented:
- line breakings
- zoom in and out
- scrolling
- text selection
- word selection with double click
- copy and paste
- undo / redo stacks
- load and save files
