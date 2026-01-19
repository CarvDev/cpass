#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 512 * sizeof(unsigned char) // size of the buffer to be read from /dev/urandom

int is_char_valid(unsigned char character) { // verifies if a character is valid (not a control char, space or DEL)
    if (character <= 32 || character == 127) return 0;
    else return 1; 
}

FILE *open_file() { // opens the /dev/urandom file
    FILE *f = fopen("/dev/urandom", "rb");
    if (f == NULL) perror("Error opening /dev/urandom");
    return f;
}

void display_help(char *program_name) {
    printf("Usage: %s [LENGTH] [OPTIONS]\n\n", program_name);
    printf("Secure password generator (CSPRNG) using /dev/urandom.\n\n");
    printf("Arguments:\n");
    printf("  LENGTH       Number of characters for the password (Default: 16)\n\n");
    printf("Options:\n");
    printf("  -h, --help   Display this help message and exit\n\n");
    printf("Examples:\n");
    printf("  %s 20        -> Generates a 20-character password\n", program_name);
    printf("  %s           -> Generates a 16-character password (default)\n", program_name);
}

int main(int argc, char *argv []) {
    FILE *fptr;

    int length = 16; // if no arguments are passed, assumes 16 chars as default size
    
    // Argument Verification
    if (argc > 1) {
        // 1. Checks if user requested help (text flag)
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            display_help(argv[0]);
            return 0; // Exit with success
        }

        // 2. Tries to read the size (if not help, assumes it is a number)
        if (sscanf(argv[1], "%d", &length) != 1) {
            fprintf(stderr, "Error: Invalid argument '%s'.\n", argv[1]);
            fprintf(stderr, "Use '%s --help' to see options.\n", argv[0]);
            return 1;
        }

        // 3. Logical validation of the number
        if (length <= 0) {
            fprintf(stderr, "Error: Length must be greater than 0.\n");
            return 1;
        }

        // 4. Protection against extra arguments (e.g., ./main 20 extra)
        if (argc > 2) {
            fprintf(stderr, "Warning: Extra arguments ignored after '%s'.\n", argv[1]);
        }
    }

    unsigned char *password = (unsigned char *) malloc((length +1) * sizeof(unsigned char)); // allocates memory for the password string
    
    if (password == NULL) {
        perror("Malloc failed");
        return 1;
    }

    fptr = open_file();
    if (fptr == NULL) {
        free(password);
        return 1;
    }

    unsigned char buffer[BUFFER_SIZE]; // string to store buffer to be read 
    int i, buffer_pos, buffer_remaining; // control variables
    i = buffer_pos = buffer_remaining = 0;

    while (i < length) { // filling the password string
        if (buffer_remaining == 0) { // if remaining chars in buffer reach zero, it will be reloaded
            size_t bytes_read = fread(buffer, 1, BUFFER_SIZE, fptr); // verifies how many chars the buffer actually read
            if (bytes_read == 0) { // if no character is read, the program exits with critical failure
                fprintf(stderr, "Critical error: /dev/urandom failure\n");
                free(password);
                fclose(fptr);
                return 1;
            }

            buffer_remaining = bytes_read;
            buffer_pos = 0;
        }
        
        unsigned char c = toascii(buffer[buffer_pos]); // reads a character from buffer, converting to ascii
        if (is_char_valid(c)) { // verifies if char is valid, if so stores in password string
            password[i] = c;
            i++;
        }

        buffer_pos++;
        buffer_remaining--;
    }
    
    password[length] = '\0'; // adds \0 at the last position of the password array, to inform string end
    printf("%s\n", password);

    free(password);
    fclose(fptr); 
    return 0;
}