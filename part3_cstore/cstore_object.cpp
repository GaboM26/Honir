#include "cstore_object.h"

int get_file_length(){
    return 0;
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

void CStoreObject::list_files(){

}

void CStoreObject::add_files(){

}

void CStoreObject::extract_files(){

}

void CStoreObject::do_operation(){
    switch(mode)
    {
        case list : list_files();
        case add : add_files();
        case extract : extract_files();
    }
}