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
    cout << "-b ignores the input file and writes out the bounding box" << endl;
    cout << "-g write out geographical coordinates instead of cartesian" << endl;
    cout << "-n write out simple shapes without values (POINTM/POLYGONM)" << endl;
    
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
        
        bool writeBoundingBox = false;
        
        bool withValues = true;

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
                else if (argvee == "-b")
                {
                    writeBoundingBox = true;
                }
                else if (argvee == "-n")
                {
                    withValues = false;
                }
            }
        }
        
        if ( shapefilePath == NULL || radolanPath == NULL )
        {
            usage();
        }
        
        RDScan* scan = RDAllocateScan();
        
        if ( RDReadScan( radolanPath, scan, true ) == 0 )
        {
            if ( writeBoundingBox )
            {
                cout << endl << "Writing bounding box for scan type " << RDScanTypeToString(scan->header.scanType) << " ... ";
                
                RDWriteRadolanBoundingBoxToShapefile( scan, shapefilePath, inverse ); 

                cout << "done." << endl;
                
                return 0;
            }
            
            cout << "Converting file " << radolanPath << " to "
            << (writePoints ? "points" : "polygon") << " shapefile " 
            << (withValues ? "with" : "without" ) << " values " 
            << (inverse ? "in geographical" : "in polar stereographic") << " coordinates" 
            << " to file " << shapefilePath << endl;
            
            if ( writePoints )
            {
                
                RDRadolan2PointShapefile( scan, shapefilePath, inverse, withValues );
            }
            else
            {
                RDRadolan2PolygonShapefile( scan, shapefilePath, inverse, withValues );
            }
            
            RDFreeScan( scan );
        }
    }
    
    return 0;
}
