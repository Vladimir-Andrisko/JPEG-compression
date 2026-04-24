#include "ColorSpaces.h"

/********************************************************************************************************************************/
/* RGB processing */
/********************************************************************************************************************************/
void processing_RGB(const uchar rgbInputImg[], int x, int y, uchar rgbOutputImg[], double R, double G, double B)
{
	for(int i = 0; i< x; i++)
	{
		for(int j=0; j<y; j++)
		{
			rgbOutputImg[j*x*3+i*3] = R*rgbInputImg[j*x*3+i*3];
			rgbOutputImg[j*x*3+i*3+1] = G*rgbInputImg[j*x*3+i*3+1];
			rgbOutputImg[j*x*3+i*3+2] = B*rgbInputImg[j*x*3+i*3+2];
		}
	}
}

/********************************************************************************************************************************/
/* YUV444 processing */
/********************************************************************************************************************************/
void RGBtoYUV444(const uchar rgbImg[], int x, int y, uchar Y_buff[], char U_buff[], char V_buff[]) 
{
	uchar R, G, B;
	for(int i = 0; i<x; i++)
	{
		for(int j = 0; j<y; j+=1)
		{
			R = rgbImg[j*3*x+i*3];
			G = rgbImg[j*3*x+i*3 + 1];
			B = rgbImg[j*3*x+i*3 + 2];

			Y_buff[j*x+i] =  0.299*R + 0.587*G + 0.114*B;
			U_buff[j*x+i] =  - 0.14713*R - 0.28886*G + 0.436*B;
			V_buff[j*x+i] =  R*0.615 - 0.51499*G - 0.10001*B;
		}
	}
}

void YUV444toRGB(const uchar Y_buff[], const char U_buff[], const char V_buff[], int x, int y, uchar rgbImg[]) 
{
	double R,G,B;
	double Y, U, V;
	for(int i = 0; i<x; i++)
	{
		for(int j = 0; j<y; j+=1)
		{
			Y = Y_buff[j*x+i];
			U = U_buff[j*x+i];
			V = V_buff[j*x+i];

			R = Y + 1.13983*V;
			G = Y - 0.39465*U - 0.58060*V;
			B = Y + 2.03211*U;
			
			if (R < 0)
				R = 0;
			else if (R > 255)
				R = 255;
			if (G< 0)
				G = 0;
			else if (G> 255)
				G = 255;
			if (B < 0)
				B = 0;
			else if (B > 255)
				B = 255;


			rgbImg[j*3*x+i*3] =  R;
			rgbImg[j*3*x+i*3 + 1] = G;
			rgbImg[j*3*x+i*3 + 2] =  B;
	
		}
	}
}

void procesing_YUV444(uchar Y_buff[], char U_buff[], char V_buff[], int x, int y, double Y, double U, double V)
{
	for(int i = 0; i<x; i++)
	{
		for(int j = 0; j<y; j++)
		{
			Y_buff[j*x+i] *= Y; 
			U_buff[j*x+i] *= U; 
			V_buff[j*x+i] *= V; 
		}
	}

}

/*******************************************************************************************************************************/
/* YUV422 processing */
/********************************************************************************************************************************/
void RGBtoYUV422(const uchar rgbImg[], int x, int y, uchar Y_buff[], char U_buff[], char V_buff[]) 
{
	uchar R, G, B;
	for(int i = 0; i<x; i+=2)
	{
		for(int j = 0; j<y; j++)
		{
			R = rgbImg[j*3*x+i*3];
			G = rgbImg[j*3*x+i*3 + 1];
			B = rgbImg[j*3*x+i*3 + 2];

			Y_buff[j*x+i] =  0.299*R + 0.587*G + 0.114*B;
			U_buff[j*x/2+i/2] =  (- 0.14713*R - 0.28886*G + 0.436*B)/2;
			V_buff[j*x/2+i/2] =  (R*0.615 - 0.51499*G - 0.10001*B)/2;

			R = rgbImg[j*3*x+(i+1)*3];
			G = rgbImg[j*3*x+(i+1)*3 + 1];
			B = rgbImg[j*3*x+(i+1)*3 + 2];

			Y_buff[j*x+(i+1)] =  0.299*R + 0.587*G + 0.114*B;
			U_buff[j*x/2+i/2] +=  (- 0.14713*R - 0.28886*G + 0.436*B)/2;
			V_buff[j*x/2+i/2] +=  (R*0.615 - 0.51499*G - 0.10001*B)/2;
		}
	}
}

void YUV422toRGB(const uchar Y_buff[], const char U_buff[], const char V_buff[], int x, int y, uchar rgbImg[]) 
{
	double R,G,B;
	double Y, U, V;
	for(int i = 0; i<x; i+=2)
	{
		for(int j = 0; j<y; j++)
		{
			U = U_buff[j*x/2+i/2];
			V = V_buff[j*x/2+i/2];

			Y = Y_buff[j*x+i];

			R = Y + 1.13983*V;
			G = Y -0.39465*U - 0.58060*V;
			B = Y + 2.03211*U;

			if (R < 0)
				R = 0;
			else if (R > 255)
				R = 255;
			if (G < 0)
				G = 0;
			else if (G> 255)
				G = 255;
			if (B < 0)
				B = 0;
			else if (B > 255)
				B = 255;

			rgbImg[j*3*x+i*3] =  R;
			rgbImg[j*3*x+i*3 + 1] = G;
			rgbImg[j*3*x+i*3 + 2] = B;

			Y = Y_buff[j*x+(i+1)];

			R = Y + 1.13983*V;
			G = Y -0.39465*U - 0.58060*V;
			B = Y + 2.03211*U;

			if (R < 0)
				R = 0;
			else if (R > 255)
				R = 255;
			if (G< 0)
				G = 0;
			else if (G> 255)
				G = 255;
			if (B < 0)
				B = 0;
			else if (B > 255)
				B = 255;

			rgbImg[j*3*x+(i+1)*3] =  R;
			rgbImg[j*3*x+(i+1)*3 + 1] = G;
			rgbImg[j*3*x+(i+1)*3 + 2] = B;
		}
	}
}

void procesing_YUV422(uchar Y_buff[], char U_buff[], char V_buff[], int x, int y, double Y, double U, double V)
{
	for(int i = 0; i<x; i+=2)
	{
		for(int j = 0; j<y; j++)
		{
			Y_buff[j*x+i] *= Y; 
			Y_buff[j*x+i+1] *= Y; 
			U_buff[j*x/2+i/2] *= U; 
			V_buff[j*x/2+i/2] *= V; 
		}
	}

}

/*******************************************************************************************************************************/
/* YUV420 processing */
/*******************************************************************************************************************************/
void RGBtoYUV420(const uchar rgbImg[], int x_size, int y_size, uchar Y_buff[], char U_buff[], char V_buff[])
{
	uchar R, G, B;
    uchar R1, G1, B1, R2, G2, B2, R3, G3, B3;

    for(int y = 0; y < y_size; y++){
        for(int x = 0; x < x_size; x++){
            R = rgbImg[(y*x_size + x)*3];
            G = rgbImg[(y*x_size + x)*3 + 1];
            B = rgbImg[(y*x_size + x)*3 + 2];

            double Y = 0.299*R + 0.587*G + 0.114*B;
            Y_buff[y*x_size + x] = (uchar)Y;
        }
    }

    for(int y = 0; y < y_size; y+=2){
        for(int x = 0; x < x_size; x+=2){
            R = rgbImg[(y*x_size + x)*3];
            G = rgbImg[(y*x_size + x)*3 + 1];
            B = rgbImg[(y*x_size + x)*3 + 2];

            double U1 = -0.14713*R - 0.28886*G + 0.436*B;
            double V1 = 0.615*R - 0.51499*G - 1.0001*B;

            R1 = rgbImg[(y*x_size + (x+1))*3];
            G1 = rgbImg[(y*x_size + (x+1))*3 + 1];
            B1 = rgbImg[(y*x_size + (x+1))*3 + 2];

            double U2 = -0.14713*R1 - 0.28886*G1 + 0.436*B1;
            double V2 = 0.615*R1 - 0.51499*G1 - 1.0001*B1;

            R2 = rgbImg[((y+1)*x_size + x)*3];
            G2 = rgbImg[((y+1)*x_size + x)*3 + 1];
            B2 = rgbImg[((y+1)*x_size + x)*3 + 2];

            double U3 = -0.14713*R2 - 0.28886*G2 + 0.436*B2;
            double V3 = 0.615*R2 - 0.51499*G2 - 1.0001*B2;

            R3 = rgbImg[((y+1)*x_size + (x+1))*3];
            G3 = rgbImg[((y+1)*x_size + (x+1))*3 + 1];
            B3 = rgbImg[((y+1)*x_size + (x+1))*3 + 2];

            double U4 = -0.14713*R3 - 0.28886*G3 + 0.436*B3;
            double V4 = 0.615*R3 - 0.51499*G3 - 1.0001*B3;

            double U = (U1 + U2 + U3 + U4)/4;
            double V = (V1 + V2 + V3 + V4)/4;

            if (U > 127) U = 127;
            if (U < -128) U = -128;

            if (V > 127) V = 127;
            if (V < -128) V = -128;

            U_buff[(y/2)*(x_size/2) + x/2] = (uchar)U;
            V_buff[(y/2)*(x_size/2) + x/2] = (uchar)V;
        }
    }
}

void YUV420toRGB(const uchar Y_buff[], const char U_buff[], const char V_buff[], int w, int h, uchar rgbImg[])
{
    double Rd,Gd,Bd;
	double Y, U, V;

    for(int y = 0; y < h; y += 2){
        for(int x = 0; x < w; x += 2){

            U = U_buff[(y/2)*(w/2) + (x/2)];
            V = V_buff[(y/2)*(w/2) + (x/2)];

            for(int yb = 0; yb < 2; yb++){
                for(int xb = 0; xb < 2; xb++){

                    Y = Y_buff[(y+yb)*w + (x+xb)];

                    Rd = Y + 1.13983*V;
                    Gd = Y -0.39465*U - 0.58060*V;
                    Bd = Y + 2.03211*U;

                    if (Rd < 0) Rd = 0;
                    if (Rd > 255) Rd = 255;

                    if (Gd < 0) Gd = 0;
                    if (Gd > 255) Gd = 255;

                    if (Bd < 0) Bd = 0;
                    if (Bd > 255) Bd = 255;

                    int outIdx = ((y+yb)*w + (x+xb))*3;

                    rgbImg[outIdx] = (uchar)Rd;
                    rgbImg[outIdx + 1] = (uchar)Gd;
                    rgbImg[outIdx + 2] = (uchar)Bd;
                }
            }
        }
    }
}

void procesing_YUV420(uchar Y_buff[], char U_buff[], char V_buff[], int w, int h, double Y, double U, double V)
{
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x){
            Y_buff[y*w + x] *= Y;
        }
    }

    for (int y = 0; y < h; y+=2)
    {
        for (int x = 0; x < w; x+=2)
        {
            U_buff[(y/2)*(w/2) + x/2] *= U;
            V_buff[(y/2)*(w/2) + x/2] *= V;
        }
    }

}

/*******************************************************************************************************************************/
/* Y decimation */
/*******************************************************************************************************************************/
void decimate_Y(uchar Y_buff[], int x, int y)
{
    uchar Y;

    for (int i = 0; i < y; i += 2) {
        for (int j = 0; j < x; j += 2) {

            Y = Y_buff[i * x + j];
            Y_buff[i * x + (j + 1)] = Y;
            Y_buff[(i + 1) * x + j] = Y;
            Y_buff[(i + 1) * x + (j + 1)] = Y;
        }
    }

}
