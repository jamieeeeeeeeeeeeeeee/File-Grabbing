# File Recylcing
The compression is done by matching strings of bytes from your target file with a parent file. The compressed output is a binary file containing the offset and length of the matched strings. Decompression is done by reading the parent file rewriting the original file using the offset and length of the matched strings.

# Usage #
To compress a file `implode <parent-file> <target-file>`

To decompress a file `explode <parent-file> <target-file>`

