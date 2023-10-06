#include "cstore_args.h"
#include "cstore_object.h"

using namespace std;
int list_files(std::string archive_name){

}

int add_files(string archive_name, vector<string> files, string password){

}

int extract_files(string archive_name, vector<string> files, string password){

}

int main(int argc, char* argv[])
{
    CStoreArgs args = CStoreArgs(argc,argv);

    string action = args.get_action();
    string archive_name = args.get_archive_name();
    vector<string> files = args.get_files();
    int retval = 0;
    if(action == "list"){
        retval = list_files(args.get_archive_name());
    }
    else if(action == "add"){
        retval = add_files(archive_name, args.get_files(), args.get_password());
    }
    else if(action == "extract"){
        retval = extract_files(archive_name, args.get_files(), args.get_password());
    }
    else{
        show_usage(argv[0]);
        error_and_quit("arg reading error, operation not supported");
    }
    
    return retval;
}