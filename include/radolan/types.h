/*
 *  radolan_types.h
 *  RadolanConversion
 *
 *  Created by simon on 19.11.07.
 *  Copyright 2007 webtecc. All rights reserved.
 *
 */
#ifndef LIBRADOLAN_TYPES_H
#define LIBRADOLAN_TYPES_H

#include <stdlib.h>

/* Prefix for Radolan constants, data types and methods is 'RD' */

#define RD_HEADER_LENGTH 138
#define RD_DIM_LON 900
#define RD_DIM_LAT	900

#define RD_SECONDARY_VALUE_BIT	0x01	// 0b0000000000000001
#define RD_ERROR_BIT			0x02	// 0b0000000000000010
#define RD_NEGATIVE_SIGN_BIT	0x04	// 0b0000000000000100
#define RD_CLUTTER_BIT			0x08	// 0b0000000000001000

#define RD_CLUTTER_VALUE -2490.0f
#define RD_ERROR_VALUE -2500.0f

#define RX_CLUTTER_VALUE 0xF9
#define RX_ERROR_VALUE 0xFA

#define RD_DBZ_BASEVALUE -32.5
#define RD_DBZ_OUTSIDEVALUE 92.5

#ifdef __cplusplus
extern "C"
{
	namespace Radolan {
#endif

    /** Scan type as denoted in Chapter 1.1 of the Radolan v2.1 spec. 
    */
    typedef enum {
        RD_UNKNOWN,
        RD_RX,
        RD_RO,
        RD_RK,
        RD_RZ,
        RD_RY,
        RD_RH,
        RD_RJ,
        RD_RP,
        RD_RT,
        RD_RC,
        RD_RI,
        RD_RG,
        RD_RB,
        RD_RA,
        RD_RM,
        RD_RL,
        RD_RN,
        RD_RD,
        RD_RF,
        RD_RW,
        RD_RU,
        RD_RR,
        RD_SQ,
        RD_SH,
        RD_SF,
        RD_RV,
        RD_RS,
        RD_RQ,
        RD_RE,
        RD_TZ,
        RD_TH,
        RD_EX,
        RD_EZ,
        RD_EH,
        RD_EB,
        RD_EW
    } RDScanType;

    /** \var typedef RDRadarFormat
     * Radar format as denoted in Chapter 1.1 of the Radolan v2.1 spec. 
     */
    typedef enum {R100km=1,R128km=2} RDRadarFormat;

    /** Quantification as denoted in Chapter 1.1 of the Radolan v2.1 spec. */
    typedef enum {RAVOQ_HV,RAVOQ_HV_ConfidenceEstimate,RAVOQ,Winterrath} RDQuantification;

    /** Contains header information as read from Radolan file. See Radolan spec v2.1 */
    typedef struct {

      /// Product ID (see Section 1, Radolan spec v2.1
      RDScanType scanType;                  

      // Date information
      unsigned short int day;               
      unsigned short int hour;
      unsigned short int minute;
      unsigned short int month;
      unsigned short int year;

      /// 100000 for composite (see Section 1, Radolan spec v2.1
      unsigned int radarLocation;           

      /// Number of bytes actually read from the header
      size_t headerSize;		      

      /// Tag <b>BY</b>. Limited to 4Gb. Corrected by subtracting the header size
      size_t payloadSize;		      

      /// Tag <b>VS</b>. 128km or 100km
      RDRadarFormat radarFormat;            

      /// Tag <b>SW</b>
      char softwareVersion[9];              

      /// Tag <b>PR</b> 1, 0.1, 0.01
      float precision;                      

      /// Tag <b>INT</b>. scan interval in minutes
      unsigned short int intervalDuration;  

      /// Tag <b>GP</b>. 900x900 in most cases
      char resolution[10];                  

      /// Tag <b>VV</b>when prediction product, time of prediction in minutes from scan time
      unsigned int predictionMinutes;       

      /// Tag <b>MF</b>. 
      char binaryFormat[9];                 

      /// Tag <b>QN</b>.
      RDQuantification quantification;      

      /// Tag <b>MS</b>. Number of radar stations
      int numberOfRadarStations;            

      /// Tag <b>MS</b>. List of radar station id's (c-strings)
      char* radarStations;                  

    } RDRadolanHeader;

    /* Which data type to use in the reading process. If you change this,
     the reader code must reflect this change */
    typedef float RDDataType;

    /** Data type for reading and handling radolan products. When read from the
     * radolan file directly, the header contains information. When read from GeoTIFF, 
     * the header will be empty. Use RDAllocate and RDFree to create and dispose of instances.
     */
    typedef struct {

      /// Name of file containing data when read
      char filename[1024];		      

      /// Header info (when read from radolan binary format)
      RDRadolanHeader header;                        

      /// Array of rd_data[header.payloadSize]
      RDDataType* data;                        

      /// Number of longitudinal vertices
      int dimLon;								

      /// Number of latitudinal vertices
      int dimLat;				

      /// Minimum value found in the actual data
      RDDataType min_value;                    

      /// Maximum value found in the actual data
      RDDataType max_value;                    
      
      /// Conversion factor from RVP6 units to DBZ
      double dbZPerUnit;

    } RDScan;

#ifdef __cplusplus
	}
}
#endif
		
#endif /* header guard */
