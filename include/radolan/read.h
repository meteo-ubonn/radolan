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

#ifndef RADOLAN_READER_H
#define RADOLAN_READER_H

#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <zlib.h>

#include <radolan/types.h>

#ifdef __cplusplus
extern "C"
{
    namespace Radolan {
#endif

/** Reads the radolan data from the given file. The result is NOT converted.
 *
 * @param filename
 * @param radolan_scan allocted instance of the radolan data type
 * @param _Bool if true, the outside values have no representation (should be default)
 *
 * @return 0 if operation was successful
 *         -1 insufficient memory
 *         -2 file not found
 *         -3 io-error while reading file
 */

/**
 * Read in a radolan scan.
 *
 * @param path to file
 * @param scan pointer to (allocated) RDScan object.
 * @param ommitOutside Flag indicating if values that are outside of the RADOLAN's
 *                     area should be read or not. If the flag is <code>true</code>,
 *                     the values are <b>not</b> read. If the flag is <code>false</code>
 *                     those values are read. You can test for values outside by using
 *                     the RD_DBZ_OUTSIDEVALUE constant.
 */

int RDReadScan(const char *filename, RDScan *scan, bool ommitOutside);

/** Reads only the header data from the given FILE*
 * @param FILE*
 * @param RDHeader*
 */
void RDReadRadolanHeader(gzFile *f, RDRadolanHeader *header);

/** Allocates a new instance of RDScan and sets its up correctly. Please
 * use this method for allocating fresh scans in order to avoid problems
 * when deallocating.
 */
RDScan *RDAllocateScan();

/** Call this function to orderly free the scan data.
 * @param RDScan*
 */
void RDFreeScan(RDScan *scan);

/** Helper method. Allocates a new RDScan and clones content of the given original.
 * @param RDScan* original
 * @return RDScan* clone	(if NULL, some error occured while cloning)
 */
RDScan *RDCloneScan(RDScan *original);

#ifdef __cplusplus
}
}
#endif

#endif /* header guard */
