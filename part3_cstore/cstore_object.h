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
        void make_password_hash(vector<char> *);
        bool is_right_password(vector<char> *fd);
        bool is_archive(vector<char> *);
        bool verify_hmac(vector<char> *fd);

        void list_files();
        void add_files();
        void extract_files();

        void make_metadata(vector<char> *buf);
        int parse_metadata(vector<char> md);

        void pad_buffer(vector<char> *buf, int len);
        void wrongfully_detected_end(vector<char> *buf, int);
        void push_e_to_buf(vector<char> *buf, encrypted_blob e);

        void my_encrypt_metadata(vector<char> *buf);
        vector<char> my_decrypt(vector<char> buf, string pass);

        void add_file_blocks(vector<char> *buf, int);
        void fill_buff_files(vector<char> *buff);

        void get_index_array(int *);
        void parse_and_extract(vector<char> , int *, int);
        vector<char> get_file_from_buff(vector<char> *data, string password);

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
