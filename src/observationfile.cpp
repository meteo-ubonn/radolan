
#include <radolan/observationfile.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>

// TODO: make this configurable

#define RAINRUNNER_ROOT				"/var/idry"

#define RAINRUNNER_OBSERVATIONS_DIR	"prediction"

#define RAINRUNNER_RADOLAN_ROOT		"/var/idry/radolan"

#define OBSERVATIONFILE_BASENAME	"observations"

#define OBSERVATION_GROUPNAME		"/observations"

#define MAX_MAPPED_SCANS			12

using namespace std;

namespace Radolan {

    RDObservationFile::RDObservationFile(RDScanType t)
    : m_scanType(t),
    m_coordinateSystem(RDCoordinateSystem(t)) {
        std::ostringstream fn;

        fn << RAINRUNNER_ROOT
                << "/" << RAINRUNNER_OBSERVATIONS_DIR
                << "/" << OBSERVATIONFILE_BASENAME << "-"
                << RDScanTypeToString(m_scanType)
                << ".h5" << ends;

        std::string filename = fn.str();

        ifstream f(filename.c_str());

        if (!f) {
            // create a new observations file

            cout << "Creating new observations file " << filename << endl;

            m_file = H5Fcreate(filename.c_str(), H5F_ACC_EXCL, H5P_DEFAULT, H5P_DEFAULT);

            // TODO: we're assuming reflectivity formats here. 
            // Perhaps it would be wiser to use the classes defined
            // for colors for example. Maybe the whole concept of
            // value classes needs re-evaluation?

            size_t width, height;

            RDGridSize(m_scanType, &width, &height);

            hsize_t dimsf[3] = {width, height, 256};

            hid_t dataspace = H5Screate_simple(3, dimsf, NULL);

            // we're counting observations, use integer

            hid_t datatype = H5Tcopy(H5T_NATIVE_INT);

            int status = H5Tset_order(datatype, H5T_ORDER_LE);

            // property list for fill value

            hid_t plist = H5Pcreate(H5P_DATASET_CREATE);

            int zeroValue = 0;

            status = H5Pset_fill_value(plist, datatype, &zeroValue);

            // create dataset

            m_dataset = H5Dcreate(m_file, OBSERVATION_GROUPNAME, datatype, dataspace, 0, plist, 0);

            H5Fflush(m_file, H5F_SCOPE_GLOBAL);

        } else {
            // if file exists, open for r/w access

            cout << "Attaching to observations file " << filename << endl;

            m_file = H5Fopen(filename.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);

            m_dataset = H5Dopen(m_file, OBSERVATION_GROUPNAME, 0);

        }

        if (!m_file) {
            cerr << "Critical Error: could not open observations file " << filename << endl;

            exit(-1);
        }

        m_dataspace = H5Dget_space(m_dataset);

    }

    RDObservationFile::~RDObservationFile() {
    }

    extern void convertToLowercase(char * s);

    void RDObservationFile::reportRain(float lon, float lat, long userID, long t) {
        cout << "reportRain lon=" << lon << " lat=" << lat << " id=" << userID << endl << " time=" << t << endl;

        RDGeographicalPoint coord = rdGeographicalPoint(lon, lat);

        bool isInside = false;

        RDGridPoint gp = m_coordinateSystem.gridPoint(coord, isInside);

        if (!isInside) {
            return;
        }

        // see if map has a scan for the timestamp, if not attempt to read it

        time_t timestamp = (time_t) t;

        map<time_t, RDScan*>::iterator iter = m_scans.find(timestamp);

        if (iter == m_scans.end()) {

            std::ostringstream path;

            char * typeString = strdup(RDScanTypeToString(m_scanType));

            convertToLowercase(typeString);

            char* scanFilename = RDGuessFilename(m_scanType, timestamp);

            path << RAINRUNNER_RADOLAN_ROOT << "/"
                    << typeString << "/"
                    << scanFilename
                    << ends;

            path.flush();

            cout << "Trying to read radolan scan " << path.str() << endl;

            RDScan* scan = RDAllocateScan();

            int status = RDReadScan(path.str().c_str(), scan, false);

            if (status == 0) {
                m_scans.insert(pair<time_t, RDScan*> (timestamp, scan));
            } else {
                RDFreeScan(scan);
            }

            // TODO: free up old scans



            // -> sort keys ascending
            // -> delete all but the MAX_MAPPED_SCANS last

        }

        iter = m_scans.find(timestamp);

        if (iter != m_scans.end()) {
            RDScan* scan = iter->second;

            RDDataType value = RDValueAt(scan, gp);

            if (RDIsCleanMeasurementAndNotMin(m_scanType, value)) {
                const unsigned char byteValue = RDRVP6ToByteValue(value);

                hsize_t point[1][3];

                point[0][0] = gp.ix;

                point[0][1] = gp.iy;

                point[0][2] = byteValue;

                H5Sselect_none(m_dataspace);

                H5Sselect_elements(m_dataspace, H5S_SELECT_SET, 1, (const hsize_t *) point);

                static hid_t memspace = -1;

                if (memspace == -1) {
                    hsize_t memspaceSize[3] = {1, 1, 1};

                    memspace = H5Screate_simple(3, memspaceSize, NULL);
                }

                int count[1];

                *count = -1;

                H5Dread(m_dataset, H5T_NATIVE_INT, memspace, m_dataspace, H5P_DEFAULT, count);

                cout << "Count for grid point (" << gp.ix << "," << gp.iy << ") [" << byteValue << "] = " << *count << endl;

                count[0]++;

                H5Dwrite(m_dataset, H5T_NATIVE_INT, memspace, m_dataspace, H5P_DEFAULT, count);

                H5Fflush(m_file, H5F_SCOPE_GLOBAL);
            }

        }
    }

    size_t RDObservationFile::observationCount(RDScan* scan, const RDGridPoint& gp) {
        hsize_t point[1][3];

        point[0][0] = gp.ix;

        point[0][1] = gp.iy;

        point[0][2] = RDRVP6ToByteValue(RDValueAt(scan, gp));

        H5Sselect_none(m_dataspace);

        H5Sselect_elements(m_dataspace, H5S_SELECT_SET, 1, (const hsize_t *) point);

        static hid_t memspace = -1;

        if (memspace == -1) {
            hsize_t memspaceSize[3] = {1, 1, 1};

            memspace = H5Screate_simple(3, memspaceSize, NULL);
        }

        int count[1];

        *count = -1;

        H5Dread(m_dataset, H5T_NATIVE_INT, memspace, m_dataspace, H5P_DEFAULT, count);

        return (size_t) *count;
    }


}
