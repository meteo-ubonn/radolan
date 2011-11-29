#ifndef RADOLANCOORDINATESYSTEM_H_
#define RADOLANCOORDINATESYSTEM_H_

//C-headers
//C++ headers
//3rd-party headers
#include <radolan/types.h>

// TODO: refactor using RDGridPoint and RDGeographicalPointRad etc

namespace Radolan
{
	
	/** Represents a point in geographical coordinate system in radians
	 * as (latitude,longitude) 
	 */
	typedef struct {
		double latitude;
		double longitude;
	} RDGeographicalPointRad;
	
	inline RDGeographicalPointRad rdGeographicalPointRad(double longitude, double latitude)
	{
		RDGeographicalPointRad p;
		p.longitude = longitude;
		p.latitude = latitude;
		return p;
	}
	
	inline bool RDEquals(RDGeographicalPointRad a, RDGeographicalPointRad b)
	{
		return a.latitude == b.latitude && a.longitude == b.longitude;
	}

	#define RDGeographicalPointRadUndefined rdGeographicalPointRad(NAN,NAN)
	
	/** Represents a point in geographical coordinate system in grad
	 * as (latitude,longitude) 
	 */
	typedef struct {
		double latitude;
		double longitude;
	} RDGeographicalPoint;
	
	inline RDGeographicalPoint rdGeographicalPoint(double longitude, double latitude)
	{
		RDGeographicalPoint p;
		p.longitude = longitude;
		p.latitude = latitude;
		return p;
	}
	
	inline bool RDEquals(RDGeographicalPoint a, RDGeographicalPoint b)
	{
		return a.latitude == b.latitude && a.longitude == b.longitude;
	}
	
	#define RDGeographicalPointUndefined rdGeographicalPoint(NAN,NAN)
	
	/** Represents a point in the cartesian coordinate system 
	 * (polar stereographic projection) as (x,y) 
	 */
	typedef struct {
		double x;
		double y;
	} RDCartesianPoint;
	
	inline RDCartesianPoint rdCartesianPoint(double x, double y)
	{
		RDCartesianPoint p;
		p.x = x;
		p.y = y;
		return p;
	}
	
	inline bool RDEquals(RDCartesianPoint a, RDCartesianPoint b)
	{
		return a.x == b.x && a.y == b.y;
	}
	
	#define RDCartesianPointUndefined rdCartesianPoint(NAN,NAN)
	
	/** Represents a point in the radolan grid by the column/row
	 * indexes as (ix,iy) 
	 */
	typedef struct {
		int ix;
		int iy;
	} RDGridPoint;
	
	inline RDGridPoint rdGridPoint(int ix, int iy)
	{
		RDGridPoint p;
		p.ix = ix;
		p.iy = iy;
		return p;
	}
	
	inline bool RDEquals(RDGridPoint a, RDGridPoint b)
	{
		return a.ix == b.ix && a.iy == b.iy;
	}
	
	#define RDGridPointUndefined rdGridPoint(-1, -1)
	
	inline RDDataType RDValueAt(RDScan* scan, RDGridPoint gridPoint) 
	{
		return scan->data[gridPoint.iy * scan->dimLon + gridPoint.ix];
	}

	//  1 | 2
	//  - + -
	//  0 | 3
	
	typedef enum {
		RDLowerLeft=0,
		RDUpperLeft=1,
		RDUpperRight=2,
		RDLowerRight=3
	} RDGridQuadrant;
	
	class RDCoordinateSystem 
	{
	public:
		
		RDCoordinateSystem(RDScanType type);
		
	    virtual ~RDCoordinateSystem();
		
		/** change scan type */
		void setScanType(RDScanType type);

		/** Calculate the polar stereographic coordinate from the given geographical coordinate 
		 * @param geographical coordinate in radians
		 * @return cartesian coordinate in the polar stereographic coordinate system
		 */
		RDCartesianPoint cartesianCoordinate(RDGeographicalPointRad);
		
		/** Calculate the polar stereographic coordinate from the given geographical coordinate 
		 * @param geographical coordinate in deg
		 * @return cartesian coordinate in the polar stereographic coordinate system
		 */
		RDCartesianPoint cartesianCoordinate(RDGeographicalPoint);
		
		/** Calculate the polar stereographic coordinate from the given grid point
		 * @param grid point
		 * @return cartesian coordinate in the polar stereographic coordinate system
		 */
		RDCartesianPoint cartesianCoordinate(RDGridPoint);
		
		/** Calculate geographical coordinate from the given polar stereographic coordinate
		 * @param cartesian coordinate in the polar stereographic coordinate system
		 * @return geographical coordinate in deg
		 */
		RDGeographicalPoint geographicalCoordinate(RDCartesianPoint);
		
		/** Calculate geographical coordinate from the given polar stereographic coordinate
		 * @param grid point
		 * @return geographical coordinate in deg
		 */
		RDGeographicalPoint geographicalCoordinate(RDGridPoint);

		/** Calculate geographical coordinate from the given polar stereographic coordinate
		 * @param cartesian coordinate in the polar stereographic coordinate system
		 * @return geographical coordinate in radians
		 */
		RDGeographicalPointRad geographicalCoordinateRad(RDCartesianPoint p);
		
		/** Calculate geographical coordinate from the given polar stereographic coordinate
		 * @param grid point
		 * @return geographical coordinate in radians
		 */
		RDGeographicalPointRad geographicalCoordinateRad(RDGridPoint p);
		
		/** Convert deg to radians 
		 * @param geographical coordinate in deg
		 * @return geographical coordinate in radians
		 */ 
		RDGeographicalPointRad toRad(RDGeographicalPoint);
		
		/** Convert radians to deg 
		 * @param geographical coordinate in radians
		 * @return geographical coordinate in deg
		 */ 
		RDGeographicalPoint toDeg(RDGeographicalPointRad);
		
		/** Calculate the geographical coordinates for the bounding box of the given grid point.
		 * @param grid point
		 * @param contains the number of points in the polygon after the call
		 * @return array of geographical coordinates (the polygon's points)
		 */
		RDGeographicalPoint* geographicalPolygonForGridpoint(RDGridPoint, int&);
		
		/** Calculate the nearest grid point for the given cartesian coordinate.
		 * @param cartesian coordinate
		 * @param contains the result of the bounds check. If given coordinate is 
	     *        outside the grid, this value will be false, otherwise true.
		 * @return grid point
		 */
		RDGridPoint gridPoint(RDCartesianPoint, bool&);

		/** Calculate the nearest grid point for the given geographical coordinate
		 * @param geographical coordinate in deg
		 * @param contains the result of the bounds check. If given coordinate is 
	     *        outside the grid, this value will be false, otherwise true.
		 * @return grid point
		 */
		RDGridPoint gridPoint(RDGeographicalPoint, bool&);
		
		/** Calculate the nearest grid point for the given geographical coordinate
		 * @param geographical coordinate in radians
		 * @param contains the result of the bounds check. If given coordinate is 
	     *        outside the grid, this value will be false, otherwise true.
		 * @return grid point
		 */
		RDGridPoint gridPoint(RDGeographicalPointRad g, bool& isInside);

		/** Find out what quadrant of the grid with respect to the grid's cartesian
		 * origin coordinates the given grid point resides in.
		 */
		RDGridQuadrant RDQuadrant(RDGridPoint p);
		
	private:

		// these parameters depend on the radolan product
		int m_radolanGridCountHorizontal;
		int m_radolanGridCountVertical;

		// Origin of the coordinate system in geographical coordinates
		RDGeographicalPoint m_originGeographical;
		
		// Origin in cartesian coordinates
		RDCartesianPoint m_originCartesian;

		// Offset of the origin from lower left corner (example: EX: -600, -800)
		RDCartesianPoint m_offset;
		
		RDScanType m_scanType;

		// These are parameters of the projection itself and don't change
		static const double LAMBDA_0;
		static const double PHI_0;
		static const double R_EARTH;
		static const double MESH_WIDTH;
		static const double RADOLAN_GRID_COUNT;
		static const double LAMBDA_MIN_BOTTOM;
		static const double PHI_MIN_BOTTOM;
		static const double LAMBDA_MAX_TOP;
		static const double PHI_MAX_TOP;
		
		/** deg -> rad */
	    double deg(double rad);
	    
	    /** rad -> deg */
	    double rad(double deg);
	    
	    /** Scaling Factor for the polar stereographic projection */
		double polarStereographicScalingFactor(double phi0,double phi);

		// Called after changing scan type to update internals
		void updateGridInfo();
		
	}; //class RDCoordinateSystem

} //namespace Radolan

#endif /*RADOLANCOORDINATESYSTEM_H_*/


