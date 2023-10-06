#ifndef CSTORE_OBJECT_H
#define CSTORE_OBJECT_H

#include "../crypto_lib/sha256.h"
#include "../crypto_lib/aes.h"
#include "../part1_hmac/hmac_lib.h"
#include "../part2_aes/aes_lib.h"
#include "cstore_args.h"
#include <stdio.h>
#include <vector>
#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;

enum Mode { list, add, extract};

class CStoreObject
{
    private:        
        Mode mode;
        std::vector<std::string> files;
        std::string password; 
        std::string archive_name;
        string err_msg;
        vector<string> retval;
        bool err;

        void get_hash(char *buf);
        bool verify_hash(ifstream file);
        void list_files();
        void add_files();
        void extract_files();
        Mode determine_mode(string);

    public:
        
        CStoreObject(CStoreArgs user_args);
        bool has_err();
        void do_operation();
        string get_mode();

};


#endif
