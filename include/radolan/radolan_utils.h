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

#ifndef __RADOLAN_UTILS__
#define __RADOLAN_UTILS__

#include <radolan/types.h>

#ifdef __cplusplus

#include <ctime>

extern "C" {
namespace Radolan {
#else
#include <time.h>
#endif

    /**
     * Prints a representation of the given data as ASCII to stdout.
     *
     * If point is outside scanned area			: '.' (dot)<br>
     * If point is inside scanned area, but 0	: ' ' (blank)<br>
     * If point is inside and not 0				: '*' (asterisk)<br>
     *
     * @param radolan_scan* the scan to plot
     * @param int n print every n-th latitudinal vertice
     * @param int m print every m-th longitudinal vertice
     */
    void RDPrintPolarStereographic(RDScan *, int, int);

    /** Print the header information to stdout */
    void RDPrintHeaderInformation(RDScan *scan);

    /** Prints an ASCII representation of the scan to console, using every latCount
     *  or lonCount - th vertice.
     */
    void RDPrintScan(RDScan *scan, int latCount, int lonCount);

    /** Return a string representation of the given scan type */
    const char *RDScanTypeToString(RDScanType type);

    /** Return a scan type for the given string representation.
     * @return the scan type or RD_UNKNOWN
     */
    RDScanType RDScanTypeFromString(const char *str);

    /** Returns the minimum value for the given scan type */
    RDDataType RDMinValue(RDScanType t);

    /** Returns the maximum value for the given scan type */
    RDDataType RDMaxValue(RDScanType t);

    /** Returns the value marking a missing value for the given scan type */
    RDDataType RDMissingValue(RDScanType t);

    /** Returns the value marking a clutter measurement for the given scan type */
    RDDataType RDClutterValue(RDScanType t);

    /** Returns a unit qualifier
     * @param scan type
     * @return unit qualifier (dBZ, mm/h etc.)
     */
    const char *RDUnits(RDScanType type);

    /** Converts RVP6 units (reflectivity) to byte values. */
    RDByteType RDRVP6ToByteValue(float rvp6);

    /** Converts byte values to RVP6 units (reflectivity). */
    float RDByteToRVP6Value(const RDByteType byte_value);

    /** Checks if the given value is a valid measurement for the given scan type (no clutter, not outside, within bounds) */
    int RDIsCleanMeasurement(RDScanType t, RDDataType value);

    /** Checks if the given value is a valid measurement for the given scan type (no clutter, not outside, within bounds), but min is not allowed*/
    int RDIsCleanMeasurementAndNotMin(RDScanType t, RDDataType value);

    /** Returns the number of bytes per pixel in the original data set */
    size_t RDBytesPerPixel(RDScanType type);

    /** Standard Z/R relationship as used by the DWD */
    float RDRainrateFromDezibels(RDDataType dezibels);

    /** Returns the grid dimensions for the scan type
     * @param scan type
     * @param pointer to grid width
     * @param pointer to grid height
     */
    void RDGridSize(RDScanType t, size_t *width, size_t *height);

    /** Construct a timestruct with timezone gmt and the other values
     * set according to the scan's header.
     * @param scan
     * @param time struct pointer
     */
    void RDScanTime(RDScan *scan, struct tm *);

    /** Returns the scan time in seconds since the epoch (1.1.1970)
     * in UTC timezone.
     * @param scan
     * @return time_t seconds since epoch (1970 00:00:00,0 +0:00)
     */
    time_t RDScanTimeInSecondsSinceEpoch(RDScan *scan);

    /** Some scan types deliver values in 1/100 mm / 5 mins and so on.
     * This method allows calculation in values of mm/hour, as is usual.
     * Formats, which give values in 1/100 mm / 5 mins will be calculated
     * in mm/h, summative formats will be corrected in quantity but not time.
     * This method does not apply to reflectivity based scans, RX, DX etc.
     * and will return -INFINITY for those.
     * @param scan type
     * @param value
     * @return the converted value or -inf if not applicable
     * set according to the scan's header
     */
    float RDMMPerHour(RDScanType t, RDDataType value);

    /** Attempt to reconstruct the original filename from the given scan type and timestamp.
     *  <b>Warning:<b> this is strictly experimental code. The DWD's name scheme may change
     * any time, in which case this code becomes immediately obsolete
     * @param scan type
     * @param time_t utc timestamp
     * @return char* allocated null terminated string containing the guessed filename
     */
    char *RDGuessFilename(RDScanType type, time_t timestamp);


#ifdef __cplusplus
}
}
#endif

#endif /* Header Guard */