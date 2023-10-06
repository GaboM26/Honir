#include "cstore_args.h"
#include "cstore_object.h"

using namespace std;

int main(int argc, char* argv[])
{
    CStoreArgs args = CStoreArgs(argc,argv);

    CStoreObject cstore = CStoreObject(args);

    cstore.do_operation();
    
    return 0;
}