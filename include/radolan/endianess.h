#ifndef __RADOLAN_ENDIANESS__
#define __RADOLAN_ENDIANESS__

bool isLittleEndian()
{
    short int number = 0x1;
    char *numPtr = (char*)&number;
    return (numPtr[0] == 1);
}

#endif
