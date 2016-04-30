#include <netcdf>
#include <iostream>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <radolan/radolan.h>

using namespace std;
using namespace Radolan;

using namespace boost;

int main(int argc, char **argv) {

    namespace fs = boost::filesystem;

    try {
        program_options::options_description desc("Options");
        desc.add_options()
                ("help,h", "Show this message and exit.")
                ("version", "Print version information and exit.")
                ("endianess", "Print out the system's endianess and exit.")
                ("file,f", program_options::value<string>(),
                 "Radolan filename or directory containing radolan scans")
                ("output-dir,o", program_options::value<string>()->default_value("."),
                 "Output directory to write resulting files to. Defaults to current directory.")
                ("bounds", "Write out a shapefile containing the bounding box")
                ("points,p", "Convert to SHPT_MULTIPOINTM (or SHPT_POINT if --no-values is given) instead of polygons")
                ("geographical,g", "Use lat/lon (geographical) instead of polar-stereographic (cartesian)")
                ("no-values,n", "Write out simple shapes without values (SHPT_POINT/SHPT_POLYGON)");

        program_options::variables_map vm;
        try {
            program_options::store(program_options::parse_command_line(argc, argv, desc), vm);
            program_options::notify(vm);
        } catch (std::exception &e) {
            cerr << "ERROR:could not parse command line:" << e.what() << endl;
            exit(EXIT_FAILURE);
        }

        if (vm.count("version") != 0) {
            cout << Radolan::VERSION << endl;
            exit(EXIT_SUCCESS);
        }

        if (vm.count("endianess") != 0) {
            cout << "Endianess: " << (isLittleEndian() ? "little" : "big") << endl;
            exit(EXIT_SUCCESS);
        }

        if (vm.count("help") != 0 || argc < 2 || vm.count("file") == 0) {
            cout << desc << "\n";
            exit(EXIT_SUCCESS);
        }

        // Write as points or polygons?
        bool writePoints = vm.count("points") > 0;

        // Write bounding box file?
        bool writeBoundingBox = vm.count("bounds") > 0;

        // Geographical?
        bool geographical = vm.count("geographical") > 0;

        // With values?
        bool withValues = vm.count("no-values") == 0;

        string infile = vm["file"].as<string>();

        // File or Directory?
        std::string ending("---bin");
        fs::path path(infile);

        fs::directory_iterator end_iter;
        vector<std::string> file_paths;

        if (fs::exists(path)) {
            if (fs::is_directory(path)) {
                for (fs::directory_iterator dir_iter(path); dir_iter != end_iter; ++dir_iter) {
                    if (fs::is_regular_file(dir_iter->status())) {
                        std::string fn = dir_iter->path().generic_string();
                        if (0 == fn.compare(fn.length() - ending.length(), ending.length(), ending)) {
                            file_paths.push_back(fn);
                        }
                    }
                }
            } else {
                std::string fn = path.generic_string();
                file_paths.push_back(fn);
            }
        } else {
            cerr << "FATAL:File or path does not exist: " << infile << endl;
            exit(EXIT_FAILURE);
        }

        boost::filesystem::path outpath(vm["output-dir"].as<std::string>());

        if (!boost::filesystem::exists(outpath) || !boost::filesystem::is_directory(outpath)) {
            cerr << "FATAL:Can't write to path " << outpath << endl;
            exit(EXIT_FAILURE);
        }

        // Bounding box?
        if (writeBoundingBox) {
            try {
            } catch (RDConversionException &e) {

            }
        }

        if (file_paths.empty()) {
            cout << "No RADOLAN files found." << endl;
            exit(EXIT_SUCCESS);
        }

        vector<std::string>::iterator fi;
        for (fi = file_paths.begin(); fi != file_paths.end(); fi++) {
            std::string fn = *fi;

            RDScan *scan = RDAllocateScan();
            if (!RDReadScan(fn.c_str(), scan, true)) {
                cerr << "ERROR:could read open RADOLAN file " << fn << endl;
                continue;
            }

            try {
                if (writeBoundingBox) {
                    boost::filesystem::path boxpath = outpath;
                    boxpath /= boost::filesystem::path(fn).filename();
                    boxpath /= "-bounding-box.shp";

                    cout << "Writing out bounding box of file " << fn << " ...";
                    Radolan2Shapefile::writeBoundingBox(scan, boxpath.generic_string().c_str());
                    cout << "done." << endl;
                }

                boost::filesystem::path path = outpath;
                path /= boost::filesystem::path(fn).filename();
                path += ".shp";

                cout << "Converting " << fn << " to " << path.generic_string() << " ...";
                if (writePoints) {
                    Radolan2Shapefile::convertToPoints(scan, path.generic_string().c_str(), geographical, withValues);
                } else {
                    Radolan2Shapefile::convertToPolygons(scan, path.generic_string().c_str(), geographical, withValues);
                }
                cout << " done." << endl;

            } catch (RDConversionException &e) {
                cerr << endl << "ERROR:" << e.what() << endl;
            }

            RDFreeScan(scan);
        }

    } catch (const std::exception &e) {
        cerr << "FATAL:exception: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
