#ifndef AES_ARGS_H
#define AES_ARGS_H

#include <argp.h>
#include <vector>
#include <iostream>
#include <string>
#include <unistd.h> // reference: https://www.gnu.org/software/libc/manual/html_node/getpass.html


#define MODE_ENCRYPT 0
#define MODE_DECRYPT 1

typedef struct {
    std::string password;
    std::string plaintext_file;
    std::string encrypted_file;
    unsigned short mode;
    bool hash_from_cli;
} arguments;

class AESArgs 
{
    private:
        arguments args;
        

    public: 
        std::string get_password();
        std::string get_plaintext_file();
        std::string get_encrypted_file();
        unsigned short get_mode();
        bool get_hash_from_cli();
        AESArgs(int argc, char ** argv, unsigned short mode);
};


#endif