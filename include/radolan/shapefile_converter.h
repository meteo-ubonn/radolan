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

#if WITH_SHAPELIB

#ifndef RADOLAN_SHAPEFILE_CONVERTER_H
#define RADOLAN_SHAPEFILE_CONVERTER_H

#include <radolan/conversion_exeption.h>
#include <radolan/types.h>
#include <vector>

namespace Radolan {

    /**
     * This class contains facilities for converting a RADOLAN
     * file into 2 different Shapefile formats. One is based on
     * individual points, the other produces polygons.
     */
    class Radolan2Shapefile {
    public:

        /**
         * Writes out a shapefile with the individual radar pixels as points
         *
         * @param scan radolan scan
         * @param filename shapefile output filename
         * @param geographic if <code>true</code> the coordinates are transformed to lat/lon.
         *        If <code>false</code> they are polar-stereographic (cartesian).
         * @param withValues If <code>true</code>, write out SHPT_MULTIPOINTM (with scan value).
         *        If <code>false</code> writes out SHPT_MULTIPOINT.
         *
         * @throws RDConversionException
         *
         */
        static void convertToPoints(RDScan *scan,
                                    const char *filename,
                                    bool geographic = false,
                                    bool withValues = true);
        /**
         * Writes out a shapefile with the individual radar pixels as polygons
         *
         * @param scan
         * @param filename shapefile output filename
         * @param geographic If <code>true</code> the coordinates are transformed to lat/lon.
         *        If <code>false</code> they are polar-stereographic (cartesian).
         * @param withValues If <code>true</code>, write out SHPT_POLYGONM (with scan value).
         *        If <code>false</code> writes out SHPT_POLYGON.
         * @throws RDConversionException
         */
        static void convertToPolygons(RDScan *scan,
                                      const char *filename,
                                      bool geographic = false,
                                      bool withValues = true);

        /**
         * Obtains the bounding box of the radolan scan.
         * TODO: this might be better off in utils or sth?
         *
         * @param scan
         * @param px contains the x (or lon) coordinates of the corners of the scan.
         * @param py contains the y (or lat) coordinates of the corners of the scan.
         *
         * @param geographic if <code>true</code> the coordinates are transformed to lat/lon.
         *        If <code>false</code> they are polar-stereographic (cartesian).
         */
        static void getBoundingBoxPolygon(RDScan *scan,
                                          std::vector<double> &px,
                                          std::vector<double> &py,
                                          bool geographic = false);

        /**
         * Writes out the bounding box of the coordinate system as shapefile
         *
         * @param scan
         * @param filename shapefile output filename
         * @param geographic if <code>true</code> the coordinates are transformed to lat/lon.
         *        If <code>false</code> they are polar-stereographic (cartesian).
         *
         * @throws RDConversionException
         */
        static void writeBoundingBox(RDScan *scan,
                                     const char *filename,
                                     bool geographic = false);

        /**
         * Prints out the bounding box of the coordinate system to console
         * TODO: this might be better off in utils or sth?
         * @param scan
         * @param geographic if <code>true</code> the coordinates are transformed to lat/lon.
         *        If <code>false</code> they are polar-stereographic (cartesian).
         */
        static void printBoundingBox(RDScan *scan,
                                     bool geographic = false);

        /**
         * Prints the scan data in proj format to console.
         * TODO: this might be better off in utils or sth?
         *
         * @param scan
         * @param geographic if <code>true</code> the coordinates are transformed to lat/lon.
         *        If <code>false</code> they are polar-stereographic (cartesian).
         */
        static void printAsProj(RDScan *scan,
                                bool geographic = false);
    };
}

#endif // WITH_SHAPELIB
#endif
