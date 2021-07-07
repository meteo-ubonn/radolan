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

#include <string.h>

#include <radolan/read.h>
#include <radolan/radolan_utils.h>
#include <radolan/endianess.h>
#include <radolan/types.h>

#ifdef __cplusplus
namespace Radolan
{
#endif

_Bool readUnsignedInt(gzFile *f, int len, unsigned int *value) {
    char buf[len + 1];
    int objectsRead = gzread(f, &buf[0], len);
    buf[len] = '\0';
    if (objectsRead == 1) {
        *value = (unsigned int) atoi(buf);
        return true;
    } else {
        return false;
    }
}

_Bool readUnsignedShortInt(gzFile *f, int len, unsigned short int *value) {
    char buf[len + 1];
    int bytesRead = gzread(f, &buf[0], len);
    buf[len] = '\0';
    if (bytesRead == len) {
        *value = (unsigned short int) atoi(buf);
        return true;
    } else {
        return false;
    }
}

_Bool readInt(gzFile *f, int len, int *value) {
    char buf[len + 1];
    int bytesRead = gzread(f, &buf[0], len);
    buf[len] = '\0';
    if (bytesRead == len) {
        *value = atoi(buf);
        return true;
    } else {
        return false;
    }
}

_Bool readLong(gzFile *f, int len, long *value) {
    char buf[len + 1];
    int bytesRead = gzread(f, &buf[0], len);
    buf[len] = '\0';
    if (bytesRead == len) {
        *value = atol(buf);
        return true;
    } else {
        return false;
    }
}

void RDReadRadolanHeader(gzFile *f, RDRadolanHeader *header) {

    // just a char buffer used throughout the function to store data temporarily
    char valueBuffer[80];

    // a few pieces of information are always in the same place
    // at the start of the header
    gzread(f, valueBuffer, 2);
    valueBuffer[2] = '\0';
    header->scanType = RDScanTypeFromString(valueBuffer);

    readUnsignedShortInt(f, 2, &header->day);
    readUnsignedShortInt(f, 2, &header->hour);
    readUnsignedShortInt(f, 2, &header->minute);
    readUnsignedInt(f, 5, &header->radarLocation);
    readUnsignedShortInt(f, 2, &header->month);
    readUnsignedShortInt(f, 2, &header->year);

    //		printf("\nSCAN TIME HEADER : %d:%d %d.%d.%d\n\n",header->hour,header->minute, header->day, header->month, header->year);

    // the can vary its location and is tagged. read the rest into a sufficiently
    // large buffer, null-terminate and process

    char buffer[250];
    gzread(f, &buffer[0], 250);
    buffer[249] = '\0';

    // header tokens according to spec
    const char *tokenList[10] = {"BY", "VS", "SW", "PR", "INT", "GP", "VV", "MF", "QN", "MS"};
    int i, j, bytesRead = 0;

    for (i = 0; i < 10; i++) {

        char *pos = strstr(buffer, tokenList[i]);
        if (pos != NULL) {

            // skip over token itself
            int tokenLen = strlen(tokenList[i]);
            for (j = 0; j < tokenLen; j++) {
                pos++;
            }

            // read content
            if (strcmp(tokenList[i], "BY") == 0) {
                strncpy(valueBuffer, pos, 7);
                valueBuffer[7] = '\0';
                header->payloadSize = atol(valueBuffer);
                bytesRead += 9;
            }
            else if (strcmp(tokenList[i], "VS") == 0) {
                strncpy(valueBuffer, pos, 2);
                valueBuffer[2] = '\0';
                int val = atoi(valueBuffer);
                if (val == 1) {
                    header->radarFormat = R100km;
                } else if (val == 2) {
                    header->radarFormat = R128km;
                } else if (val == 3) {
                    header->radarFormat = R150km;
                }
                bytesRead += 4;
            }
            else if (strcmp(tokenList[i], "SW") == 0) {
                strncpy(&header->softwareVersion[0], pos, 8);
                header->softwareVersion[8] = '\0';
                bytesRead += 10;
            }
            else if (strcmp(tokenList[i], "PR") == 0) {
                if (strncmp(pos, " E-00", 5) == 0 || strncmp(pos, " E+00", 5) == 0) {
                    header->precision = 1.0f;
                }
                else if (strncmp(pos, " E-01", 5) == 0) {
                    header->precision = 0.1f;
                }
                else if (strncmp(pos, " E-02", 5) == 0) {
                    header->precision = 0.01f;
                }
                bytesRead += 6;
            }
            else if (strcmp(tokenList[i], "INT") == 0) {
                strncpy(valueBuffer, pos, 4);
                valueBuffer[4] = '\0';
                header->intervalDuration = (unsigned short int) atoi(valueBuffer);
                bytesRead += 7;
            }
            else if (strcmp(tokenList[i], "GP") == 0) {
                strncpy(&header->resolution[0], pos, 9);
                header->resolution[9] = '\0';
                bytesRead += 11;
            }
            else if (strcmp(tokenList[i], "VV") == 0) {
                strncpy(valueBuffer, pos, 3);
                valueBuffer[3] = '\0';
                header->predictionMinutes = (unsigned int) atoi(valueBuffer);
                bytesRead += 5;
            }
            else if (strcmp(tokenList[i], "MF") == 0) {
                strncpy(&header->resolution[0], pos, 8);
                header->binaryFormat[8] = '\0';
                bytesRead += 10;
            }
            else if (strcmp(tokenList[i], "QN") == 0) {
                if (strncmp(pos, "001", 4) == 0) {
                    header->quantification = RAVOQ_HV;
                }
                else if (strncmp(pos, "002", 4) == 0) {
                    header->quantification = RAVOQ_HV_ConfidenceEstimate;
                }
                else if (strncmp(pos, "003", 4) == 0) {
                    header->quantification = RAVOQ;
                }
                else if (strncmp(pos, "004", 4) == 0) {
                    header->quantification = Winterrath;
                }
                bytesRead += 6;
            }
            else if (strcmp(tokenList[i], "MS") == 0) {
                strncpy(valueBuffer, pos, 3);
                valueBuffer[3] = '\0';
                int len = atoi(valueBuffer);
                header->radarStations = (char *) calloc(len, sizeof(char));
                strncpy(header->radarStations, (pos + 3), len);
                bytesRead += (5 + len);
            }
        }
    }

    // figure out the real header and payload size
    size_t realPayloadSize = 0;
    switch (header->scanType) {
        case RD_TZ:
        case RD_TH:
        case RD_EX:
        case RD_EZ:
        case RD_EW:
            realPayloadSize = 1500 * 1400 * RDBytesPerPixel(header->scanType);
            break;
        case RD_FZ:
            // For a change these match
            realPayloadSize = header->payloadSize;
            break;
        default:
            realPayloadSize = 900 * 900 * RDBytesPerPixel(header->scanType);
    }
    header->headerSize = header->payloadSize - realPayloadSize;
    header->payloadSize = realPayloadSize;

    gzrewind(f);
    gzseek(f, header->headerSize, 0);
}

int RDReadScan(const char *filename, RDScan *scan, bool ommitOutside) {
    gzFile *f = gzopen(filename, "r");
    if (f != NULL) {
        if (scan == NULL) {
            fprintf(stderr, "RDReadScan : WARNING : scan is NULL, allocating new instance ...\n");
            scan = (RDScan *) calloc(sizeof(RDScan), 1);
            if (scan == NULL) {
                fprintf(stderr, "RDReadScan: ERROR : out of memory\n");
                return 0;
            }
        }

        // store the filename
        strcpy(scan->filename, filename);

        // read the header information
        RDReadRadolanHeader(f, &scan->header);

        // figure out the resolution lat x lon
        switch (scan->header.scanType) {
            case RD_TZ:
            case RD_TH:
            case RD_EX:
            case RD_EZ:
            case RD_EW:
                scan->dimLat = 1500;
                scan->dimLon = 1400;
                break;
            case RD_FZ:
                scan->dimLat = 450;
                scan->dimLon = 450;
                break;
            default:
                scan->dimLat = 900;
                scan->dimLon = 900;
        }

        // allocate sufficient block for the actual data
        // the actual number of vertices is payload size / 2 (data is 16bit word, 4 flag, 12 data = 2 bytes)
        scan->data = (RDDataType *) calloc(scan->dimLat * scan->dimLon, sizeof(RDDataType));

        if (scan->data == NULL) {
            fprintf(stderr, "RDReadScan : ERROR : could not allocate data buffer : out of memory\n");
            return 0;
        }

        switch (scan->header.scanType) {
            case RD_RX:
            case RD_EX: {
                // 8 Byte encoding
                scan->min_value = RDMinValue(scan->header.scanType); //dbZ
                scan->max_value = RDMaxValue(scan->header.scanType); //dbZ

                // allocate a large enough buffer for the 8bit binary data chunks from radolan
                unsigned char *buffer = (unsigned char *) calloc(scan->dimLon * scan->dimLat, sizeof(unsigned char));

                if (buffer == NULL) {
                    fprintf(stderr, "RDReadScan : ERROR : could not allocate data buffer : out of memory\n");
                    return 0;
                }

                // since August 11 2009 some problem with radolan header. It's missing 2 bytes!

                // START WORKAROUND
                size_t pos = gztell(f);

                //					gzrewind(f);
                //					size_t fileSize = gzread(f, buffer, -1);
                //					size_t nominalSize = scan->header.payloadSize + scan->header.headerSize;
                //					if (fileSize < nominalSize )
                //					{
                //						fprintf(stdout,"RDReadScan : ERROR : header payload size exceeds file size by %ld. Attempting to compensate\n", nominalSize-fileSize);
                //						pos = pos - (nominalSize - fileSize);
                //					}
                //					gzseek( f, pos, SEEK_SET );
                // since August 11 2009 some problem with radolan header. It's missing 2 bytes!
                // END WORKAROUND

                // read data into buffer
                int bytesRead = gzread(f, buffer, scan->header.payloadSize);

                if (bytesRead == -1) {
                    fprintf(stderr, "RDReadScan : ERROR : could not read %lu bytes from position %zuld.",
                            scan->header.payloadSize, pos);
                    return 0;
                }
                else if (bytesRead != scan->header.payloadSize) {
                    fprintf(stderr, "RDReadScan : ERROR : payload size wrong. File corrupt?\n");
                    return 0;
                }

                // All data read. Close file.
                gzclose(f);

                // massage the data. Discard information on clutter, errors and secondary values
                int bufferIndex = 0;
                for (bufferIndex = 0; bufferIndex < scan->header.payloadSize; bufferIndex++) {
                    unsigned char rvp6Value = buffer[bufferIndex];

                    // convert RVP6-Units to reflectivity
                    switch (rvp6Value) {
                        case 249:   // Clutter
                        case 250:   // Fehlkennung
                        {
                            if (ommitOutside) {
                                scan->data[bufferIndex] = RDMinValue(scan->header.scanType);
                            } else {
                                scan->data[bufferIndex] = RDMinValue(scan->header.scanType) + (rvp6Value / 2.0);
                            }
                        } break;

                        default:
                            scan->data[bufferIndex] = RDMinValue(scan->header.scanType) + (rvp6Value / 2.0);
                    }

                    // update min/max
                    if (scan->data[bufferIndex] > scan->max_value) scan->max_value = scan->data[bufferIndex];
                    else if (scan->data[bufferIndex] < scan->min_value) scan->min_value = scan->data[bufferIndex];
                }
            } break;

            default: {
                // 16 bit encoding (little endian)
                scan->min_value = RDMinValue(scan->header.scanType); //dbZ
                scan->max_value = RDMaxValue(scan->header.scanType); //dbZ

                // allocate a large enough buffer for the 16bit binary data chunks from radolan
                unsigned short int *buffer = (unsigned short int *)
                        calloc(scan->dimLon * scan->dimLat, sizeof(unsigned short int));

                if (buffer == NULL) {
                    fprintf(stderr, "RDReadScan : ERROR : could not allocate data buffer : out of memory\n");
                    return 0;
                }

                // read data into buffer
                int bytesRead = gzread(f, buffer, scan->header.payloadSize);
                if (bytesRead != scan->header.payloadSize) {
                    fprintf(stderr, "RDReadScan : ERROR : payload too small. File corrupt?\n");
                    return 0;
                }

                // All data read. Close file.
                gzclose(f);

                int rawMin = 4095;
                int rawMax = 0;

                // massage the data. Discard information on clutter, errors and replace secondary values
                int bufferIndex = 0;

                bool is_little_endian = isLittleEndian();

                for (bufferIndex = 0; bufferIndex < scan->dimLon * scan->dimLat; bufferIndex++) {

                    unsigned short int rawBufferValue = buffer[bufferIndex];
                    unsigned short int bufferValue = rawBufferValue;

                    if (!is_little_endian) {
                        // change from little endian to big endian
                        bufferValue = ((rawBufferValue >> 8) & 0xff) + ((rawBufferValue << 8) & 0xff00);
                    }

                    // in order to get to the 4 highest bits, shift the 16 bit value 12 bits to the right.
                    // the flags are then 0001 0010 etc.
                    unsigned char flagValue = bufferValue >> 12;

                    // in order to separate the payload value, shift 4 to the left to drop the flag value
                    // then shift back.
                    unsigned short int beef = (bufferValue << 4);
                    beef = beef >> 4;

                    // calculate rain rate from header->precision
                    float rainValue = scan->header.precision * (float) beef;

                    _Bool errorBitSet = (flagValue & RD_ERROR_BIT) == RD_ERROR_BIT;
                    _Bool clutterBitSet = (flagValue & RD_CLUTTER_BIT) == RD_CLUTTER_BIT;
                    _Bool secondaryValueBitSet = (flagValue & RD_SECONDARY_VALUE_BIT) == RD_SECONDARY_VALUE_BIT;
                    _Bool negativeSignBitSet = (flagValue & RD_NEGATIVE_SIGN_BIT) == RD_NEGATIVE_SIGN_BIT;

                    if (clutterBitSet) {
                        // There is some obvious confusion in the specifications as to the meaning of
                        // this bit. In praxi it turns out, that this value is used for error as well
                        // as clutter. 

                        // Perhaps we need to check in the value given matches the value
                        // for clutter. If so, we use the RD_ERROR_VALUE, if not we use the rain value?

                        if (beef != RD_CLUTTER_VALUE) {
                            if (ommitOutside) {
                                scan->data[bufferIndex] = RDMinValue(scan->header.scanType);
                            } else {
                                scan->data[bufferIndex] = RD_ERROR_VALUE;
                            }
                        }
                        else {
                            scan->data[bufferIndex] = RDMaxValue(scan->header.scanType);
                        }
                    }

                    else if (errorBitSet) {
                        scan->data[bufferIndex] = rainValue;
                    }

                    else if (secondaryValueBitSet) {
                        // replaced by secondary value. At this point, we don't 
                        // really care about where the value comes from
                        scan->data[bufferIndex] = rainValue;
                    }

                    else if (negativeSignBitSet) {
                        // this is only important with RD product
                        scan->data[bufferIndex] = -rainValue;

                        // update min/max
                        if (scan->data[bufferIndex] > scan->max_value) scan->max_value = scan->data[bufferIndex];
                        else if (scan->data[bufferIndex] < scan->min_value) scan->min_value = scan->data[bufferIndex];
                    }

                    else {
                        scan->data[bufferIndex] = rainValue;

                        // update min/max
                        if (scan->data[bufferIndex] > scan->max_value) scan->max_value = scan->data[bufferIndex];
                        else if (scan->data[bufferIndex] < scan->min_value) scan->min_value = scan->data[bufferIndex];

                        if (beef < rawMin) rawMin = beef;
                        if (beef > rawMax) rawMax = beef;
                    }
                }
            }
        }  // switch scan type

    } else {
        fprintf(stderr, "RDReadScan : ERROR : could not open file %s", filename);
        return 0;
    }

    return 1;
}

RDScan *RDAllocateScan() {
    RDScan *scan = (RDScan *) malloc(sizeof(RDScan));
    if (scan == NULL) {
        fprintf(stderr, "RDAllocateScan : ERROR : out of memory\n");
        return NULL;
    }
    scan->header.radarStations = NULL;
    scan->min_value = 4095;
    scan->max_value = 0;
    return scan;
}


void RDFreeScan(RDScan *scan) {
    if (scan != NULL) {
        char *ds = scan->header.radarStations;
        if (ds != NULL) {
            free(ds);
        }
        RDDataType *dataPtr = scan->data;
        if (dataPtr != NULL) {
            free(dataPtr);
        }
        free(scan);
    }
}

RDScan *RDCloneScan(RDScan *original) {
    RDScan *clone = NULL;
    if (original != NULL) {
        clone = (RDScan *) malloc(sizeof(RDScan));
        if (clone == NULL) {
            fprintf(stderr, "RDCloneScan : ERROR : out of memory\n");
            return NULL;
        }
        memcpy(clone, original, sizeof(RDScan));
        clone->data = NULL;
        clone->data = (RDDataType *) calloc(original->dimLon * original->dimLat, sizeof(RDDataType));
        if (clone->data == NULL) {
            fprintf(stderr, "RDCloneScan : ERROR : could not allocate data buffer : out of memory\n");
            RDFreeScan(clone);
            return NULL;
        }
        memcpy(clone->data, original->data, original->dimLon * original->dimLat * sizeof(RDDataType));
    }
    return clone;
}

#ifdef __cplusplus
}
#endif
