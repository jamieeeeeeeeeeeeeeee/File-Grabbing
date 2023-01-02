# File Grabbing
Sort of a compression file but also not really! It turns your original file into a file containing chunks of matching data with a superfile. And now anyone with access to the superfile, and your chunk file can recreate your original file!

So far I have tested this on .exe files, which seems like the best file type for this to work with.
Video files would be interesting to try.

.c file will take the shrink.c output (output.bin), and using the *superfile* it will recreate your original file.

## Why? ##
I wanted a way to host ~~malicious~~ files without actually hosting them.

## Proof of concept ##
In my proof of concept the compression percentage is only 89%. I'm hoping with a more optimal superfile that this can be made a better.
Additionally, there will be better ways to store the output.bin data, rather than just storing (4 bytes offset, 2 bytes length) in a binary file.
