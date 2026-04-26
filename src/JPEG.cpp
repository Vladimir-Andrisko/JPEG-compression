#include "JPEG.h"
#include "NxNDCT.h"
#include <math.h>

#include "JPEGBitStreamWriter.h"


#define DEBUG(x) do{ qDebug() << #x << " = " << x;}while(0)

enum zigzag_option{QUANT = 0, DC = 1};

// quantization tables from JPEG Standard, Annex K
uint8_t QuantLuminance[8*8] =
    { 16, 11, 10, 16, 24, 40, 51, 61,
      12, 12, 14, 19, 26, 58, 60, 55,
      14, 13, 16, 24, 40, 57, 69, 56,
      14, 17, 22, 29, 51, 87, 80, 62,
      18, 22, 37, 56, 68,109,103, 77,
      24, 35, 55, 64, 81,104,113, 92,
      49, 64, 78, 87,103,121,120,101,
      72, 92, 95, 98,112,100,103, 99 };
uint8_t QuantChrominance[8*8] =
    { 17, 18, 24, 47, 99, 99, 99, 99,
      18, 21, 26, 66, 99, 99, 99, 99,
      24, 26, 56, 99, 99, 99, 99, 99,
      47, 66, 99, 99, 99, 99, 99, 99,
      99, 99, 99, 99, 99, 99, 99, 99,
      99, 99, 99, 99, 99, 99, 99, 99,
      99, 99, 99, 99, 99, 99, 99, 99,
      99, 99, 99, 99, 99, 99, 99, 99 };

static char quantizationMatrix[64] =
{
    16, 11, 10, 16, 24, 40, 51, 61,
    12, 12, 14, 19, 26, 58, 60, 55,
    14, 13, 16, 24, 40, 57, 69, 56,
    14, 17, 22, 29, 51, 87, 80, 62,
    18, 22, 37, 56, 68, 109, 103, 77,
    24, 35, 55, 64, 81, 104, 113, 92,
    49, 64, 78, 87, 103, 121, 120, 101,
    72, 92, 95, 98, 112, 100, 103, 99
};

struct imageProperties{
    int width;
    int height;
    int16_t coeffs[8*8];
};


void DCTUandV(const char input[], int16_t output[], int N, double* DCTKernel)
{
    double* temp = new double[N*N];
    double* DCTCoefficients = new double[N*N];

    double sum;
    for (int i = 0; i <= N - 1; i++)
    {
        for (int j = 0; j <= N - 1; j++)
        {
            sum = 0;
            for (int k = 0; k <= N - 1; k++)
            {
                sum = sum + DCTKernel[i*N+k] * (input[k*N+j]);
            }
            temp[i*N + j] = sum;
        }
    }

    for (int i = 0; i <= N - 1; i++)
    {
        for (int j = 0; j <= N - 1; j++)
        {
            sum = 0;
            for (int k = 0; k <= N - 1; k++)
            {
                sum = sum + temp[i*N+k] * DCTKernel[j*N+k];
            }
            DCTCoefficients[i*N+j] = sum;
        }
    }

    for(int i = 0; i < N*N; i++)
    {
        output[i] = floor(DCTCoefficients[i]+0.5);
    }

    delete[] temp;
    delete[] DCTCoefficients;

    return;
}

uint8_t quantQuality(uint8_t quant, uint8_t quality) {
    // Convert to an internal JPEG quality factor, formula taken from libjpeg
    int16_t q = quality < 50 ? 5000 / quality : 200 - quality * 2;
    return clamp((quant * q + 50) / 100, 1, 255);
}

static void doZigZag(int16_t block[], uint8_t quantizationBlock[], int N, int DCTorQuantization)
{

    // DCT
    if(DCTorQuantization){
        uint16_t* temp = new uint16_t[N*N];

        int x = 0, y = 0;
        bool up = true;

        for (int i = 0; i < N*N; i++) {
            temp[i] = block[y*N + x];

            if(up){
                if(x == N-1){
                    y++;
                    up = false;
                }else if(y == 0){
                    x++;
                    up = false;
                }else{
                    x++;
                    y--;
                }
            }else{
                if(y == N-1){
                    x++;
                    up = true;
                }else if(x == 0){
                    y++;
                    up = true;
                }else{
                    x--;
                    y++;
                }
            }
        }

        for(int i = 0; i < N*N; i++){
            block[i] = temp[i];
        }

        delete[] temp;

    }else{  // Quantization
        uint8_t* temp = new uint8_t[N*N];

        int x = 0, y = 0;
        bool up = true;

        for (int i = 0; i < N*N; i++) {
            temp[i] = quantizationBlock[y*N + x];

            if(up){
                if(x == N-1){
                    y++;
                    up = false;
                }else if(y == 0){
                    x++;
                    up = false;
                }else{
                    x++;
                    y--;
                }
            }else{
                if(y == N-1){
                    x++;
                    up = true;
                }else if(x == 0){
                    y++;
                    up = true;
                }else{
                    x--;
                    y++;
                }
            }
        }

        for(int i = 0; i < N*N; i++){
            quantizationBlock[i] = temp[i];
        }

        delete[] temp;
    }
}

/* perform DCT */
imageProperties performDCT(char input[], int xSize, int ySize, int N, uint8_t quality, bool quantType)
{
    double *kernel = new double[N*N];

    imageProperties output;
    output.width = xSize;
    output.height = ySize;

    GenerateDCTmatrix(kernel, N);
    if(quantType){
        DCT(input, output.coeffs, N, kernel);
    }else{
        DCTUandV(input, output.coeffs, N, kernel);
    }

    if(quantType){
        for(int i = 0; i < N*N; i++){
            output.coeffs[i] *= (int16_t)(QuantLuminance[i] + 0.5);
        }
    }else{
        for(int i = 0; i < N*N; i++){
            output.coeffs[i] *= (int16_t)(QuantChrominance[i] + 0.5);
        }
    }

    doZigZag(output.coeffs, nullptr, N, DC);

    delete[] kernel;
    return output;
}

inline void copyBlock(char *block, char* input, int x, int y, int width, int N){
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            block[i*N + j] = input[(y+i)*width + x+j];
        }
    }
}

//JPEGBitStreamWriter streamer("example.jpg");
void performJPEGEncoding(uchar Y_buff[], char U_buff[], char V_buff[], int xSize, int ySize, int quality)
{
	DEBUG(quality);
    auto s = new JPEGBitStreamWriter("example.jpg");
    const int N = 8;

    for (int i = 0; i < 64; i++) {
        QuantLuminance[i] = quantQuality(QuantLuminance[i], quality);
        QuantChrominance[i] = quantQuality(QuantChrominance[i], quality);
    }

    uint8_t *temp_quant_lum = new uint8_t[N*N];
    uint8_t *temp_quant_chrom = new uint8_t[N*N];
    for(int i = 0; i < N*N; i++){
        temp_quant_lum[i] = QuantLuminance[i];
        temp_quant_chrom[i] = QuantChrominance[i];
    }

    doZigZag(nullptr, temp_quant_lum, N, QUANT);
    doZigZag(nullptr, temp_quant_chrom, N, QUANT);

    s->writeHeader();
    s->writeQuantizationTables(temp_quant_lum, temp_quant_chrom);
    s->writeImageInfo(xSize, ySize);
    s->writeHuffmanTables();

    // Blocks of Y U V
    int newXSize, newYSize, dump;
    char* Y_buff_extended = nullptr;
    char* U_buff_extended = nullptr;
    char* V_buff_extended = nullptr;

    extendBorders((char*)Y_buff, xSize, ySize, 16, &Y_buff_extended, &newXSize, &newYSize);
    extendBorders(U_buff, xSize, ySize, 8, &U_buff_extended, &dump, &dump);
    extendBorders(V_buff, xSize, ySize, 8, &V_buff_extended, &dump, &dump);

    for(int y = 0; y < newYSize; y += 16){
        for(int x = 0; x < newXSize; x += 16){
            char block[N*N];

            copyBlock(block, Y_buff_extended, x, y, newXSize, N);
            s->writeBlockY(performDCT(block, N, N, N, quality, 1).coeffs);

            copyBlock(block, Y_buff_extended, x+8, y, newXSize, N);
            s->writeBlockY(performDCT(block, N, N, N, quality, 1).coeffs);

            copyBlock(block, Y_buff_extended, x, y+8, newXSize, N);
            s->writeBlockY(performDCT(block, N, N, N, quality, 1).coeffs);

            copyBlock(block, Y_buff_extended, x+8, y+8, newXSize, N);
            s->writeBlockY(performDCT(block, N, N, N, quality, 1).coeffs);

            copyBlock(block, U_buff_extended, x/2, y/2, newXSize/2, N);
            s->writeBlockU(performDCT(block, N, N, N, quality, 0).coeffs);

            copyBlock(block, V_buff_extended, x/2, y/2, newXSize/2, N);
            s->writeBlockV(performDCT(block, N, N, N, quality, 0).coeffs);
        }
    }

    s->finishStream();
    delete[] temp_quant_lum;
    delete[] temp_quant_chrom;
}
