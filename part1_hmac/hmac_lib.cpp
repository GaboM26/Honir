#include "hmac_lib.h"


/**
 * @brief generates a sha256 hash of some input
 *  note that this version handles all of the input
 *  at once. for large files you may want to chunk
 * 
 * @param input a byte array of data
 * @param output a byte array to store the hash; should be 32 bytes
 * @param in_len the size of the input data
 */
void hash_sha256(const BYTE * input, BYTE * output, int in_len)
{
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, input, in_len);
    sha256_final(&ctx, output);
}

/**
 * @brief returns a buffer of a hexidecimal representation of a string
 * 
 * @param byte_arr - byte array to print
 * @param len - length of byte array
 */
char *sprint_hex(const char* byte_arr, uint32_t len)
{
    uint64_t buff_len = len*2+1;
    char * buffer = (char *) malloc(buff_len);

    if(buffer == NULL)
        return buffer;

    memset(buffer, 0, buff_len);

    char *buffer_ptr = buffer;

    for(uint32_t index = 0; index < len; index++) {
        sprintf(buffer_ptr, "%02X", (unsigned char) byte_arr[index]);
        buffer_ptr += 2;
    }
    return buffer;
}

/**
 * @brief print a byte string as its hexidecimal representation
 * 
 * @param byte_arr - byte array to print
 * @param len - length of byte array
 */
void print_hex(const char* byte_arr, int len)
{
    char * buff = sprint_hex(byte_arr,len);
    if (buff != NULL) {
        printf("%s\n", buff);
    }
    free(buff);
}

/**
 * @brief print a byte vector as its hexidecimal representation
 *  provided as a brief demonstration for how to interface
 *  between vectors and C arrays
 * 
 * @param bytes a vector of bytes
 */
void print_vector_as_hex(std::vector<char> bytes)
{
        print_hex(bytes.data(), bytes.size());
}

/**
 * @brief writes a binary file to disk
 * 
 * @param filename name of file to write
 * @param data vector of data to write
 */
void write_data_to_file(std::string filename, std::vector<char> data)
{
        std::ofstream outfile;
        outfile.open(filename,std::ios::binary|std::ios::out);
        outfile.write(data.data(),data.size());
        outfile.close();
}

char *prep_str(const char *password, unsigned int *password_length){
    // TODO implement padding
    if(*password_length <= SHA256_DATA_SIZE)
        return (char *) password;
    
    char *comp_pass = new char[SHA256_BLOCK_SIZE];

    hash_sha256((BYTE *) password, (BYTE *) comp_pass, *password_length);

    *password_length = SHA256_BLOCK_SIZE;
    return comp_pass;
}

/**
 * @brief Reads a file and generate a hmac of its contents, given a password
 * 
 * @param filename - name of file to generate hmac
 * @param password - password to use when generating secret
 * @param dest - buffer to store the final hash; should be size of a sha256
 * @return true - successfully completed actions
 * @return false - an error occurred 
 */
bool generate_hmac(const char * filename, const char * password, 
        unsigned int password_length, char * dest, bool hash_from_cli)
{
    // TODO: rewrite this function to be correct

    std::vector<BYTE> hmac_data;
    std::ifstream cur_file;
    std::streampos file_size;
    std::streampos file_pos = 0;
    bool success = true;
    
    SHA256_CTX ctx;
    unsigned char ipad[SHA256_DATA_SIZE+1];
    unsigned char opad[SHA256_DATA_SIZE+1];
    unsigned int plen_actual = password_length;

    char *key = prep_str(password, &plen_actual);

    bzero(ipad, sizeof(ipad));
    bzero(opad, sizeof(opad));
    bcopy(key, ipad, password_length);
    bcopy(key, opad, password_length);

    for(int i=0; i<SHA256_DATA_SIZE; i++){
        ipad[i] ^= 0x36;
        opad[i] ^= 0x5c;
    }

    /* check if we are hashing from cli to skip file reading altogether*/
    if(hash_from_cli){
        unsigned int content_size = strlen(filename);

        if(content_size > SHA256_DATA_SIZE){
            std::cerr << "Error: hash_from_cli does not support long cli input. Consider using file" << std::endl;
            success = false;
            goto exit;
        }
        char *content = (char *) filename;
        sha256_init(&ctx);
        sha256_update(&ctx,(BYTE *) ipad, SHA256_DATA_SIZE);
        sha256_update(&ctx, (BYTE *) content, content_size);
        sha256_final(&ctx, (BYTE *) dest);
        goto hash;
    }

    // a brief example of file IO in C++
    // you don't need to use it if you prefer C
    cur_file.open( filename, std::ios::in | std::ios::binary |std::ios::ate );
  
    if (!cur_file.is_open()) {
        success = false;
        goto exit;
    }
    //https://cplusplus.com/reference/istream/istream/read/
        
    file_size = cur_file.tellg();
    cur_file.seekg(0, std::ios::beg);
    char chunk[SHA256_SIZE_IN_BYTES];
    sha256_init(&ctx);
    sha256_update(&ctx,(BYTE *) ipad, SHA256_DATA_SIZE);
    while (file_pos < file_size && cur_file) {
        cur_file.read(chunk, SHA256_SIZE_IN_BYTES);
        sha256_update(&ctx, (BYTE *)chunk, cur_file.gcount());
        file_pos += cur_file.gcount();
    }
    sha256_final(&ctx, (BYTE *) dest);

hash:
    sha256_init(&ctx);
    sha256_update(&ctx, opad, SHA256_DATA_SIZE);
    sha256_update(&ctx, (BYTE *) dest, SHA256_BLOCK_SIZE);
    sha256_final(&ctx, (BYTE *) dest);

exit:
    if(plen_actual != password_length){
        delete[] key;
    }

    return success;
}
