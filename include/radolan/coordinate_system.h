/* The MIT License (MIT)
 *
 * (c) Jürgen Simon 2014 (juergen.simon@uni-bonn.de)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef RADOLAN_COORDINATE_SYSTEM_H
#define RADOLAN_COORDINATE_SYSTEM_H

//C-headers
//C++ headers
//3rd-party headers
#include <radolan/types.h>

// TODO: refactor using RDGridPoint and RDGeographicalPointRad etc

namespace Radolan {

    /**
     * Represents a point in geographical coordinate system in radians
     * as (latitude,longitude)
     */
    typedef struct
    {
        /// Geographical latitude (in radians)
        double latitude;
        /// Geographical longitute (in radians)
        double longitude;
    } RDGeographicalPointRad;

    inline RDGeographicalPointRad rdGeographicalPointRad(double longitude, double latitude) {
        RDGeographicalPointRad p;
        p.longitude = longitude;
        p.latitude = latitude;
        return p;
    }

    inline bool RDEquals(RDGeographicalPointRad a, RDGeographicalPointRad b) {
        return a.latitude == b.latitude && a.longitude == b.longitude;
    }

#define RDGeographicalPointRadUndefined rdGeographicalPointRad(NAN,NAN)

    /** Represents a point in geographical coordinate system in grad
     * as (latitude,longitude)
     */
    typedef struct
    {
        /// Geographical latitude
        double latitude;
        /// Geographical longitude
        double longitude;
    } RDGeographicalPoint;

    inline RDGeographicalPoint rdGeographicalPoint(double longitude, double latitude) {
        RDGeographicalPoint p;
        p.longitude = longitude;
        p.latitude = latitude;
        return p;
    }

    inline bool RDEquals(RDGeographicalPoint a, RDGeographicalPoint b) {
        return a.latitude == b.latitude && a.longitude == b.longitude;
    }

#define RDGeographicalPointUndefined rdGeographicalPoint(NAN,NAN)

    /** Represents a point in the cartesian coordinate system
     * (polar stereographic projection) as (x,y)
     */
    typedef struct
    {
        /// Cartesian x coordinate
        double x;
        /// Cartesian y coordinate
        double y;
    } RDCartesianPoint;

    inline RDCartesianPoint rdCartesianPoint(double x, double y) {
        RDCartesianPoint p;
        p.x = x;
        p.y = y;
        return p;
    }

    inline bool RDEquals(RDCartesianPoint a, RDCartesianPoint b) {
        return a.x == b.x && a.y == b.y;
    }

#define RDCartesianPointUndefined rdCartesianPoint(NAN,NAN)

    /** Represents a point in the radolan grid by the column/row
     * indexes as (ix,iy)
     */
    typedef struct
    {
        /// horizontal index
        int ix;
        /// vertical index
        int iy;
    } RDGridPoint;

    inline RDGridPoint rdGridPoint(int ix, int iy) {
        RDGridPoint p;
        p.ix = ix;
        p.iy = iy;
        return p;
    }

    inline bool RDEquals(RDGridPoint a, RDGridPoint b) {
        return a.ix == b.ix && a.iy == b.iy;
    }

#define RDGridPointUndefined rdGridPoint(-1, -1)

    inline RDDataType RDValueAt(RDScan *scan, RDGridPoint gridPoint) {
        return scan->data[gridPoint.iy * scan->dimLon + gridPoint.ix];
    }

    //  1 | 2
    //  - + -
    //  0 | 3

    typedef enum
    {
        RDLowerLeft = 0,
        RDUpperLeft = 1,
        RDUpperRight = 2,
        RDLowerRight = 3
    } RDGridQuadrant;

    /** This class wraps a number of routines to deal with the radolan specific coordinate systems.
     * It provides methods for converting geographical coordinates into cartesian coordinates (within
     * the radolan grid) as well as conversion from and to grid points - aka index pairs.
     */
    class RDCoordinateSystem
    {
    public:

        /**
         * Constructs a coordinate system for a specific RDScanType
         *
         * Constructor.
         * @param type
         */
        RDCoordinateSystem(RDScanType type);

        /**
         * Destructor.
         */
        virtual ~RDCoordinateSystem();

        /**
         * Changes the scan type.
         *
         * @param type scan type
         */
        void setScanType(RDScanType type);

        /**
         * Calculate the polar stereographic coordinate from the given geographical coordinate
         *
         * @param coord geographical coordinate in radians
         * @return cartesian coordinate in the polar stereographic coordinate system
         */
        RDCartesianPoint cartesianCoordinate(RDGeographicalPointRad coord);

        /**
         * Calculate the polar stereographic coordinate from the given geographical coordinate.
         *
         * @param coord geographical coordinate in deg
         * @return cartesian coordinate in the polar stereographic coordinate system
         */
        RDCartesianPoint cartesianCoordinate(RDGeographicalPoint coord);

        /**
         * Calculate the polar stereographic coordinate from the given grid point.
         *
         * @param gridpoint grid point
         * @return cartesian coordinate in the polar stereographic coordinate system
         */
        RDCartesianPoint cartesianCoordinate(RDGridPoint gridpoint);

        /**
         * Calculate geographical coordinate from the given polar stereographic coordinate.
         *
         * @param p cartesian coordinate in the polar stereographic coordinate system
         * @return geographical coordinate in deg
         */
        RDGeographicalPoint geographicalCoordinate(RDCartesianPoint p);

        /**
         * Calculate geographical coordinate from the given polar stereographic coordinate.
         *
         * @param p grid point
         * @return geographical coordinate in deg
         */
        RDGeographicalPoint geographicalCoordinate(RDGridPoint p);

        /**
         * Calculate geographical coordinate from the given polar stereographic coordinate.
         *
         * @param p cartesian coordinate in the polar stereographic coordinate system
         * @return geographical coordinate in radians
         */
        RDGeographicalPointRad geographicalCoordinateRad(RDCartesianPoint p);

        /**
         * Calculate geographical coordinate from the given polar stereographic coordinate.
         *
         * @param p grid point
         * @return geographical coordinate in radians
         */
        RDGeographicalPointRad geographicalCoordinateRad(RDGridPoint p);

        /**
         * Convert deg to radians.
         *
         * @param p geographical coordinate in deg
         * @return geographical coordinate in radians
         */
        RDGeographicalPointRad toRad(RDGeographicalPoint p);

        /**
         * Convert radians to deg.
         *
         * @param p geographical coordinate in radians
         * @return geographical coordinate in deg
         */
        RDGeographicalPoint toDeg(RDGeographicalPointRad p);

        /**
         * Calculate the geographical coordinates for the bounding box of the given grid point.
         *
         * @param p grid point
         * @param num contains the number of points in the polygon after the call
         * @return array of geographical coordinates (the polygon's points)
         */
        RDGeographicalPoint *geographicalPolygonForGridpoint(RDGridPoint p, int &num);

        /**
         * Calculate the nearest grid point for the given cartesian coordinate.
         *
         * @param p cartesian coordinate
         * @param isInside contains the result of the bounds check. If given coordinate is
         *        outside the grid, this value will be false, otherwise true.
         * @return grid point
         */
        RDGridPoint gridPoint(RDCartesianPoint p, bool &isInside);

        /**
         * Calculate the nearest grid point for the given geographical coordinate.
         *
         * @param p geographical coordinate in deg
         * @param isInside contains the result of the bounds check. If given coordinate is
         *        outside the grid, this value will be false, otherwise true.
         * @return grid point
         */
        RDGridPoint gridPoint(RDGeographicalPoint p, bool &isInside);

        /**
         * Calculate the nearest grid point for the given geographical coordinate.
         *
         * @param p geographical coordinate in radians
         * @param isInside contains the result of the bounds check. If given coordinate is
         *        outside the grid, this value will be false, otherwise true.
         * @return grid point
         */
        RDGridPoint gridPoint(RDGeographicalPointRad p, bool &isInside);

        /**
         * Find out what quadrant of the grid with respect to the grid's cartesian
         * origin coordinates the given grid point resides in.
         *
         * @param p grid point
         * @return quadrant
         */
        RDGridQuadrant RDQuadrant(RDGridPoint p);

        /**
         * Scaling Factor for the polar stereographic projection.
         *
         * @param phi0
         * @param phi
         * @return scaling factor.
         */
        double polarStereographicScalingFactor(double phi0, double phi);

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

        // Called after changing scan type to update internals
        void updateGridInfo();

    }; //class RDCoordinateSystem
}

#endif /* Header Guard */