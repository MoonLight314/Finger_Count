#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"

#include<stdio.h>
#include<io.h>
#include<conio.h>

using namespace cv;

const int	TOTAL_FILE_COUNT = 54123;


void main()
{
	IplImage			*OrgImg;
	unsigned char		b,g,r;
	unsigned int		Black = 0 , White = 0, None = 0;
	FILE				*fp;
	_finddata_t			fd;
    long				handle;
    int					result;
	unsigned int		Idx, TotalCount = 0;
	char				OutputFileName[256] , OpenFileName[256] , tmp[256];


	for(int i=0; i<6; i++)
	{
		sprintf(tmp,"C:\\Deep\\GetFingerCount\\Generate_Training_Images\\%d\\*.jpg" , i);
		result = 1;
		Idx = 0;

		handle = _findfirst(tmp, &fd);
 
		if (handle == -1)
		{
			printf("There were no files.\n");
			return;
		}
 
		while (result != -1)
		{
			OpenFileName[0]='\0';
			sprintf(OpenFileName, "C:\\Deep\\GetFingerCount\\Generate_Training_Images\\%d\\%s", i , fd.name );

			OrgImg = cvLoadImage( OpenFileName );

			if( OrgImg == NULL )
			{
				printf("File Open Error");
			}

			result = _findnext(handle, &fd);

			sprintf(OutputFileName, "C:\\Deep\\GetFingerCount\\TransformImgData\\TransformImgData\\TransformImgData\\%d\\%05d.bin",i,Idx);

			fp = fopen( OutputFileName ,"wb");

			for( int row = 0; row < OrgImg->height; row++ )
			{
				for ( int col = 0; col < OrgImg->width; col++ )
				{
					b = OrgImg->imageData[OrgImg->widthStep * row + col * 3];
					g = OrgImg->imageData[OrgImg->widthStep * row + col * 3 + 1];
					r = OrgImg->imageData[OrgImg->widthStep * row + col * 3 + 2];

					if( b <= 128 && g <= 128 && r <= 128 )
					{
						Black++;
						fputc(0x00,fp);
					}
					else if( b > 128 && g > 128 && r > 128 )
					{
						White++;
						fputc(0xFF,fp);
					}
					else
					{
						None++;
					}

				}
			}

			Idx++;
			fclose( fp );
			cvReleaseImage( &OrgImg );
		}
	}
	
	_findclose(handle);	
	getch();
}