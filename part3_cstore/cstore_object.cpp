#include "cstore_object.h"

void str_fill_buff(vector<char> *buf, string str){
    const char *c_str;
    for(c_str = str.c_str(); *c_str!='\0'; c_str++){
        buf->push_back(*c_str);
    }
}

bool file_exists(string filename){
    ifstream file(filename.c_str());
    return file.good();
}

streampos get_file_length(ifstream file){
    streampos file_size;

    if (!file.is_open()) {
        return -1;
    }
    file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    return file_size;
}

ifstream open_file(string filename, int *success ){
    ifstream cur_file;
    cur_file.open( filename, std::ios::in | std::ios::binary |std::ios::out );
  
    if (!cur_file.is_open()) {
        *success = -1;
        return cur_file;
    }

    *success = 0;
    return cur_file;
}

Mode CStoreObject::determine_mode(string act){
    if(act == "list"){
        return list;
    }
    else if(act == "add"){
        return add;
    }
    return extract;
}

CStoreObject::CStoreObject(CStoreArgs user_args)
{
    files = user_args.get_files();
    password = user_args.get_password();
    archive_name = user_args.get_archive_name();
    mode = determine_mode(user_args.get_action());

    err_msg = "";
    err = false;
}


bool CStoreObject::has_retval(){
    return mode == list;
}

vector<string> CStoreObject::get_retval(){
    return retval;
}

void CStoreObject::get_hash(char *buf){
    //TODO: Retrieves stored hash

}

bool CStoreObject::verify_hash(ifstream file){
    //TODO: verify hash with current hash of file
    return true;

}

void CStoreObject::read_metadata(ifstream *file, char *md){
    //TODO reads metadata

}

int CStoreObject::parse_metadata(char *md, int *num_files){
    //TODO parse metadata
    return 0;

}

void CStoreObject::make_file_list(ifstream *file, 
                                int num_blocks, int num_files){
    //TODO: make list and place in retval
    vector<string> buff;

}

void CStoreObject::list_files(){
    //TODO: Find way to list files (use archive_name as key)
    ifstream file;
    char metadata[AES_BLOCK_SIZE];
    int succ, num_files, num_blocks;
    if(!file_exists(archive_name)){
        err = true;
        err_msg = "error: archive does not exist";
    }
    file = open_file(archive_name, &succ);
    if(succ){
        err = true;
        err_msg = "i/o error: couldn't open file";
    }

    read_metadata(&file, metadata);
    num_blocks = parse_metadata(metadata, &num_files);
    make_file_list(&file, num_blocks, num_files);
}

void CStoreObject::pad_buffer(vector<char> *buff, int len){
    for(int i=0; i<len; i++){
        buff->push_back('\0');
    }
}

void CStoreObject::add_file_blocks(vector<char> *buff, int totalbytes){
    vector<string>::iterator iter;
    for(iter = files.begin(); iter < files.end(); iter++){
        string curr = *iter;
        str_fill_buff(buff, curr);
        buff->push_back('%');
    }
    int mustpad = AES_BLOCK_SIZE - totalbytes%AES_BLOCK_SIZE;
    pad_buffer(buff, mustpad);
}

void CStoreObject::make_metadata(vector<char> *buff){
    int file_num = files.size();
    int total_bytes = 0;
    int num_blocks, pad_len;
    string nb_str, fn_str;
    vector<string>::iterator iter;
    for(iter = files.begin(); iter < files.end(); iter++){
        string curr = *iter;
        int strlen = curr.size();
        total_bytes += strlen+1;
    }
    num_blocks = (total_bytes+AES_BLOCK_SIZE)/AES_BLOCK_SIZE;
    nb_str = to_string(num_blocks);
    fn_str = to_string(file_num);
    str_fill_buff(buff, fn_str);
    buff->push_back('%'); //separator
    str_fill_buff(buff, nb_str);

    pad_len = AES_BLOCK_SIZE - nb_str.size() - 1 - fn_str.size();
    if(pad_len < 0){
        err = true;
        err_msg = "exceeded metadata length";
    }
    pad_buffer(buff, pad_len);
    add_file_blocks(buff, total_bytes);
}

void CStoreObject::make_MAC(vector<char> *buff){
    //TODO: make hash out of complete buff (integrity)

}

void fill_buff_files(vector<char> *buff){
    //TODO: append buff files to vector

}

void CStoreObject::my_encrypt_metadata(vector<char> *buf){
    string temp_name = archive_name + "_temp.txt";
    if(!err)
        write_data_to_file(temp_name, *buf);

    //For this "public" information (ie. returned from list)
    //just encrypt using archive_name as key
    auto encrypted = encrypt_file(temp_name, archive_name);
    buf->clear();
    buf->resize(AES_BLOCK_SIZE + encrypted.ciphertext.size());
    memcpy((char *) buf->data(), encrypted.IV, AES_BLOCK_SIZE);
    memcpy((char *) buf->data()+AES_BLOCK_SIZE, 
        encrypted.ciphertext.data(), encrypted.ciphertext.size());
    
    if(remove(temp_name.c_str())){
        err = true;
        err_msg = "error: could not remove temp file";
    }

}

void CStoreObject::add_files(){
    vector<char> buff;
    if(file_exists(archive_name)){
        err = true;
        err_msg = "error: archive already exists, not supported";
    }
    make_metadata(&buff);
    my_encrypt_metadata(&buff);
    fill_buff_files(&buff); //adds files and encrypts
    make_MAC(&buff); //adds a MAC
    
    if(!err)
        write_data_to_file(archive_name, buff);
}

void CStoreObject::extract_files(){
    //TODO: Unencrypt archive's files

}

void CStoreObject::do_operation(){
    switch(mode)
    {
        case list : list_files();
        case add : add_files();
        case extract : extract_files();
    }
}

bool CStoreObject::has_err(){
    return err;
}

string CStoreObject::get_err(){
    return err_msg;
}