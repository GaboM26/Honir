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
        void read_metadata(ifstream *, char *);
        void make_metadata(vector<char> *buf);
        int parse_metadata(char *md, int *num_files);
        void make_file_list(ifstream *file, int num_blocks, int num_files);
        void pad_buffer(vector<char> *buf, int len);
        Mode determine_mode(string);
        void make_hash(vector<char> *buf);

    public:
        
        CStoreObject(CStoreArgs user_args);
        bool has_err();
        string get_err();
        void do_operation();
        bool has_retval();
        vector<string> get_retval();
        string get_mode();

};


#endif
