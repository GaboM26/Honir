#include "cstore_args.h"
#include "cstore_object.h"

void print_files(vector<string> files){
    vector<string>::iterator iter;
    for(iter = files.begin(); iter < files.end(); iter++){
        cout << *iter << "\n" << endl;
    }
}

int main(int argc, char* argv[])
{
    CStoreArgs args = CStoreArgs(argc,argv);

    CStoreObject cstore = CStoreObject(args);

    cstore.do_operation();

    if(cstore.has_err()){
        error_and_quit(cstore.get_err());
        return -1;
    }

    if(cstore.has_retval()){
        print_files(cstore.get_retval());
    }
    
    return 0;
}