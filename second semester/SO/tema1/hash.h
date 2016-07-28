
#ifndef HASH_H_
#define HASH_H_

typedef unsigned int (*h)(const char *str, unsigned int hash_length);
unsigned int hash(const char *str, unsigned int hash_length);

#endif

