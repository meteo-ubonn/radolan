#if WITH_SHAPEFILE
#include <radolan/shapefile.h>
#include <radolan/coordinatesystem.h>
#include <shapefil.h>

#ifdef __cplusplus
namespace Radolan
{
#endif

    int RDRadolan2PointShapefile( RDScan* scan, const char* filename, bool inverse, bool withValues )
    {
        SHPHandle shapefile = withValues ? SHPCreate(filename, SHPT_MULTIPOINTM) : SHPCreate(filename, SHPT_MULTIPOINT);
        
        if (shapefile) 
        {
            int partIndexes[1] = {0};
            
            int *parts = &partIndexes[0];
            
            int maxNumObjects = scan->dimLat * scan->dimLon;
            
            double *px = (double*)calloc(maxNumObjects,sizeof(double));
            
            double *py = (double*)calloc(maxNumObjects,sizeof(double));
            
            double *m = (double*)calloc(maxNumObjects,sizeof(double));
            
            if (px==NULL || py==NULL || m==NULL) 
            {
                fprintf(stderr,"Could not allocate memory\n");
                
                exit(-5);
            }
            
            RDCoordinateSystem rcs = RDCoordinateSystem( scan->header.scanType );
            
            int index=0, ix, iy;
            
            for (iy=0; iy<scan->dimLat; iy++) 
            {
                for (ix=0; ix<scan->dimLon; ix++) 
                {
                    RDDataType value = scan->data[ iy*scan->dimLon + ix ];
                    
                    if (!(value == -32.5 || value==92.5)) 
                    {
                        RDCartesianPoint cart = rcs.cartesianCoordinate( rdGridPoint(ix, iy) );
                        
                        double lat; 
                        
                        double lon;
                        
                        if (inverse==true) 
                        {
                            RDGeographicalPoint geo = rcs.geographicalCoordinate(cart);
                            
                            lon = geo.longitude;
                            
                            lat = geo.latitude;
                        } 
                        else 
                        {
                            lon = cart.x;
                            
                            lat = cart.y;
                        }
                        
                        px[index] = lat;
                        
                        py[index] = lon;
                        
                        m[index] = value;
                        
                        index++;
                    }
                }
            }
            
            printf("Multipoint contains %d values of interest\n",index);
            
            SHPObject *multipoint = withValues 
            ? SHPCreateObject( SHPT_MULTIPOINTM, 0, 1, parts, NULL, index, px, py, NULL, m ) 
            : SHPCreateSimpleObject(SHPT_POINT, 1, px, py, NULL);
            
            SHPWriteObject(shapefile,-1,multipoint);
            
            SHPClose(shapefile);
            
            SHPDestroyObject(multipoint);
            
            free(px);
            
            free(py);
            
            free(m);
        } 
        else 
        {
            fprintf(stderr,"ERROR:Could not open output file %s",filename);
            
            return -1;
        }

        return 0;
    }

    int RDRadolan2PolygonShapefile( RDScan *scan, const char* filename, bool inverse, bool withValues )
    {
        SHPHandle shapefile = withValues 
        ? SHPCreate(filename, SHPT_POLYGONM) 
        : SHPCreate(filename, SHPT_POLYGON);
        
        if (shapefile) 
        {
            int partIndexes[1] = {0};
            
            int *parts = &partIndexes[0];
            
            int ix, iy;
            
            RDCoordinateSystem rcs = RDCoordinateSystem( scan->header.scanType );
            
            for (iy=0; iy<scan->dimLat; iy++) 
            {
                for (ix=0; ix<scan->dimLon; ix++) 
                {
                    double value = scan->data[iy*scan->dimLon + ix];
                    
                    if (!(value == -32.5 || value==92.5)) 
                    {
                        double px[5], py[5], m[5];

                        double lat_min, lat_max, lon_min, lon_max;
                        
                        RDCartesianPoint cart_min = rcs.cartesianCoordinate( rdGridPoint(ix, iy) );
                        RDCartesianPoint cart_max = rcs.cartesianCoordinate( rdGridPoint(ix+1, iy+1) );
                        
                        if (inverse==true) 
                        {
                            RDGeographicalPoint geo_min = rcs.geographicalCoordinate( cart_min );
                            
                            lon_min = geo_min.longitude;
                            
                            lat_min = geo_min.latitude;
                            
                            RDGeographicalPoint geo_max = rcs.geographicalCoordinate( cart_max );
                            
                            lon_max = geo_max.longitude;
                            
                            lat_max = geo_max.latitude;
                        } 
                        else 
                        {
                            lon_min = cart_min.x;
                            
                            lat_min = cart_min.y;
                            
                            lon_max = cart_max.x;
                            
                            lat_max = cart_max.y;
                        }
                        
                        // bottom left
                        px[0] = lon_min;
                        py[0] = lat_min;
                        
                        // top left
                        px[1] = lon_min;
                        py[1] = lat_max;
                        
                        // top right
                        px[2] = lon_max;
                        py[2] = lat_max;
                        
                        // bottom right
                        px[3] = lon_max;
                        py[3] = lat_min;
                        
                        // and back
                        px[4] = lon_min;
                        py[4] = lat_min;
                        
                        m[0] = m[1] = m[2] = m[3] = m[4] = value;
                        
                        SHPObject *polygon = withValues 
                        ? SHPCreateObject( SHPT_POLYGONM, iy*scan->dimLat+ix, 4, parts, NULL, 5, px, py, NULL, m)
                        : SHPCreateSimpleObject( SHPT_POLYGON, 5, px, py, NULL );
                        
                        SHPWriteObject(shapefile, -1, polygon);
                        
                        SHPDestroyObject(polygon);
                    }
                }
            }
            SHPClose(shapefile);
        } 
        else 
        {
            fprintf(stderr,"ERROR:Could not open output file %s",filename);
            
            return -1;
        }

      return 0;
    }

    int RDWriteRadolanBoundingBoxToShapefile( RDScan* scan, const char* filename, bool inverse )
    {
        SHPHandle shapefile = SHPCreate(filename, SHPT_POLYGON);
        
        if (shapefile) 
        {
            double px[5], py[5];
            
            RDCoordinateSystem rcs = RDCoordinateSystem( scan->header.scanType );
            
            RDCartesianPoint cart = rcs.cartesianCoordinate( rdGridPoint(0, 0) );
            
            // beginning and end 
            
            px[0]=(double) cart.x;
            py[0]=(double) cart.y;
            
            px[4]=(double) cart.x;
            py[4]=(double) cart.y;
            
            cart = rcs.cartesianCoordinate( rdGridPoint(0, scan->dimLat-1) );
            
            px[1]=(double) cart.x;
            py[1]=(double) cart.y;
            
            cart = rcs.cartesianCoordinate( rdGridPoint(scan->dimLon-1, scan->dimLat-1) );
            
            px[2]=(double) cart.x;
            py[2]=(double) cart.y;
            
            cart = rcs.cartesianCoordinate( rdGridPoint(scan->dimLon-1, 0) );

            px[3]=(double) cart.x;
            py[3]=(double) cart.y;
            
            if (inverse) 
            {
                int i;
                
                for (i=0; i<5; i++) 
                {
                    cart = rdCartesianPoint( px[i], py[i] );
                    
                    RDGeographicalPoint geo = rcs.geographicalCoordinate( cart );
                    
                    px[i] = (double) geo.longitude;
                    
                    py[i] = (double) geo.latitude;
                }
            }
            
            SHPObject *polygon = SHPCreateSimpleObject( SHPT_POLYGON, 5, px, py, NULL);
            
            SHPWriteObject(shapefile, -1, polygon);
            
            SHPDestroyObject(polygon);
            
            SHPClose(shapefile);
        } 
        else 
        {
            fprintf(stderr,"ERROR:Could not open output file %s",filename);
            
            return -1;
        }

      return 0;
    }

    void RDPrintRadolanBoundingBox( RDScan *scan, bool inverse )
    {
        double px[5], py[5];
        
        RDCoordinateSystem rcs = RDCoordinateSystem( scan->header.scanType );
        
        RDCartesianPoint cart = rcs.cartesianCoordinate( rdGridPoint(0, 0) );
        
        // beginning and end 
        
        px[0]=(double) cart.x;
        py[0]=(double) cart.y;
        
        px[4]=(double) cart.x;
        py[4]=(double) cart.y;
        
        cart = rcs.cartesianCoordinate( rdGridPoint(0, scan->dimLat-1) );
        
        px[1]=(double) cart.x;
        py[1]=(double) cart.y;
        
        cart = rcs.cartesianCoordinate( rdGridPoint(scan->dimLon-1, scan->dimLat-1) );
        
        px[2]=(double) cart.x;
        py[2]=(double) cart.y;
        
        cart = rcs.cartesianCoordinate( rdGridPoint(scan->dimLon-1, 0) );
        
        px[3]=(double) cart.x;
        py[3]=(double) cart.y;
        
        if (inverse) 
        {
            int i;
            
            for (i=0; i<5; i++) 
            {
                cart = rdCartesianPoint( px[i], py[i] );
                
                RDGeographicalPoint geo = rcs.geographicalCoordinate( cart );
                
                px[i] = (double) geo.longitude;
                
                py[i] = (double) geo.latitude;
            }
        }

        printf("Lower Left  Corner 	: (%f,%f)\n",px[0],py[0]);
        printf("Lower Right Corner 	: (%f,%f)\n",px[3],py[3]);
        printf("Upper Right Corner 	: (%f,%f)\n",px[2],py[2]);
        printf("Upper Left  Corner 	: (%f,%f)\n",px[1],py[1]);
    }

    void RDRadolan2Proj( RDScan *scan, bool inverse )
    {
        float lat, lon;
        
        int ix, iy;
        
        RDCoordinateSystem rcs = RDCoordinateSystem( scan->header.scanType );
        
        for (iy=0; iy<scan->dimLat; iy++) 
        {
            for (ix=0; ix<scan->dimLon; ix++) 
            {
                if ( inverse )
                {
                    RDGeographicalPoint geo = rcs.geographicalCoordinate( rdGridPoint(ix, iy) );
                    
                    lon = geo.longitude;
                    
                    lat = geo.latitude;
                }
                else
                {
                    RDCartesianPoint cart = rcs.cartesianCoordinate( rdGridPoint(ix, iy) );
                    
                    lon = cart.x;
                    
                    lat = cart.y;
                }

                printf("%f\t%f\t%f\n",lat,lon,scan->data[iy*scan->dimLat + ix]);
            }
        }
    }
#ifdef __cplusplus
  }
#endif
#endif