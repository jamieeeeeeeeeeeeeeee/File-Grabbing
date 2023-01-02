# File Recylcing
Sort of a compression algorithm? Works on the concept of recycling data!

The compression is done by matching strings of bytes from your target file with a parent file. The compressed output is a binary file containing the offset and length of the matched strings. Decompression is done by reading the parent file rewriting the original file using the offset and length of the matched strings.

# Usage #
To compress a file `implode <parent-file> <target-file>`

To decompress a file `explode <parent-file> <target-file>`

I have provided an example with instructions so you can test it out yourself! This example 'compresses' helloworld.exe to 89% of its original size!

# Notes! #
**Please beware that this is very likley to be unsafe!!**

Also it is not really compression, as the parent file is still needed (and is likely to be larger than the original file*). However, I do think this concept is interesting, probably already exists, and could be built upon and for something actually useful!

*Even though this is the case, compressing multiple files using the same parent file will eventually make sense!

I initially did this because the I had a virus written for a (Pico) USB Rubber Ducky, however the entire file could not fit on the device, and I did not want to download the host the file on the internet (because then it could be traced). So instead I put the explode.exe file (only 361kb) as well as the outputted chunk file on the device, and downloaded a parent file instead. (This parent file is a random GitHub executable file from someone elses repo, so nothing could be traced back to me!) and rebuilt the virus on the target machine! In this case the limitation is that you still need explode.exe which will likley be caught by Anti-Virus software, as it builds and runs an executable file.

However, I as mentioned above, compressing multiple files using the same parent file could be beneficial to server hosters (if it works well for other file types). A (probably bad) example of this would be YouTube! After finding an (or more likley set of) optimal parent file(s), each user could have these files download, and then YouTube would only have to store (probably not) and send the chunk files to the user. This would save a lot of space on the server, and would also save a lot of bandwidth for the user and YouTube!
