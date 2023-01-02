# Example Instructions #
You can use the pre-compiled test.exe file (or compile it yourself if you don't trust me :)).

**(Note these files are not in the same directory as implode/explode.exe!)**
Run `implode.exe [parentfile.exe] [helloworld.exe]` to shrink the hello world executable.
This will output a file named `helloworld-output.bin` which contains the compressed data.

Running `explode.exe [parentfile.exe] [helloworld-output.bin]` will decompress the file back to its original state.
This will output a file named `original.exe`. If you run original.exe, you should see it prints "Hello World!" :D !
