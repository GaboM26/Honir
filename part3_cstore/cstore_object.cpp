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

bool file_is_empty(string filename){
    ifstream file(filename.c_str());
    return file.peek() == std::ifstream::traits_type::eof();
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
    cur_file.open( filename, std::ios::in | std::ios::binary);
  
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
    return files_in_archive;
}

void CStoreObject::get_hash(char *buf){
    //TODO: Retrieves stored hash

}

bool CStoreObject::verify_hash(ifstream file){
    //TODO: verify hash with current hash of file
    return true;

}

void CStoreObject::wrongfully_detected_end(vector<char> *md, int fix){
    for(int i=0; i<fix; i++){
        md->push_back('\0');
    }
}

vector<char> CStoreObject::read_metadata(vector<char> *data){
    //TODO parse metadata
    vector<char> metadata;
    vector<char>::iterator iter = data->begin();
    int count_null = 0;
    while(count_null != AES_BLOCK_SIZE){
        if(*iter == '\0'){
            count_null++;
        }
        else{
            if(count_null!=0){
                wrongfully_detected_end(&metadata, count_null);
                count_null = 0;
            }
            metadata.push_back(*iter);
        }
        iter++;
    }
    metadata = my_decrypt_metadata(metadata);
    data->erase(data->begin(), iter);//remain only with data 
    return metadata;

}

int CStoreObject::parse_metadata(vector<char> md){
    //TODO: make list and place in retval
    char buff[20]; //file_character names > 20
    char *filler = buff;
    vector<char>::iterator iter;
    for(iter=md.begin(); iter<md.end(); iter++){
        //retrieving numfiles
        if(*iter == '%'){
            *filler = '\0';
            break ;
        }
        *filler++ = *iter;
    }
    iter++;
    string temp(buff);
    if(temp.size()>16){
        err =true;
        err_msg="error: couldnt recover metadata, data corrupted";
    }
    int numfiles = stoi(temp);
    filler = buff;
    int i;
    for(i=0; i<numfiles && iter!=md.end(); iter++){
        //retrieving actual file names
        if(*iter == '%'){
            *filler = '\0';
            string str(buff);
            files_in_archive.push_back(str);
            filler = buff;
            i++;
        }
        else{
            *filler++ = *iter;
        }
    }
    if(i != numfiles){
        err = true;
        err_msg = "error: Data files not found, possible tampering";
    }
    return numfiles;
}

void CStoreObject::list_files(){
    if(!file_exists(archive_name)){
        err = true;
        err_msg = "error: archive does not exist";
    }

    vector<char> files_data = get_data_from_file(archive_name);
    vector<char> metadata = read_metadata(&files_data);
    parse_metadata(metadata);
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
}

void CStoreObject::make_metadata(vector<char> *buff){
    int file_num = files.size();
    int total_bytes = 0;
    string fn_str;
    vector<string>::iterator iter;
    for(iter = files.begin(); iter < files.end(); iter++){
        string curr = *iter;
        int strlen = curr.size();
        total_bytes += strlen+1;
    }
    fn_str = to_string(file_num);
    str_fill_buff(buff, fn_str);
    buff->push_back('%'); //separator

    add_file_blocks(buff, total_bytes);
}

void CStoreObject::make_MAC(vector<char> *buff){
    //TODO: make hash out of complete buff (integrity)

}

void CStoreObject::push_e_to_buf(vector<char> *buf, encrypted_blob e){
    for(uint64_t i=0; i<AES_BLOCK_SIZE; i++){
        buf->push_back(e.IV[i]);
    }

    for(uint64_t i=0; i<e.ciphertext.size(); i++){
        buf->push_back(e.ciphertext[i]);
    }
}

void CStoreObject::fill_buff_files(vector<char> *buf){
    //TODO: append buff files to vector
    vector<string>::iterator iter;
    for(iter = files.begin(); iter<files.end(); iter++){
        if(!file_exists(*iter) || file_is_empty(*iter)){
            err = true;
            err_msg = "error: file " + *iter + "does not exist or is empty";
        }
        auto encrypted = encrypt_file(*iter, password);
        push_e_to_buf(buf, encrypted);
        pad_buffer(buf, AES_BLOCK_SIZE);
    }

}

vector<char> CStoreObject::my_decrypt_metadata(vector<char> buf){
    string temp_name = archive_name + "_temp.txt";
    if(!err)
        write_data_to_file(temp_name, buf);

    std::vector<char> decrypted;
    decrypted = decrypt_file(temp_name, archive_name);
    if(remove(temp_name.c_str())){
        err = true;
        err_msg = "error: could not remove temp file";
    }
    return decrypted;
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
        err_msg = "error: encrypt could not remove temp file";
    }

}

void CStoreObject::add_files(){
    vector<char> buff;
    if(file_exists(archive_name)){
        err = true;
        err_msg = "error: archive already exists, not supported";
        return ;
    }
    make_metadata(&buff);
    my_encrypt_metadata(&buff);
    pad_buffer(&buff, AES_BLOCK_SIZE);
    
    fill_buff_files(&buff); //adds files and encrypts
    make_MAC(&buff); //adds a MAC
    
    if(!err)
        write_data_to_file(archive_name, buff);
}

void CStoreObject::get_index_array(int *ind){
    memset(ind, -1, files.size());
    for(uint64_t i=0; i<files.size(); i++){
        for(uint64_t j=0; j<files_in_archive.size(); j++){
            if(files[i] == files_in_archive[j])
                ind[i] = j;
        }
        if(ind[i] == -1){
            err = true;
            err_msg = "error: file(s) not found in archive";
            return ;
        }
    }
    sort(ind, ind+files.size());
}

void CStoreObject::extract_files(){
    //TODO: Unencrypt archive's files
    if(!file_exists(archive_name)){
        err = true;
        err_msg = "error: archive doesn't exist";
        return ;
    }
    vector<char> files_data = get_data_from_file(archive_name);
    vector<char> metadata = read_metadata(&files_data);
    uint64_t numfiles = parse_metadata(metadata);

    if(numfiles < files.size()){
        err = true;
        err_msg = "error: file(s) not in archive";
        return ;
    }
    int indices[files.size()];
    get_index_array(indices);

}

void CStoreObject::do_operation(){
    switch(mode)
    {
        case list : list_files(); break;
        case add : add_files(); break;
        case extract : extract_files(); break;
    }
}

bool CStoreObject::has_err(){
    return err;
}

string CStoreObject::get_err(){
    return err_msg;
}