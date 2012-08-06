#ifndef __RADOLAN_SHAPEFILE
#define __RADOLAN_SHAPEFILE

#include <radolan/types.h>

#ifdef __cplusplus
extern "C" 
{
    namespace Radolan
    {
#endif
        
        /** Writes out a shapefile with the individual radar pixels as points 
         * @param scan
         * @param shapefile filename
         @ @param inverse if true, coordinates are transformed using the inverse 
         *        transformation back from polar stereographic into lat/lon. Defaults to false
         * @return 0 if all ok, error code else.
         */
        int RDRadolan2PointShapefile( RDScan* scan, const char* filename, bool inverse=false, bool withValues=true );
        
        /** Writes out a shapefile with the individual radar pixels as polygons
         * @param scan
         * @param shapefile filename
         @ @param inverse if true, coordinates are transformed using the inverse 
         *        transformation back from polar stereographic into lat/lon. Defaults to false
         * @return 0 if all ok, error code else.
         */
        int RDRadolan2PolygonShapefile( RDScan *scan, const char* filename, bool inverse=false, bool withValues=true );
        
        /** Writes out the bounding box of the coordinate system as shapefile
         * @param scan
         * @param shapefile filename
         @ @param inverse if true, coordinates are transformed using the inverse 
         *        transformation back from polar stereographic into lat/lon. Defaults to false
         * @return 0 if all ok, error code else.
         */
        int RDWriteRadolanBoundingBoxToShapefile( RDScan* scan, const char* filename, bool inverse=false );
        
        /** Prints out the bounding box of the coordinate system to console
         * @param scan
         @ @param inverse if true, coordinates are transformed using the inverse 
         *        transformation back from polar stereographic into lat/lon. Defaults to false
         * @return 0 if all ok, error code else.
         */
        void RDPrintRadolanBoundingBox( RDScan *scan, bool inverse=false );
        
        /** Prints out the scan data in proj format to console
         * @param scan
         * @return 0 if all ok, error code else.
         */    
        void RDRadolan2Proj( RDScan *scan, bool inverse=false );
        
#ifdef __cplusplus
    }
}
#endif

#endif
