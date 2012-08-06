/*
 *  radolan_to_cartesian.h
 *  RadolanConversion
 *
 *  Created by simon on 26.10.07.
 *  Copyright 2007 webtecc. All rights reserved.
 *
 */
#ifndef LIBRADOLAN_READER_H
#define LIBRADOLAN_READER_H

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
		int RDReadScan(const char* filename, RDScan* scan, _Bool ommitOutside );
		
		/** Reads only the header data from the given FILE*
		 * @param FILE*
		 * @param RDHeader* 
		 */
		void RDReadRadolanHeader(gzFile* f, RDRadolanHeader* header);
		
		/** Allocates a new instance of RDScan and sets its up correctly. Please
		 * use this method for allocating fresh scans in order to avoid problems 
		 * when deallocating. 
		 */
		RDScan* RDAllocateScan();
		
		/** Call this function to orderly free the scan data.
		 * @param RDScan*
		 */
		void RDFreeScan(RDScan* scan);
		
		/** Helper method. Allocates a new RDScan and clones content of the given original.
		 * @param RDScan* original
		 * @return RDScan* clone	(if NULL, some error occured while cloning)
		 */
		RDScan* RDCloneScan(RDScan* original);
		
#ifdef __cplusplus
	}
}
#endif

#endif /* header guard */
