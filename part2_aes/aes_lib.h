#ifndef AES_LIB_H
#define AES_LIB_H

#include "../crypto_lib/sha256.h"
#include "../crypto_lib/aes.h"
#include "../part1_hmac/hmac_lib.h"
#include <stdio.h>
#include <vector>
#include <string.h>
#include <iostream>
#include <fstream>

#define PAD_CHAR 0xff

typedef struct {
        std::vector<char> ciphertext;
        char IV[AES_BLOCK_SIZE];
} encrypted_blob;

std::vector<char> get_data_from_file(std::string filename);
encrypted_blob encrypt_file(std::string filename, 
        std::string password);
std::vector<char> decrypt_file(std::string filename, 
        std::string password);

// My modifications
void init_iv(char *IV, unsigned int iv_len);
void gen_key(std::string pass_str, char *key);

#define RANDOM_LOC "/dev/urandom"
#define PREP_KEY_NUMHASH 10000

#endif
