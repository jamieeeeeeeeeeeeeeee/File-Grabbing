# file-plucker
An alternative to hosting files online. Instead pluck your own files data using data from existing ones.
This can also be used for compressing files! 

So far I have tested this on .exe files, which seems like the best file type for this to work with.
Video files would be interesting to try.

## How it works ##
The plucker.c file finds matching chunks from your file and the *superfile*.
*A good superfile would be a large file that contains data similar to your own!*
The plucker.c then spits out a file that contains the offset and length of the matching chunks.

*I've not implemented the disperse.c file yet!*
The disperse.c file will take the plucker.c output, and using the *superfile* it will recreate your original file.

## Why? ##
I wanted a way to host ~~malicious~~ files without actually hosting them.

## Proof of concept ##
In my proof of concept the compression percentage is only 64%. I'm hoping with a more optimal superfile that this can be made a lot better. Another way of making it small is encoding the chunk file differently. Currently the file is a memory bin of (4 byte int -> 2 byte int). Using hexadecimal and a safer .txt format this could be made a lot smaller.
