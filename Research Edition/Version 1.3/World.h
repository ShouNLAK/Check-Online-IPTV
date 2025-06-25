#ifndef WORLD_H
#define WORLD_H

typedef struct {
    char code[3];  // 2 letters + null terminator
    char name[100];
} Country;

extern Country worldCountries[];
extern const size_t worldCountryCount; // This exposes the number of elements

#endif