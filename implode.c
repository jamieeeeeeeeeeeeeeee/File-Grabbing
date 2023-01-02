#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHUNK 65535 // so chunks sizes can be stored in 16 bits

// redefining memmem because it's unavailable on Windows
// https://stackoverflow.com/questions/52988769/writing-own-memmem-for-windows
size_t memmem(const void *haystack, size_t haystack_len, const void * const needle, const size_t needle_len) {
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
  size_t tstart;
  size_t pstart;
  size_t chunk;
  char error;
} Result;

Result find_biggest_chunk (ByteArray* tfile, ByteArray* pfile) {
  Result result;
  
  result.tstart = 0;
  result.chunk = 0;
  result.error = 0;
  result.pstart = 0;

  size_t start = 0;
  size_t chunk = 1;
  size_t best_chunk = 0;
  size_t end = 1;
  do {
    end = start + chunk;
    // find first occurance of initial byte matching
    size_t memres = memmem(pfile->data, pfile->size, tfile->data + start, chunk);
    if (memres == 0) {
      // no match found so go to the next start position
      start++;
    }
    else {
      // match found so increase chunk size
      result.tstart = memres + chunk - 1; // -1 because i + 1 so i does not return as NULL if 0
      start = end;
      do { 
        chunk++;;
      }
      while (pfile->data[result.tstart++] == tfile->data[start++] && start < tfile->size && result.pstart < pfile->size);
      
      // printf for debugging 
      chunk--;
      best_chunk = chunk;
      end = start + chunk;
      result.tstart = start - chunk;
      result.pstart = result.pstart - chunk;
      printf("tstart: %d, chunk: %d, parent start: %d\n", result.tstart, best_chunk, result.pstart);
    }
  } while (end < tfile->size);

  if (chunk > MAX_CHUNK) {
    chunk = MAX_CHUNK;
  }

  result.tstart = result.tstart;
  result.chunk = chunk;
  result.pstart = memmem(pfile->data, pfile->size, tfile->data + result.tstart, best_chunk);
  if (result.pstart == 0) {
    printf("Error: Match found but then lost?! (Bad logic?!) start: %d, chunk: %d, end: %d\n", start, chunk, end);
    result.error = 1;
  }

  // printf result for debuggings sake
  printf("start: %d, chunk: %d, pstart: %d\n", result.tstart, result.chunk, result.pstart);
  return result;
}

int main(int argc, char *argv[]) {
  // Pre Checks! \\.
  if (argc != 3) {
    printf("Usage: %s <parent file> <target file>", argv[0]);
    return 1;
  }

  if (sizeof(unsigned long) != 4) {
    printf("Error: sizeof unsigned long is not 4 bytes! [UNSUPPORTED ARCHITECTURE]");
    return 1;
  }

  if (sizeof(unsigned short) != 2) {
    printf("Error: sizeof unsigned short is not 2 bytes! [UNSUPPORTED ARCHITECTURE]");
    return 1;
  }

  // Parent File Setup (p) \\.
  // open parent file
  FILE *pfile = fopen(argv[1], "rb");

  if (pfile == NULL) {
    printf("Error opening parent file!");
    return 1;
  }

  // get file size
  fseek(pfile, 0, SEEK_END);
  long psize = ftell(pfile);
  rewind(pfile);

  // create bytearray for parent file
  ByteArray *pbytes = (ByteArray*)malloc(sizeof(ByteArray));
  if (pbytes == NULL) {
    printf("Error allocating memory for parent file!");
    return 1;
  }
  pbytes->size = psize;
  pbytes->data = (unsigned char*)malloc(psize);
  if (pbytes->data == NULL) {
    printf("Error allocating memory for parent file bytes!");
    return 1;
  }
  
  // read parent file into bytearray
  if (fread(pbytes->data, 1, psize, pfile) == 0) {
    printf("Error reading parent file!");
    goto cleanup;
  }

  // close parent file
  fclose(pfile);

  // Target File Setup (t) \\.
  // open file to be compressed
  FILE *tfile = fopen(argv[2], "rb");
  if (tfile == NULL) {
    printf("Error opening target file!");
    goto cleanup;
  }

  // get file size
  fseek(tfile, 0, SEEK_END);
  long tsize = ftell(tfile);
  rewind(tfile);

  // create bytearray for target file
  ByteArray *tbytes = (ByteArray*)malloc(sizeof(ByteArray));
  if (tbytes == NULL) {
    printf("Error allocating memory for target file!");
    goto cleanup;
  }
  tbytes->size = tsize;
  tbytes->data = (unsigned char*)malloc(tsize);
  if (tbytes->data == NULL) {
    printf("Error allocating memory for target file bytes!");
    goto cleanup;
  }

  // read target file into bytearray
  if (fread(tbytes->data, 1, tsize, tfile) == 0) {
    printf("Error reading target file!");
    goto cleanup;
  }

  // close target file
  fclose(tfile);


  // Output File Setup (o) \\.
  // remove . exe from target file name and add -output.bin 
  char *oname = (char*)malloc(strlen(argv[2]) + 1);
  strcpy(oname, argv[2]);
  oname[strlen(argv[2]) - 4] = '\0';
  strcat(oname, "-output.bin");

  FILE *ofile = fopen(oname, "wb");
  size_t osize = 0;
  free(oname);

  // Linear Algorithm \\.
  size_t start = 0;
  size_t chunk = 1;
  size_t target_start = 0;
  do {
    size_t memres = memmem(pbytes->data, pbytes->size, tbytes->data + start, chunk);
    if (memres == 0) {
      if (chunk == 1) {
        printf("Single byte without a match :( [BAD PARENT FILE]");
        break;
      }
      else {
        if (chunk > MAX_CHUNK) {
          chunk = MAX_CHUNK;
        } 
        chunk -= 1;
        start += chunk;

        unsigned long offset = (unsigned long) target_start;
        unsigned short size = (unsigned short) chunk;
        fwrite(&offset, 4, 1, ofile);
        fwrite(&size, 2, 1, ofile);     
        
        printf("%d of %d\r", start, tbytes->size);

        chunk = 1;
        osize += 6;
      }
    }
    else {
      target_start = memres - 1;
      chunk++;
    }
  } while(start < tbytes->size);

  // Post Checks! \\.
  fseek(ofile, 0, SEEK_END);
  size_t check = ftell(ofile);
  if (osize != check) {
    printf("Error: Output size expected to be %d bytes but is %d bytes! [TRY AGAIN?]", osize, check);
    goto cleanup;
  }

  // Output! \\.
  printf("\n\nNew size: %d\n", osize);
  printf("Old size: %d\n", tsize);
  printf("Compression percentage: %f\n", 100 - ((float)osize / (float)tsize) * 100);

  // clean up 
  cleanup:
    free(tbytes->data);
    free(tbytes);
    free(pbytes->data);
    free(pbytes);
    if (ofile != NULL)
      fclose(ofile);
    goto cleanup;

  free(tbytes->data);
  free(tbytes);
  free(pbytes->data);
  free(pbytes);
  if (ofile != NULL)
    fclose(ofile);
  return 0;
}

