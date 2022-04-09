#ifndef SYSOPY_LIBRARY_H
#define SYSOPY_LIBRARY_H

void create_array(unsigned int size_);

int wc_files(char* files);

char* allocate_block(void);

void remove_block(int index);

void remove_array(void);

#endif