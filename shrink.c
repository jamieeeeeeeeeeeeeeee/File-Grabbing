#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHUNK 65535 // so chunks sizes can be stored in 16 bits

// redefining memmem because it's unavailable on Windows
// https://stackoverflow.com/questions/52988769/writing-own-memmem-for-windows
size_t memmem(const void *haystack, size_t haystack_len, 
    const void * const needle, const size_t needle_len)
{
    if (haystack == NULL || haystack_len == 0 || needle == NULL || needle_len == 0) return 0;

    size_t i = 0;
    for (const char *h = haystack; haystack_len >= needle_len; ++h, i++, --haystack_len) {
        // check if the first byte matches and then memcmp the rest
        if (!memcmp(h, needle, 1) && !memcmp(h, needle, needle_len)) {
            return (i + 1);
        }
    }
    return 0;
}

// define byte array
typedef struct {
  size_t size;
  unsigned char *data;
} ByteArray;

// define result struct
typedef struct {
  size_t target_file_start;
  size_t chunk;
  size_t super_file_start;
  char error;
} Result;

Result find_biggest_chunk (ByteArray* target_file, ByteArray* super_file) {
  Result result;
  
  result.target_file_start = 0;
  result.chunk = 0;
  result.error = 0;
  result.super_file_start = 0;

  size_t start = 0;
  size_t chunk = 1;
  size_t best_chunk = 0;
  size_t end = 1;
  do {
    end = start + chunk;
    // find first occurance of initial byte matching
    size_t memres = memmem(super_file->data, super_file->size, target_file->data + start, chunk);
    if (memres == 0) {
      // no match found so go to the next start position
      start++;
    }
    else {
      // match found so increase chunk size
      result.target_file_start = memres + chunk - 1; // -1 because i + 1 so i does not return as NULL if 0
      start = end;
      do {
        chunk++;;
      }
      while (super_file->data[result.target_file_start++] == target_file->data[start++] && start < target_file->size && result.super_file_start < super_file->size);
      // printf for debugging 
      chunk--;
      best_chunk = chunk;
      end = start + chunk;
      result.target_file_start = start - chunk;
      result.super_file_start = result.super_file_start - chunk;
      printf("target_file_start: %d, chunk: %d, super start: %d\n", result.target_file_start, best_chunk, result.super_file_start);

    }
  } while (end < target_file->size);

  if (chunk > MAX_CHUNK) {
    chunk = MAX_CHUNK;
  }

  result.target_file_start = result.target_file_start;
  result.chunk = chunk;
  result.super_file_start = memmem(super_file->data, super_file->size, target_file->data + result.target_file_start, best_chunk);
  if (result.super_file_start == 0) {
    printf("Error: Match found but then lost?! (Bad logic?!) start: %d, chunk: %d, end: %d\n", start, chunk, end);
    result.error = 1;
  }

  // printf result for debuggings sake
  printf("start: %d, chunk: %d, super_file_start: %d\n", result.target_file_start, result.chunk, result.super_file_start);

  return result;
}

int main(int argc, char *argv[]) {
  // pre checks!
  if (argc != 3) {
    printf("Usage: %s <super file> <target file>", argv[0]);
    return 1;
  }

  if (sizeof(unsigned long) != 4 || sizeof(unsigned short) != 2){
    printf("Error unsigned long or unsigned short are too big!");
    return 1;
  }

  // open file to be shrank
  FILE *target_file = fopen(argv[2], "rb");

  if (target_file == NULL) {
    printf("Error opening file");
    return 1;
  }

  // get file size
  fseek(target_file, 0, SEEK_END);
  long target_file_size = ftell(target_file);
  rewind(target_file);

  // create bytearray for target file
  ByteArray *target_bytes = (ByteArray*)malloc(sizeof(ByteArray));
  target_bytes->size = target_file_size;
  target_bytes->data = (unsigned char*)malloc(target_file_size);

  // read target file into bytearray
  if (fread(target_bytes->data, 1, target_file_size, target_file) == 0) {
    printf("Error reading file");
    return 1;
  }

  // close target file
  fclose(target_file);

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

  // Algorithm Testing \\.

  // create output.bin file
  FILE *output_file = fopen("output.bin", "wb");
  size_t new_size = 0;
  
  // biggest chunk algorithm (this is magnitudes slower than linear)
  /*
  do {
    Result result = find_biggest_chunk(target_bytes, super_bytes);
    // remove the chunk from the target file
    memmove(target_bytes->data + result.target_file_start, target_bytes->data + result.target_file_start + result.chunk, target_bytes->size - result.target_file_start - result.chunk);
    target_bytes->size -= result.chunk;

    // temporarially add new_size
    new_size += 6; // this is just for now
  }
  while (target_bytes->size <= 0); // it should == 0 
  if (target_bytes->size < 0) {
    printf("Error: target_bytes->size == ");
    return 1;
  }
  */

 // linear algorithm
  size_t start = 0;
  size_t chunk = 1;
  size_t target_start = 0;
  do {
    size_t memres = memmem(super_bytes->data, super_bytes->size, target_bytes->data + start, chunk);
    if (memres == 0) {
      if (chunk == 1) {
        printf("Single byte without a match :(");
        break;
      }
      else {
        if (chunk > MAX_CHUNK) {
          chunk = MAX_CHUNK;
        } 
        // 4 bytes 
        chunk -= 1;
        start += chunk;

        int old = ftell(output_file);
        // write start (4 bytes) followed by chunk (2 bytes) to output file
        unsigned long offset = (unsigned long) target_start;
        unsigned short size = (unsigned short) chunk;
        fwrite(&offset, 4, 1, output_file);
        fwrite(&size, 2, 1, output_file);     
        
        // double check size of file
        int new = ftell(output_file);
        if (new - old != 6) {
          printf("Error: Something went wrong! Wrote %d bytes instead of 6", new - old);
          return 1;
        }
        printf("Byte %d of %d\r", start, target_bytes->size);

        chunk = 1;
        new_size += 6;
      }
    }
    else {
      target_start = memres - 1;
      chunk++;
    }
  } while(start < target_bytes->size);

  // read size of output.bin
  fseek(output_file, 0, SEEK_END);
  size_t output_file_size = ftell(output_file);
  if (new_size != output_file_size) {
    printf("Error: File size doesn't look correct! >:( (%d != %d)", new_size, output_file_size);
    return 1;
  }
  printf("\n\nNew size: %d\n", new_size);
  printf("Old size: %d\n", target_file_size);
  printf("\nCompression ratio: %f\n", (float)new_size / (float)target_file_size);
  printf("Compression percentage: %f\n", 100 - ((float)new_size / (float)target_file_size) * 100);

  // clean up 
  free(target_bytes->data);
  free(target_bytes);
  free(super_bytes->data);
  free(super_bytes);
  fclose(output_file);
  return 0;
}
