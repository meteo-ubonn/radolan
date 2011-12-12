#include <radolan/radolan.h>
#include <radolan/radolan_utils.h>
#include <iostream>
#include <string>

using namespace Radolan;
using namespace std;

void usage()
{
    cout << "radolan2shapefile [-g -p] -f <radolan scan path> -o <shapefile path>" << endl;
    cout << "-p write out points instead of polygons" << endl;
    cout << "-g write out geographical coordinates instead of cartesian" << endl;
    
    exit(0);
}

using namespace std;

int main(int argc, char** argv) 
{
    if ( argc < 3 ) 
    { 
        usage();
    } 
    else 
    { 
        char* radolanPath = NULL;
        
        char *shapefilePath = NULL;
        
        bool inverse = false;
        
        bool writePoints = false;
        
        std::cout << argv[0];
        
        for ( int i = 1; i < argc; i++ ) 
        { 
            if ( i + 1 != argc ) 
            {
                string argvee( (const char*)argv[i] );
                
                if ( argvee == "-f" ) 
                {
                    radolanPath = argv[i + 1];
                } 
                else if (argvee == "-o") 
                {
                    shapefilePath = argv[i + 1];
                } 
                else if (argvee == "-g") 
                {
                    inverse = true;
                } 
            }
            std::cout << argv[i] << " ";
        }
        
        if ( shapefilePath == NULL || radolanPath == NULL )
        {
            usage();
        }
    
        RDScan* scan = RDAllocateScan();
	
        if ( RDReadScan( radolanPath, scan, true ) == 0 )
        {
            RDFreeScan( scan );
            
            if ( writePoints )
            {
                RDRadolan2PointShapefile( scan, shapefilePath );
            }
            else
            {
                RDRadolan2PolygonShapefile( scan, shapefilePath );
            }
        }
    }
   
    return 0;
}
