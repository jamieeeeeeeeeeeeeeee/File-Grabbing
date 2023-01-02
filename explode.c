#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// define byte array
typedef struct {
  size_t size;
  unsigned char *data;
} ByteArray;

// this will be the program that reforms the file from the chunks
int main(int argc, char *argv[]) {
  // check for correct number of arguments 
  if (argc != 3) {
    printf("Usage: %s <super file> <chunk.bin>", argv[0]);
    return 1;
  }

  // open chunk file
  FILE *chunk_file = fopen(argv[2], "rb");
  if (chunk_file == NULL) {
    printf("Error opening file");
    return 1;
  }

  // get file size
  fseek(chunk_file, 0, SEEK_END);
  long chunk_file_size = ftell(chunk_file);
  rewind(chunk_file);

  // create bytearray for chunk file
  ByteArray *chunk_bytes = (ByteArray*)malloc(sizeof(ByteArray));
  chunk_bytes->size = chunk_file_size;
  chunk_bytes->data = (unsigned char*)malloc(chunk_file_size);

  // read chunk file into bytearray
  if (fread(chunk_bytes->data, 1, chunk_file_size, chunk_file) == 0) {
    printf("Error reading file");
    return 1;
  }

  // close chunk file
  fclose(chunk_file);

  // open super file
  FILE *super_file = fopen(argv[1], "rb");
  if (super_file == NULL) {
    printf("Error opening file");
    return 1;
  }

  // get file size
  fseek(super_file, 0, SEEK_END);
  long super_file_size = ftell(super_file);
  rewind(super_file);

  // create bytearray for super file
  ByteArray *super_bytes = (ByteArray*)malloc(sizeof(ByteArray));
  super_bytes->size = super_file_size;
  super_bytes->data = (unsigned char*)malloc(super_file_size);

  // read super file into bytearray
  if (fread(super_bytes->data, 1, super_file_size, super_file) == 0) {
    printf("Error reading file");
    return 1;
  }

  // close super file
  fclose(super_file);

  // create original file
  FILE *original_file = fopen("original.exe", "wb");
  if (original_file == NULL) {
    printf("Error writing file");
    return 1;
  }

  // begin reading chunks (chunks are formatted 4 BYTE OFFSET | 2 BYTE SIZE) (146940 / 6 = 24490)
  int chunk_offset = 0;
  while (chunk_bytes->size > 0) {
    // get offset
    unsigned long offset = 0;
    memcpy(&offset, chunk_bytes->data + chunk_offset, 4);
    // get size
    unsigned short size = 0;
    chunk_offset += 4;
    memcpy(&size, chunk_bytes->data + chunk_offset, 2);
    chunk_offset += 2;
    // write chunk from superfile into original
    printf("offset: %d, size: %d\n", offset, size);
    fwrite(super_bytes->data + (size_t)offset, 1, size, original_file);

    chunk_bytes->size -= 6;
  }
  printf("Finished!\n");
}