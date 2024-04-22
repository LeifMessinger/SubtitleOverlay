#include <stdio.h>
#include <dirent.h>	//There doesn't seem to be a header guard on this.
#include <stdlib.h>
#include <string.h>

char **listFiles(const char *dirPath, int *numFiles) {
    DIR *dir;
    struct dirent *entry;
    char **filePaths = NULL;
    int capacity = 0;
    int count = 0;
    int dirPathLen = strlen(dirPath);

    // Open the directory
    dir = opendir(dirPath);
    if (dir == NULL) {
        printf("Error opening directory %s\n", dirPath);
        *numFiles = 0;
        return NULL;
    }

    // Read and store each entry in the directory
    while ((entry = readdir(dir)) != NULL) {
        // Ignore "." and ".." entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Reallocate the filePaths array if needed
        if (count >= capacity) {
            capacity = (capacity + 1) * 2;
            filePaths = (char **)realloc(filePaths, capacity * sizeof(char *));
        }

        // Allocate memory for the file path string
        int filePathLen = dirPathLen + strlen(entry->d_name) + 2; // +2 for "/" and null terminator
        filePaths[count] = (char *)malloc(filePathLen * sizeof(char));

        // Concatenate the directory path and file name
        strcpy(filePaths[count], dirPath);
        strcat(filePaths[count], "/");
        strcat(filePaths[count], entry->d_name);

        count++;
    }

    // Close the directory
    closedir(dir);

    // Set the number of files found
    *numFiles = count;

    // Return the array of file paths
    return filePaths;
}

void freeFileList(char **fileNames, int numFiles) {
    for (int i = 0; i < numFiles; i++) {
        free(fileNames[i]);
    }
    free(fileNames);
}