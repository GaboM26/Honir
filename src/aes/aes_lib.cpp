#include "aes_lib.h"

void do_cbc_xor(char *dest, const char *prev){
    for(int i=0; i<AES_BLOCK_SIZE; i++){
        dest[i] ^= prev[i];
    }
}
void pad_plaintext(char *plaintext, uint64_t *len, uint64_t padding_length)
{
    if(padding_length == 0)
        return ;
    
    char *pt = plaintext;
    pt += *len;
    for(uint64_t i=0; i<padding_length; i++){
        pt[i] = '\0';
    }
    *len += padding_length;
}

void do_cbc_encrypt(char *ciphertext, const char *plaintext,
    uint64_t plaintext_length, WORD *keysched, const char *IV, 
    uint64_t ciphertext_length, uint64_t padding_length)
{
    char pad_pt[ciphertext_length];
    memcpy(pad_pt, plaintext, plaintext_length);
    pad_plaintext(pad_pt, &plaintext_length, padding_length);

    const char *pt = pad_pt;
    const char *prev = IV;
    char *ct_move = ciphertext;
    char buff[AES_BLOCK_SIZE];
    for(uint64_t i=0; i<(plaintext_length/AES_BLOCK_SIZE); i++){
        pt = pad_pt + i*AES_BLOCK_SIZE;
        memcpy(buff, pt, AES_BLOCK_SIZE);
        do_cbc_xor(buff, prev);
        aes_encrypt((BYTE *) buff, (BYTE *) ct_move, keysched, SHA256_BLOCK_SIZE*8);
        prev = ct_move;
        ct_move += AES_BLOCK_SIZE;
    }


}
/**
 * @brief encrypts plaintext using AES CBC using key. 
 * buffer allocated in ciphertext must be freed. 
 * Note the current prototype allocates a buffer locally;
 * 
 * @param plaintext - pointer to plaintext buffer; if this ends in a null byte, add
 * a non-null character to the end and then strip it during decryption (or just always
 * do this for deterministic behavior)
 * @param plaintext_length - length in bytes of plaintext
 * @param IV - IV to use 
 * @param ciphertext - should pass a pointer to a pointer. This pointer 
 * will be updated with a heap-based pointer to the final ciphertext. Must be freed by caller.
 * @param ciphertext_length - pointer to int. updated with final length after padding
 * @param key - pointer to key. size is AES_BLOCK_SIZE
 * @return int - returns -1 on error, 0 otherwise.
 */
int encrypt_cbc(const char * plaintext, uint64_t plaintext_length, 
    const char * IV, char ** ciphertext,
    uint64_t * ciphertext_length, char* key )
{
    WORD key_sched[60];
    aes_key_setup((BYTE *) key, key_sched, SHA256_BLOCK_SIZE*8);

    *ciphertext_length = plaintext_length + 1;
    uint64_t padding_length = 0;
    if (*ciphertext_length % AES_BLOCK_SIZE != 0) {
        *ciphertext_length = (plaintext_length/AES_BLOCK_SIZE)*AES_BLOCK_SIZE + AES_BLOCK_SIZE;
        padding_length = *ciphertext_length - plaintext_length;
    }
    *ciphertext = (char *) malloc(*ciphertext_length);
    if(*ciphertext == NULL) {
        perror("error");
        return -1;
    }
    memset(*ciphertext, 0, *ciphertext_length);
    
    do_cbc_encrypt(*ciphertext, plaintext, plaintext_length,
        key_sched, IV, *ciphertext_length, padding_length); // encryption should take place here

    return 0;
}

/**
 * @brief read file from disk
 * note that this may fail on very big files
 * 
 * @param filename - file to be read
 * @return std::vector<char> - on fail, this will be empty
 */
std::vector<char> get_data_from_file(std::string filename)
{
    std::ifstream cur_file;
    std::streampos file_size;
    cur_file.open( filename, std::ios::in | std::ios::binary |std::ios::ate );
    std::vector<char> file_contents;
    if (cur_file.is_open())
    {
        file_size = cur_file.tellg();
        file_contents.resize(file_size);
        cur_file.seekg (0, std::ios::beg);
        cur_file.read( (char *) file_contents.data(), file_size);
        cur_file.close();
    }
        
    return file_contents;
}

void do_cbc_decrypt(const char *ciphertext, char *plaintext,
    WORD *keysched, const char *IV, uint64_t ciphertext_length)
{
    char buff[ciphertext_length];
    char *end_ct = (char *) ciphertext + ciphertext_length - AES_BLOCK_SIZE;
    char *end_buf = buff + ciphertext_length - AES_BLOCK_SIZE;
    char *last_block;

    aes_decrypt((BYTE *) end_ct, (BYTE *) end_buf, keysched, 
        SHA256_BLOCK_SIZE*8);

    for(uint64_t i=0; i<(ciphertext_length/AES_BLOCK_SIZE)-1; i++){
        last_block = end_buf;
        end_buf -= AES_BLOCK_SIZE;
        end_ct -= AES_BLOCK_SIZE;
        do_cbc_xor(last_block, end_ct);
        aes_decrypt((BYTE *) end_ct, (BYTE *) end_buf, keysched, 
        SHA256_BLOCK_SIZE*8);

    }
    do_cbc_xor(buff, IV);

    memcpy(plaintext, buff, sizeof(buff));
}
/**
 * @brief decrypts ciphertext using key and IV stores result in buffer and updates 
 * plaintext with pointer to buffer. Note that decrypt will remove null byte padding. 
 * assumes plaintext does not end in null bytes. If your plaintext will, add a non-null
 * character before encrypting and then strip it after.
 * 
 * @param ciphertext - pointer to buffer with ciphertext
 * @param ciphertext_length - length of ciphertext. should be a multiple of AES_BLOCKSIZE
 * @param IV - IV used to decrypt first block.
 * @param plaintext - double pointer to plaintext buffer. This should be a pointer to a pointer. Once the ciphertext has been decrypted, the first-depth pointer will be updated with a new heap-based buffer containing plaintext
 * @param plaintext_length length of plaintext. Should be ciphertext_length - padding. 
 * @param key - key used for AES 
 * @return int - 0 on success, -1 on error.
 */
int decrypt_cbc(const char* ciphertext, uint64_t ciphertext_length, 
    const char * IV, char ** plaintext, uint64_t * plaintext_length, 
    char* key)
{
    WORD key_sched[60];
    aes_key_setup((BYTE *) key, key_sched, SHA256_BLOCK_SIZE*8);
    *plaintext_length = ciphertext_length;
    *plaintext = (char *)  malloc(*plaintext_length);
    if(*plaintext == NULL) {
        perror("error");
        return -1;
    }

    do_cbc_decrypt(ciphertext, *plaintext, key_sched, IV, ciphertext_length);

    // remove null byte padding
    char * plaintext_ptr = *plaintext;
    for(uint64_t index = ciphertext_length - 1; 
        index > (ciphertext_length - AES_BLOCK_SIZE); index--) {
            if(plaintext_ptr[index] != '\0') {
                break;
            }

            *plaintext_length = index;
    }
    return 0;
}

void init_iv(char *IV, unsigned int iv_len, int *success){
    std::ifstream cur_file;
    cur_file.open( RANDOM_LOC, std::ios::in | std::ios::binary |std::ios::ate );
    if (!cur_file.is_open()) {
        *success = 0;
        return ; 
    }
    cur_file.read(IV, AES_BLOCK_SIZE);

    *success = 1;

    if(cur_file.gcount() != AES_BLOCK_SIZE)
        *success = 0;
    
}

void gen_key(std::string pass_str, char *key){
    char password[pass_str.size()+1];
    strcpy(password, pass_str.c_str());
    hash_sha256((BYTE *)password, (BYTE *) key, pass_str.size());
    for(int i=0; i<PREP_KEY_NUMHASH-1; i++){
        hash_sha256((BYTE *)key, (BYTE *) key, SHA256_BLOCK_SIZE);
    }
}

encrypted_blob encrypt_file(std::string user_input, std::string password)
{
    char IV[AES_BLOCK_SIZE];
    encrypted_blob return_value;
    int succ;
    init_iv(IV, AES_BLOCK_SIZE, &succ);

    if(!succ)
        return return_value;
    
    char key[SHA256_BLOCK_SIZE];
    gen_key(password, key); 

    std::vector<char> plaintext = get_data_from_file(user_input);

    char * ciphertext = NULL;
    uint64_t ciphertext_length = 0;

    plaintext.push_back(PAD_CHAR); // ensure padding doesn't consume a null byte in plaintext

    int encrypt_success = encrypt_cbc(plaintext.data(), plaintext.size(), IV, &ciphertext, 
    &ciphertext_length, key);

    std::vector<char> return_vector;
    if(encrypt_success == 0) {
        return_vector.resize(ciphertext_length);
        memcpy(return_vector.data(), ciphertext, ciphertext_length);
    }
    free(ciphertext);

    return_value.ciphertext = return_vector;
    memcpy(return_value.IV, IV, AES_BLOCK_SIZE);

    return return_value;
}

std::vector<char> decrypt_file(std::string user_input, std::string password)
{
    std::vector<char> return_vector;
    char IV[AES_BLOCK_SIZE];

    char key[SHA256_BLOCK_SIZE];
    gen_key(password, key);

    std::vector<char> ciphertext = get_data_from_file(user_input);

    if (ciphertext.size() > 16) {
        memcpy(IV,ciphertext.data(),AES_BLOCK_SIZE);
        char * plaintext = NULL;
        uint64_t plaintext_length = 0;

        char * ciphertext_data = ciphertext.data()+16;
        uint64_t ciphertext_size = ciphertext.size()-16;

        int decrypt_success = decrypt_cbc(ciphertext_data, ciphertext_size, IV, 
            &plaintext, &plaintext_length, key);

        if(decrypt_success == 0) {
            return_vector.resize(plaintext_length -1); // consume padding char
            memcpy(return_vector.data(), plaintext, plaintext_length - 1);
        }
        free(plaintext);
    }
        
    return return_vector;
}
