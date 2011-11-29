#include <radolan/radolan_utils.h>

#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <ctype.h>

#ifdef __cplusplus
namespace Radolan {
#endif
	
#define RVP6_BASE_VALUE -32.5f
	
	RDDataType RDMinValue(RDScanType t)
	{
		switch(t)
		{
			case RD_RX:
			case RD_EX:
				return RVP6_BASE_VALUE;
				break;
			default:
				return 0.0;
		};
	}
	
	RDDataType RDMaxValue(RDScanType t)
	{
		switch(t)
		{
			case RD_RX:
			case RD_EX:
				return 95.0;
				break; 
			default:
				return 4095;
		};
	}
	
	RDDataType RDMissingValue(RDScanType t)
	{
		switch(t)
		{
			case RD_RX:
			case RD_EX:
				return RD_DBZ_OUTSIDEVALUE;
				break;
			default:
				return RD_ERROR_VALUE;
		};
	}
	
	RDDataType RDClutterValue(RDScanType t)
	{
		switch(t)
		{
			case RD_RX:
			case RD_EX:
				return 92.0f;
				break;
			default:
				return RD_CLUTTER_VALUE;
		};
	}
	
	int RDIsCleanMeasurement(RDScanType t, RDDataType value)
	{
        bool notMin = value >= RDMinValue(t);
        
        bool notMax = value <= RDMaxValue(t);
        
        bool notMissing = value != RDMissingValue(t);
        
        bool notClutter = value != RDClutterValue(t);
        
		return ( notMin && notMax && notMissing && notClutter );
	}
	
	int RDIsCleanMeasurementAndNotMin(RDScanType t, RDDataType value)
	{
        bool notMin = value > RDMinValue(t);
        
        bool notMax = value <= RDMaxValue(t);
        
        bool notMissing = value != RDMissingValue(t);
        
        bool notClutter = value != RDClutterValue(t);
        
		return ( notMin && notMax && notMissing && notClutter );
	}
	
	float RDRainrateFromDezibels(RDDataType dezibels)
	{
		return powf(dezibels/256.0f,1.0f/1.42f);
	}
	
	unsigned char RDRVP6ToByteValue( float rvp6 )
	{
		return (unsigned char) 2u * (rvp6 - RVP6_BASE_VALUE);
	}
	
	float RDByteToRVP6Value( const unsigned char byteValue )
	{
		return RVP6_BASE_VALUE + ((float)byteValue) / 2.0f;
	}
	
	void RDScanTime(RDScan* scan, struct tm * t)
	{
	  // find out if Daylight Saving Time is on
	  // by obtaining local time

	  time_t now;
	  
	  time( &now );

//	  struct tm nowPtr = * localtime( (const time_t*) &now );

	  // initialize the scan time
	  gmtime_r( (const time_t * ) &now, t );

	  // construct the server timestamp @ GMT with DST set up correctly
	  t->tm_sec = 0;
	  t->tm_min = scan->header.minute;
	  t->tm_hour = scan->header.hour;
	  t->tm_mday = scan->header.day;
	  t->tm_mon = scan->header.month;
	  
	  //printf("SCAN HEADER MONTH = %d\n",scan->header.month );

	  t->tm_year = scan->header.year + 100; // man mktime
	}
	
	size_t RDBytesPerPixel(RDScanType type)
	{
		switch (type) 
		{
			case RD_RX: 
			case RD_EX:
				return 1;
				break;
			default:
				return 2;
		}
	}
	
	float RDMMPerHour(RDScanType t, RDDataType value)
	{
		float result = (float)value;
		switch (t) {
			case RD_RX:
			case RD_EX:
				result = -INFINITY;
				break;
			case RD_RZ:
			case RD_RY:
			case RD_RV:	
			case RD_EZ:
				// 1/100 mm / 5 min
				result = value * 12.0f / 100.0f;
				break;
			case RD_RH:
			case RD_RB:
			case RD_RW:
			case RD_RL:
			case RD_RU:
			case RD_RS:
			case RD_RQ:
				// 1/10 mm / h
				result /= 10.0f;
				break;
			case RD_SQ:
				// 1/10 mm / 6h
				result = value / 10.0f;
				break;
			case RD_SH:
				// 1/10 mm / 12h
				result = value / 10.0f;
				break;
			case RD_SF:
				// 1/10 mm / 24h
				break;
			default:
				result = value;
				break;
		}
		return result;
	}
	
	const char* RDScanTypeToString(RDScanType type) 
	{
		switch (type) 
		{
			case RD_RX: return "RX"; break;
			case RD_RO: return "RO"; break;
			case RD_RK: return "RK"; break;
			case RD_RZ: return "RZ"; break;
			case RD_RY: return "RY"; break;
			case RD_RH: return "RH"; break;
			case RD_RJ: return "RJ"; break;
			case RD_RP: return "RP"; break;
			case RD_RT: return "RT"; break;
			case RD_RC: return "RC"; break;
			case RD_RI: return "RI"; break;
			case RD_RG: return "RG"; break;
			case RD_RB: return "RB"; break;
			case RD_RA: return "RA"; break;
			case RD_RM: return "RM"; break;
			case RD_RL: return "RL"; break;
			case RD_RN: return "RN"; break;
			case RD_RD: return "RD"; break;
			case RD_RF: return "RF"; break;
			case RD_RW: return "RW"; break;
			case RD_RU: return "RU"; break;
			case RD_RR: return "RR"; break;
			case RD_SQ: return "SQ"; break;
			case RD_SH: return "SH"; break;
			case RD_SF: return "SF"; break;
			case RD_RV: return "RV"; break;
			case RD_RS: return "RS"; break;
			case RD_RQ: return "RQ"; break;
			case RD_TZ: return "TZ"; break;
			case RD_TH: return "TH"; break;
	        case RD_EX: return "EX"; break;
			case RD_EZ: return "EZ"; break;
			case RD_EH: return "EH"; break;
			case RD_EB: return "EB"; break;
			case RD_EW: return "EW"; break;
			default: 	
				return "UNKNOWN";
		}
	}
	
	RDScanType RDScanTypeFromString(const char* str)
	{
		RDScanType type = RD_UNKNOWN;
		if (strcmp(str,"RX")==0) {
			type = RD_RX;
		} else if (strcmp(str,"RO")==0) {
			type = RD_RO;
		} else if (strcmp(str,"RK")==0) {
			type = RD_RK;
		} else if (strcmp(str,"RZ")==0) {
			type = RD_RZ;
		} else if (strcmp(str,"RX")==0) {
			type = RD_RX;
		} else if (strcmp(str,"RY")==0) {
			type = RD_RY;
		} else if (strcmp(str,"RH")==0) {
			type = RD_RH;
		} else if (strcmp(str,"RJ")==0) {
			type = RD_RJ;
		} else if (strcmp(str,"RP")==0) {
			type = RD_RP;
		} else if (strcmp(str,"RT")==0) {
			type = RD_RT;
		} else if (strcmp(str,"RC")==0) {
			type = RD_RC;
		} else if (strcmp(str,"RI")==0) {
			type = RD_RI;
		} else if (strcmp(str,"RG")==0) {
			type = RD_RG;
		} else if (strcmp(str,"RB")==0) {
			type = RD_RB;
		} else if (strcmp(str,"RA")==0) {
			type = RD_RA;
		} else if (strcmp(str,"RM")==0) {
			type = RD_RM;
		} else if (strcmp(str,"RL")==0) {
			type = RD_RL;
		} else if (strcmp(str,"RN")==0) {
			type = RD_RN;
		} else if (strcmp(str,"RD")==0) {
			type = RD_RD;
		} else if (strcmp(str,"RF")==0) {
			type = RD_RF;
		} else if (strcmp(str,"RW")==0) {
			type = RD_RW;
		} else if (strcmp(str,"RU")==0) {
			type = RD_RU;
		} else if (strcmp(str,"RR")==0) {
			type = RD_RR;
		} else if (strcmp(str,"SQ")==0) {
			type = RD_SQ;
		} else if (strcmp(str,"SH")==0) {
			type = RD_SH;
		} else if (strcmp(str,"SF")==0) {
			type = RD_SF;
		} else if (strcmp(str,"RV")==0) {
			type = RD_RV;
		} else if (strcmp(str,"RS")==0) {
			type = RD_RS;
		} else if (strcmp(str,"RQ")==0) {
			type = RD_RQ;
		} else if (strcmp(str,"TZ")==0) {
			type = RD_TZ;
		} else if (strcmp(str,"TH")==0) {
			type = RD_TH;
		} else if (strcmp(str,"EX")==0) {
			type = RD_EX;
		} else if (strcmp(str,"EZ")==0) {
			type = RD_EZ;
		} else if (strcmp(str,"EH")==0) {
			type = RD_EH;
		} else if (strcmp(str,"EB")==0) {
			type = RD_EB;
		} else if (strcmp(str,"EW")==0) {
			type = RD_EW;
		}
		return type;
	}
	
	void RDGridSize( RDScanType t, size_t* width, size_t* height )
	{
		switch ( t ) 
		{
			case RD_RX: 
			case RD_RO: 
			case RD_RK: 
			case RD_RZ: 
			case RD_RY: 
			case RD_RH: 
			case RD_RJ: 
			case RD_RP: 
			case RD_RT: 
			case RD_RC: 
			case RD_RI: 
			case RD_RG: 
			case RD_RB: 
			case RD_RA: 
			case RD_RM: 
			case RD_RL: 
			case RD_RN: 
			case RD_RD: 
			case RD_RF: 
			case RD_RW: 
			case RD_RU: 
			case RD_RR: 
			case RD_SQ:
			case RD_SH:
			case RD_SF:
			case RD_RV:
			case RD_RS:
			case RD_RQ:
			case RD_TZ:
			case RD_TH:
				
				*width = 900;
				
				*height = 900;
				
				break;
				
	        case RD_EX:
			case RD_EZ:
			case RD_EH:
			case RD_EB:
			case RD_EW:
				
				*width = 1500;
				
				*height = 1400;
				
				break;
				
			case RD_UNKNOWN:
				
				*width = -1;
				
				*height = -1;
		}
	}
	
	void RDPrintHeaderInformation(RDScan* scan) 
	{
		// TODO: Extend
		printf("\nHeader of file %s\n",scan->filename);
		RDRadolanHeader h = scan->header;
		printf("Scan Type: %s\n",RDScanTypeToString(h.scanType));
		printf("Zeitstempel %02i:%02i GMT %02i.%02i.%02i\n",h.hour,h.minute,h.day,h.month,h.year);
		printf("Precision : %5.4f\n",h.precision);
		printf("Min value : %f Max value : %f\n",scan->min_value,scan->max_value);
		printf("Header size %li , payload size %li\n",h.headerSize,h.payloadSize);
		printf("Stations %s\n",h.radarStations);
		
	}
	
	void RDPrintScan(RDScan* scan, int latCount, int lonCount) 
	{
		int lat,lon;
        
		for (lat=0; lat<scan->dimLat; lat++) 
		{
			if (lat%latCount==0) 
			{
				for (lon=0; lon<scan->dimLon; lon++) {
					if (lon%lonCount==0) 
					{
						float value = scan->data[lat*scan->dimLon+lon];
						printf(RDIsCleanMeasurementAndNotMin(scan->header.scanType, value) ? "*":" ");
					}
				}
				printf("\n");
			}
		}
	}
	
	void convertToLowercase( char* s )
	{
		size_t len = strlen( s );
		
		for ( size_t i=0; i < len; i++ )
		{
			char c = s[i];
			
			s[i] = tolower( c );
		}
	} 
	
	char* RDGuessFilename( RDScanType type, time_t timestamp )
	{
		// raa01-rx_10000-0909030000-dwd---bin
		
		char fn[1024];
		
		fn[0] = '\0';
		
		// get string for scan type and convert to lower case
		
		char* typeString = strdup( RDScanTypeToString( type ) ); 
		
		convertToLowercase( typeString );
		
		// now for the time pokery
		
		struct tm timeComponents;
		
		gmtime_r( &timestamp, &timeComponents );
		
		char timeString[32];
		
		timeString[0] = '\0';
		
		// create our guess
		
		sprintf( fn,
				"raa01-%s_10000-%02d%02d%02d%02d%02d-dwd---bin",
				typeString,
				timeComponents.tm_year - 100,
				timeComponents.tm_mon,
				timeComponents.tm_mday,
				timeComponents.tm_hour,
				timeComponents.tm_min );
		
		free( typeString );
		
		return strdup( fn );
	}
	
	
	
#ifdef __cplusplus
}
#endif
