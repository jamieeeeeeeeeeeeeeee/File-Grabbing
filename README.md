# File Grabbing
Sort of a compression algorithm? 

The compression is done by matching strings of bytes from your target file with a parent file. The compressed output is a binary file containing the offset and length of the matched strings. Decompression is done by reading the parent file rewriting the original file using the offset and length of the matched strings.

# Usage #
To compress a file `implode <parent-file> <target-file>`

To decompress a file `explode <parent-file> <target-file>`

I have provided an example with instructions so you can test it out yourself! This example 'compresses' helloworld.exe to 89% of its original size!

# Notes! #
**Please beware that this is very likley to be unsafe!!**

**Also it is not really compression, as the parent file is still needed (and is likely to be larger than the original file). However, I do think this concept is interesting, probably already exists, and could be built upon and for something actually useful!**