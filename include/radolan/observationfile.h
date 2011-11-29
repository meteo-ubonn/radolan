#ifndef __RADOLAN_OBSERVATIONFILE__
#define __RADOLAN_OBSERVATIONFILE__

#include <radolan/radolan.h>

#include <hdf5.h>

#include <map>

namespace Radolan
{
	
	class RDObservationFile
		{
			
		public:
			
			RDObservationFile();
			
			RDObservationFile( RDScanType t );
			
			virtual ~RDObservationFile();
			
			void reportRain( float lat, float lon, long userID, long timestamp );
			
			RDScan* scanForTimestamp( time_t timestamp );
			
			size_t observationCount( RDScan* scan, const RDGridPoint& gridPoint );
			
		private:
			
			hid_t				m_file;
			
			hid_t				m_dataset;
			
			hid_t				m_dataspace;
			
			RDScanType			m_scanType;
			
			RDCoordinateSystem	m_coordinateSystem;
			
			std::map< time_t, RDScan* >	m_scans;
			
		};
	
}


#endif __RADOLAN_OBSERVATIONFILE__

