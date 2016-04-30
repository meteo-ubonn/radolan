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

#include <radolan/shapefile_converter.h>
#include <radolan/coordinate_system.h>
#include <shapefil.h>

#ifdef __cplusplus
namespace Radolan {
#endif

    void Radolan2Shapefile::convertToPoints(RDScan *scan,
                                            const char *filename,
                                            bool geographic,
                                            bool withValues) throw(RDConversionException)
    {
        SHPHandle shapefile = withValues ? SHPCreate(filename, SHPT_MULTIPOINTM) : SHPCreate(filename, SHPT_MULTIPOINT);
        if (shapefile) {
            int partIndexes[1] = {0};
            int *parts = &partIndexes[0];
            int maxNumObjects = scan->dimLat * scan->dimLon;

            double *px = (double *) calloc(maxNumObjects, sizeof(double));
            double *py = (double *) calloc(maxNumObjects, sizeof(double));
            double *m = (double *) calloc(maxNumObjects, sizeof(double));

            if (px == NULL || py == NULL || m == NULL) {
                throw new RDConversionException("Could not allocate memory");
            }

            RDCoordinateSystem rcs = RDCoordinateSystem(scan->header.scanType);
            int index = 0, ix, iy;
            for (iy = 0; iy < scan->dimLat; iy++) {
                for (ix = 0; ix < scan->dimLon; ix++) {
                    RDDataType value = scan->data[iy * scan->dimLon + ix];
                    if (!(value == -32.5 || value == 92.5)) {
                        RDCartesianPoint cart = rcs.cartesianCoordinate(rdGridPoint(ix, iy));
                        double lat;
                        double lon;
                        if (geographic == true) {
                            RDGeographicalPoint geo = rcs.geographicalCoordinate(cart);
                            lon = geo.longitude;
                            lat = geo.latitude;
                        } else {
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

            SHPObject *multipoint = withValues
                ? SHPCreateObject(SHPT_MULTIPOINTM, 0, 1, parts, NULL, index, px, py, NULL, m)
                : SHPCreateSimpleObject(SHPT_MULTIPOINT, 1, px, py, NULL);

            SHPWriteObject(shapefile, -1, multipoint);
            SHPClose(shapefile);
            SHPDestroyObject(multipoint);
            free(px);
            free(py);
            free(m);
        } else {
            throw new RDConversionException("Could not open output file");
        }
    }

    void Radolan2Shapefile::convertToPolygons(RDScan *scan,
                                              const char *filename,
                                              bool geographic,
                                              bool withValues) throw(RDConversionException) {
        SHPHandle shapefile = withValues
          ? SHPCreate(filename, SHPT_POLYGONM)
          : SHPCreate(filename, SHPT_POLYGON);

        if (shapefile) {
            int partIndexes[1] = {0};
            int *parts = &partIndexes[0];
            int ix, iy;

            RDCoordinateSystem rcs = RDCoordinateSystem(scan->header.scanType);
            for (iy = 0; iy < scan->dimLat; iy++) {
                for (ix = 0; ix < scan->dimLon; ix++) {
                    double value = scan->data[iy * scan->dimLon + ix];
                    if (!(value == -32.5 || value == 92.5)) {
                        double px[5], py[5], m[5];
                        double lat_min, lat_max, lon_min, lon_max;

                        RDCartesianPoint cart_min = rcs.cartesianCoordinate(rdGridPoint(ix, iy));
                        RDCartesianPoint cart_max = rcs.cartesianCoordinate(rdGridPoint(ix + 1, iy + 1));

                        if (geographic == true) {
                            RDGeographicalPoint geo_min = rcs.geographicalCoordinate(cart_min);
                            lon_min = geo_min.longitude;
                            lat_min = geo_min.latitude;
                            RDGeographicalPoint geo_max = rcs.geographicalCoordinate(cart_max);
                            lon_max = geo_max.longitude;
                            lat_max = geo_max.latitude;
                        } else {
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
                         ? SHPCreateObject(SHPT_POLYGONM, iy * scan->dimLat + ix, 4, parts, NULL, 5, px, py, NULL, m)
                         : SHPCreateSimpleObject(SHPT_POLYGON, 5, px, py, NULL);

                        SHPWriteObject(shapefile, -1, polygon);
                        SHPDestroyObject(polygon);
                    }
                }
            }
            SHPClose(shapefile);
        } else {
            throw new RDConversionException("Could not open output file");
        }
    }

    void Radolan2Shapefile::getBoundingBoxPolygon(RDScan *scan,
                                                  std::vector<double> &px,
                                                  std::vector<double> &py,
                                                  bool geographic)
    {
        RDCoordinateSystem rcs = RDCoordinateSystem(scan->header.scanType);
        RDCartesianPoint cart = rcs.cartesianCoordinate(rdGridPoint(0, 0));

        px.resize(5, 0.0);
        py.resize(5, 0.0);

        // beginning and end
        px[0] = (double) cart.x;
        py[0] = (double) cart.y;

        cart = rcs.cartesianCoordinate(rdGridPoint(0, scan->dimLat - 1));
        px[1] = (double) cart.x;
        py[1] = (double) cart.y;

        cart = rcs.cartesianCoordinate(rdGridPoint(scan->dimLon - 1, scan->dimLat - 1));
        px[2] = (double) cart.x;
        py[2] = (double) cart.y;

        cart = rcs.cartesianCoordinate(rdGridPoint(scan->dimLon - 1, 0));
        px[3] = (double) cart.x;
        py[3] = (double) cart.y;

        if (geographic) {
            int i;
            for (i = 0; i < 4; i++) {
                cart = rdCartesianPoint(px[i], py[i]);
                RDGeographicalPoint geo = rcs.geographicalCoordinate(cart);
                px[i] = (double) geo.longitude;
                py[i] = (double) geo.latitude;
            }
        }
    }

    void Radolan2Shapefile::writeBoundingBox(RDScan *scan,
                                             const char *filename,
                                             bool geographic) throw(RDConversionException) {
        SHPHandle shapefile = SHPCreate(filename, SHPT_POLYGON);
        if (shapefile) {
            std::vector<double> vx, vy;
            Radolan2Shapefile::getBoundingBoxPolygon(scan, vx, vy, geographic);

            double px[5], py[5];
            for (size_t i=0; i<4; i++) {
                px[i] = vx[i];
                py[i] = vy[i];
            }
            px[4] = px[0];
            py[4] = py[0];

            SHPObject *polygon = SHPCreateSimpleObject(SHPT_POLYGON, 5, px, py, NULL);
            SHPWriteObject(shapefile, -1, polygon);
            SHPDestroyObject(polygon);
            SHPClose(shapefile);
        } else {
            throw new RDConversionException("Could not open output file");
        }
    }

    void Radolan2Shapefile::printBoundingBox(RDScan *scan, bool geographic) {
        std::vector<double> px,py;
        Radolan2Shapefile::getBoundingBoxPolygon(scan, px, py, geographic);

        printf("Lower Left  Corner 	: (%f,%f)\n", px[0], py[0]);
        printf("Lower Right Corner 	: (%f,%f)\n", px[3], py[3]);
        printf("Upper Right Corner 	: (%f,%f)\n", px[2], py[2]);
        printf("Upper Left  Corner 	: (%f,%f)\n", px[1], py[1]);
    }

    void Radolan2Shapefile::printAsProj(RDScan *scan, bool geographic) {
        float lat, lon;
        int ix, iy;
        RDCoordinateSystem rcs = RDCoordinateSystem(scan->header.scanType);
        for (iy = 0; iy < scan->dimLat; iy++) {
            for (ix = 0; ix < scan->dimLon; ix++) {
                if (geographic) {
                    RDGeographicalPoint geo = rcs.geographicalCoordinate(rdGridPoint(ix, iy));
                    lon = geo.longitude;
                    lat = geo.latitude;
                }
                else {
                    RDCartesianPoint cart = rcs.cartesianCoordinate(rdGridPoint(ix, iy));
                    lon = cart.x;
                    lat = cart.y;
                }
                printf("%f\t%f\t%f\n", lat, lon, scan->data[iy * scan->dimLat + ix]);
            }
        }
    }

#ifdef __cplusplus
}
#endif

#endif