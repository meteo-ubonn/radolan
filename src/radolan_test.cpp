

#include <radolan/radolan.h>
#include <radolan/radolan_utils.h>

#include <stdlib.h>
#include <ctime>
#include <stdio.h>

using namespace Radolan;

bool testCoordinateSystem(RDScanType type)
{
    bool failed = false;
    
    RDCoordinateSystem *cs = new RDCoordinateSystem(type);
    
    // test the anchor points
    
    RDGeographicalPoint anchorCoordinate;
    RDGridPoint anchorPoint;
    int maxGridX, maxGridY;
    switch (type) 
    {
        case RD_EX:
        case RD_TH:
        case RD_TZ:
            anchorCoordinate = rdGeographicalPoint(9.0f,51.0f);
            anchorPoint = rdGridPoint(599, 799);
            maxGridX = 1399;
            maxGridY = 1499;
            break;
        default:
            anchorCoordinate = rdGeographicalPoint(9.0f,51.0f);
            anchorPoint = rdGridPoint(449, 449);
            maxGridX = 899;
            maxGridY = 899;
            break;
    }
    
    RDGeographicalPoint geographical = cs->geographicalCoordinate(anchorPoint);
    if (geographical.longitude!=anchorCoordinate.longitude || 
        geographical.latitude!=anchorCoordinate.latitude) 
    {
        fprintf(stderr, "FAILED:geographical coordinates of anchor point don't match\n");
        failed = failed || true;
    }	
    
    bool isInside;
    RDGridPoint	gridpoint = cs->gridPoint(anchorCoordinate,isInside);
    if (gridpoint.ix!=anchorPoint.ix || gridpoint.iy!=anchorPoint.iy) 
    {
        fprintf(stderr, "FAILED:grid coordinates of anchor point don't match\n");
        failed = failed || true;
    }
    
    // test the corners of the grid
    
    RDGridPoint original = rdGridPoint(0, 0);
    RDCartesianPoint cartesian = cs->cartesianCoordinate(original);
    geographical = cs->geographicalCoordinate(original);
    
    gridpoint = cs->gridPoint(cartesian, isInside);
    if (gridpoint.ix != original.ix || gridpoint.iy != original.iy) 
    {
        fprintf(stderr, "FAILED:grid points dont match\n");
        failed = failed || true;
    }
    
    gridpoint = cs->gridPoint(geographical, isInside);
    if (gridpoint.ix != original.ix || gridpoint.iy != original.iy) 
    {
        fprintf(stderr, "FAILED:grid points dont match\n");
        failed = failed || true;
    }
    
    original = rdGridPoint(899, 899);
    cartesian = cs->cartesianCoordinate(original);
    gridpoint = cs->gridPoint(cartesian, isInside);
    if (gridpoint.ix != original.ix || gridpoint.iy != original.iy) 
    {
        fprintf(stderr, "FAILED:grid points dont match\n");
        failed = failed || true;
    }
    
    geographical = cs->geographicalCoordinate(cartesian);
    gridpoint = cs->gridPoint(geographical, isInside);
    if (gridpoint.ix != original.ix || gridpoint.iy != original.iy) 
    {
        fprintf(stderr, "FAILED:grid points dont match\n");
        failed = failed || true;
    }
    
    geographical = cs->geographicalCoordinate(rdGridPoint(0, 0));
    printf("\n\nLower Left Corner: %6.4f , %6.4f\n",geographical.longitude, geographical.latitude);
    
    geographical = cs->geographicalCoordinate(rdGridPoint(maxGridX, maxGridY));
    printf("Upper Right Corner: %6.4f , %6.4f\n",geographical.longitude, geographical.latitude);
    
    return !failed;
}

int main(int argc, char** argv) 
{
    printf("\nendianess = %s\n", isLittleEndian() ? "LITTLE":"BIG" );
    
    bool coordTest = testCoordinateSystem( RD_RX );
    
    printf( "RDCoordinateSystem test: %s\n", coordTest ? "OK" : "FAILED" );

    printf( "RDReadScan test:\n" );
	
    RDScan* scan = RDAllocateScan();
	
	if ( RDReadScan( argv[1], scan, true ) >= 0 )
    {
        RDPrintHeaderInformation( scan );
        
        RDPrintScan( scan, 20, 20 );
        
        RDFreeScan( scan );
        
        struct tm tm_time;
        
        RDScanTime( scan, &tm_time );
        
        time_t time = timegm( &tm_time );
        
        char * fn = RDGuessFilename( scan->header.scanType, time );
        
        printf( "RDGuessFilename = %s\n", fn );
        
        free( fn );
    }
    
	return 0;
}
