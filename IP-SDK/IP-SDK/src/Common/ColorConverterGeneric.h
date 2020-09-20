/* File Name: ColorConvertorGeneric.h */
#pragma once

struct intData4
{
    int x{0};
    int y{0};
    int z{0};
    int w{0};
};

#define _FUNC_ inline

// START_CL
#define ROUND211(__v) ((__v) >> 10) + (((__v) & 0x400) >> 9);
#define MAXVAL16            0xffff   // 2^16-1

// Macros for min/max.
#ifndef MIN
#define    MIN(a,b) (((a)<(b))?(a):(b))
#endif // MIN
#ifndef MAX
#define    MAX(a,b) (((a)>(b))?(a):(b))
#endif // MAX

#define __SCALEDOWN16 7

#define __GETY16(R, G, B) (int)((1645 * (R) + 3226 * (G) + 627 * (B)) >> (6 + __SCALEDOWN16))
#define __GETCR16(R, G, B) (int)((2810 * (R) - 2355 * (G) - 454 * (B)) >> (6 + __SCALEDOWN16))
#define __GETCB16(R, G, B) (int)((-947 * (R) - 1862 * (G) + 2810 * (B)) >> (6 + __SCALEDOWN16))

_FUNC_ intData4 RGBToYCrCb(int R, int G, int B)//, __arg_ptr int &Y, __arg_ptr int &Cr, __arg_ptr int &Cb)
{
    intData4 ret;
    ret.x = __GETY16(R, G, B); // Y
    ret.y = __GETCR16(R, G, B); // Cr
    ret.z = __GETCB16(R, G, B); // Cb
    return ret;
}

_FUNC_ intData4 YCrCbToRGB(int Y, int Cr, int Cb, const bool VALIDATE)// __arg_ptr int &R, __arg_ptr int &G, __arg_ptr int &B, const bool VALIDATE = true)
{
    intData4 rgb;
    int Ycalc = 1526 * Y;

    rgb.x = ROUND211(Ycalc + 2092 * Cr);
    rgb.y = ROUND211(Ycalc - 1066 * Cr - (Cb << 9) - (Cb << 2));
    rgb.z = ROUND211(Ycalc + 2644 * Cb);

    if (VALIDATE)
    {
        rgb.x = MAX(0, MIN(MAXVAL16, rgb.x));
        rgb.y = MAX(0, MIN(MAXVAL16, rgb.y));
        rgb.z = MAX(0, MIN(MAXVAL16, rgb.z));
    }
    return rgb;
}

_FUNC_ intData4 RGBToHSL(int R, int G, int B)//, __arg_ptr int &H, __arg_ptr int &S, __arg_ptr int &L)
{
    intData4 hsl;

    int Max = MAX(R, G);
    Max = MAX(Max, B);
    int Min = MIN(R, G);
    Min = MIN(Min, B);
    unsigned int uS = 0;
    unsigned int  uH = 0;
    unsigned int  uL = (unsigned int)((Max + Min) >> 1);

    if (uL == 0 || Max == Min)
        uS = 0;
    else if (uL > 0 && uL <= (MAXVAL16 / 2))
        uS = (unsigned int)(((Max - Min) << 15) / ((Max + Min) >> 1));
    else
        uS = (unsigned int)(((Max - Min) << 15) / (MAXVAL16 - ((Max + Min) >> 1)));

    if (Max == Min)
        uH = 0;
    else if (Max == R && G >= B)
        uH = (unsigned int)((MAXVAL16 / 6) * (G - B) / (Max - Min));
    else if (Max == R && G < B)
        uH = (unsigned int)((MAXVAL16 / 6) * (G - B) / (Max - Min) + MAXVAL16);
    else if (Max == G)
        uH = (unsigned int)((MAXVAL16 / 6) * (B - R) / (Max - Min) + (MAXVAL16 / 3));
    else// if (Max == B)
        uH = (unsigned int)((MAXVAL16 / 6) * (R - G) / (Max - Min) + 2 * (MAXVAL16 / 3));

    uH = MIN(uH, MAXVAL16);
    uS = MIN(uS, MAXVAL16);
    hsl.x = uH;
    hsl.y = uS;
    hsl.z = uL;
    return hsl;
}
_FUNC_ int RFromYCC(int Y, int Cr, int Cb)
{
    int Ycalc = 1526 * Y;
    return ROUND211(Ycalc + 2092 * Cr);
}


// END_CL
