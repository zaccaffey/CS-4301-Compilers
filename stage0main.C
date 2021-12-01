#include <stage2.h>
int main(int argc, char **argv)
{
 // This program is the stage1 compiler for Pascallite. It will accept
 // input from argv[1], generate a listing to argv[2], and write object
 // code to argv[3].
 if (argc != 4) // Check to see if pgm was invoked correctly
 {
 // No; print error msg and terminate program
 cerr << "Usage: " << argv[0] << " SourceFileName ListingFileName "
 << "ObjectFileName" << endl;
 exit(EXIT_FAILURE);
 }
 Compiler myCompiler(argv);
 myCompiler.createListingHeader();
 myCompiler.parser();
 myCompiler.createListingTrailer();
 return 0;
}
