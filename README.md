# radolan                                                                          
[//]: # {#mainpage}

This is a C++ library for reading and working with the RADOLAN data format of 
the German Weather Forecast Service (DWD). Also contains methods for converting 
to and from the RADOLAN coordinate system or grid, as well as conversion routines 
into NetCDF and Shapefile.

Information 
https://git.meteo.uni-bonn.de/attachments/download/2/RADOLAN_RADVOR_OP_Komposit_format_pdf.pdf

## Installation

### Dependencies
* boost
* libz
* hdf5
* netcdf4 (C++ bindings included)
* Shapelib (optional)

Use your platform specific package managers to install those dependencies. 
CMake will look for the dependencies in the usual locations (/usr or /usr/local).
If you install dependencies in different locations, please consider the CMAKE
documentation and adjust the file CMakeLists.txt accordingly.

### Installation

    git clone https://github.com/meteo-ubonn/radolan.git
    cd radolan
    cmake .
    make install

## Library and ececutables

The project produces a libradolan, which is installed to /usr/local/lib
together with it's headers at /usr/local/include. In addition, the following
executables are produced:

### radolan2netcdf
This command converts a RADOLAN file into a NetCDF file following the 
CF-Metadata convention (v1.6). See here: http://cfconventions.org/

### radolan2shapefile
If shapelib was detected during the cmake step, this executable is installed 
as well. It converts RADOLAN files into .shp files. You can choose between
polygons (radar 'pixels' are actually polygons) or points (center of the radar
pixel) and with- or without the scan value.

## How to use this
The following, simple example will read a radolan file, print out header information 
and a simple ASCII representation of the file to console:

    #include <radolan/radolan.h>
    #include <radolan/radolan_utils.h>
    
    using namespace Radolan;
    
    int main(int argc, char** argv)  {
        RDScan* scan = RDAllocateScan();
        if (RDReadScan(argv[1], scan, true)) {
            RDPrintHeaderInformation(scan);
            RDPrintScan(scan,20,20);
            RDFreeScan(scan);
        }
    }