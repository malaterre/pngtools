#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>
#include <unistd.h>

void pnginfo_copyfile(char *, char *, int);
void pnginfo_error(char *);
void *pnginfo_xmalloc(size_t);
void usage(void);

#define pnginfo_true 1
#define pnginfo_false 0

int main(int argc, char *argv[]){
  int optchar, destwidth = -1;
  char *input, *output;

  // Use getopt to determine what we have been asked to do
  while((optchar = getopt(argc, argv, "i:o:w:")) != -1){
    switch(optchar){
    case 'i':
      // The name of the input file
      input = optarg;
      break;

    case 'o':
      // The name of the outputfile
      output = optarg;
      break;

    case 'w':
      destwidth = atoi(optarg);
      break;

    case '?':
    default:
      usage();
      break;
    }
  }

  // Determine if we were given a filename on the command line
  if(argc < 2)
    usage();

  pnginfo_copyfile(input, output, destwidth);
}

void pnginfo_copyfile(char *input, char *output, int destwidth){
  FILE *image;
  unsigned long imageBufSize, width, height;
  unsigned char signature;
  int bitdepth, colourtype;
  png_uint_32 i, j, rowbytes;
  png_structp png;
  png_infop info;
  unsigned char sig[8];
  png_bytepp row_pointers = NULL;
  char *bitmap;

  // Open the file
  if ((image = fopen (input, "rb")) == NULL)
    pnginfo_error ("Could not open the specified PNG file.");

  // Check that it really is a PNG file
  fread(sig, 1, 8, image);
  if(!png_check_sig(sig, 8)){
    printf("  This file is not a valid PNG file\n");
    fclose(image);
    return;
  }

  // Start decompressing
  if((png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, 
				   NULL, NULL)) == NULL)
    pnginfo_error("Could not create a PNG read structure (out of memory?)");

  if((info = png_create_info_struct(png)) == NULL)
    pnginfo_error("Could not create PNG info structure (out of memory?)");
  
  // If pnginfo_error did not exit, we would have to call 
  // png_destroy_read_struct

  if(setjmp(png_jmpbuf(png)))
    pnginfo_error("Could not set PNG jump value");

  // Get ready for IO and tell the API we have already read the image signature
  png_init_io(png, image);
  png_set_sig_bytes(png, 8);
  png_read_info(png, info);
  png_get_IHDR(png, info, &width, &height, &bitdepth, &colourtype, NULL, 
	       NULL, NULL);

  if (colourtype == PNG_COLOR_TYPE_PALETTE)
    png_set_expand (png);
  
  png_set_strip_alpha (png);
  png_read_update_info (png, info);
  
  rowbytes = png_get_rowbytes (png, info);
  bitmap =
    (unsigned char *) pnginfo_xmalloc ((rowbytes * height) + 1);
  row_pointers = pnginfo_xmalloc (height * sizeof (png_bytep));
  
  // Get the image bitmap
  for (i = 0; i < height; ++i)
    row_pointers[i] = bitmap + (i * rowbytes);
  png_read_image (png, row_pointers);
  free(row_pointers);
  png_read_end (png, NULL);
  
  // Do we want to display this bitmap?

  


















  
  // This cleans things up for us in the PNG library
  fclose(image);
  png_destroy_read_struct(&png, &info, NULL);
}

// You can bang or head or you can drown in a hole
//                                                    -- Vanessa Amarosi, Shine
void
pnginfo_error (char *message)
{
  fprintf (stderr, "%s\n", message);
  exit (42);
}

// Allocate some memory
void *
pnginfo_xmalloc (size_t size)
{
  void *buffer;

  if ((buffer = malloc (size)) == NULL)
    {
      pnginfo_error ("pnginfo_xmalloc failed to allocate memory");
    }

  return buffer;
}

void usage(){
  pnginfo_error("Usage: pngcp ---update this---");
}