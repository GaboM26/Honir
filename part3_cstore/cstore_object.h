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
#include <algorithm>


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
        vector<string> files_in_archive;
        bool err;

        void make_MAC(vector<char> *buf);
        void get_hash(char *buf);
        bool verify_hash(ifstream file);

        void list_files();
        void add_files();
        void extract_files();

        void make_metadata(vector<char> *buf);
        vector<char> read_metadata(vector<char> *md);
        int parse_metadata(vector<char> md);

        void pad_buffer(vector<char> *buf, int len);
        void wrongfully_detected_end(vector<char> *buf, int);
        void push_e_to_buf(vector<char> *buf, encrypted_blob e);

        void my_encrypt_metadata(vector<char> *buf);
        vector<char> my_decrypt_metadata(vector<char> buf);

        void add_file_blocks(vector<char> *buf, int);
        void fill_buff_files(vector<char> *buff);

        void get_index_array(int *);

        Mode determine_mode(string);

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
