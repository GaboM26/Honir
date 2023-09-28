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


class CStoreObject
{
    private:        
    public: 
        
        CStoreObject(CStoreArgs user_args);
        ~CStoreObject();
};


#endif
