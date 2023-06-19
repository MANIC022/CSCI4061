#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

/* 
    Function: traverse_dir_core       --> "Recursively print out the information of non-directory files inside a directory"
    Input:    char *root_dir_path     --> "The path of the directory"
    Returns:  void
*/
void traverse_dir_core(char *root_dir_path) {
    // If root_dir_path is NULL, return
    if (root_dir_path == NULL) {
        return ;
    }
    
    // Open the root_dir
    DIR* root_dir = opendir(root_dir_path);

    // Traverse the root_dir
    struct dirent* entry;
    while ((entry = readdir(root_dir)) != NULL) {
        // Skip "." and ".."
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
            continue;
        }

        char child_path[strlen(root_dir_path) + strlen(entry->d_name) + 2];
        sprintf(child_path, "%s/%s", root_dir_path, entry->d_name);
        
        // If child is a directory, go into child directory
        if (entry->d_type == DT_DIR) {
            traverse_dir_core(child_path);
        } else {
            // Otherwise, print out the file's information
            struct stat buf;
            stat(child_path, &buf);
            printf("Name: %s\nHard Links: %ld\nSize: %ld\n\n",
                entry->d_name, buf.st_nlink, buf.st_size);
        }
    }

    // Close root_dir
    closedir(root_dir);
    return ;
}

// traverse_dir.c takes the path to a directory as its input and 
// prints out the information of all the non-directory files
// NOTE: please use test_directory to test your traverse_dir.c
// To compile: gcc -o traverse_dir traverse_dir.c
int main(int argc, char** argv) {

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <directory_path>\n", argv[0]);
        exit(1);
    }

    char* root_dir_path = argv[1];

    // Check input
    struct stat root_dir_sb;
    // On success, stat() returns 0
    if (stat(root_dir_path, &root_dir_sb) != 0) {
        fprintf(stderr, "stat() system call failed on %s\n", root_dir_path);
        exit(1);
    }
    // Check whether the input is a directory or not
    if (!(root_dir_sb.st_mode & __S_IFDIR)) {
        fprintf(stderr, "%s is not a directory\n", root_dir_path);
        exit(1);
    }

    // Core logic
    traverse_dir_core(root_dir_path);

    return 0;
}