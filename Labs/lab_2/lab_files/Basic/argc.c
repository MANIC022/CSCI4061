#include <stdio.h>

int main( int argc, char *argv[] ) {

        if ( argc == 1 ) {
                printf( "No arguments were passed.\n" );
        } else {
                printf( "Arguments:\n" );

                for ( int i = 1; i < argc; ++i ) {
                        printf( "  %d. %s\n", i, argv[i] );
                }
        }

        return 0;
}
