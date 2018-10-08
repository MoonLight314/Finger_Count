//#include <cv.h>
//#include <highgui.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"

#include <windows.h>
#include <tchar.h>

using namespace cv;

int FileExists(TCHAR *);


const	int LIST_LENGTH = 5;
char	CurrentCount = 0;

char	FingerCountList[LIST_LENGTH]={0, };

void	AddFingerCountList(char c);
char	GetFingerCount();


void main()
{
	IplImage*			Cam = 0;	
	CvCapture			*capture = cvCaptureFromCAM(0);
	IplImage			*skin = cvCreateImage(cvSize(320 , 240), IPL_DEPTH_8U, 3);
	IplImage			*gray = cvCreateImage(cvSize(320 , 240), IPL_DEPTH_8U, 1);
	IplImage			*tempImage = cvCreateImage(cvSize(320 , 240), IPL_DEPTH_8U, 3);
	IplImage			*image = cvCreateImage(cvSize(320 , 240), IPL_DEPTH_8U, 3);
	CvScalar			value;
	CvMemStorage		*g_storage = NULL;
	CvSeq				*contours = NULL;
	double				R,G,B;
	unsigned int		Idx = 0;
	CvFont font;

	cvInitFont(&font , CV_FONT_HERSHEY_SIMPLEX, 1.5, 1.5, 0, 3);

	cvNamedWindow( "Skin", 0 );
	cvNamedWindow( "Cam", 0 );

	IplConvKernel * element = cvCreateStructuringElementEx(3, 3, 1, 1, CV_SHAPE_RECT, NULL);
	IplConvKernel * element2 = cvCreateStructuringElementEx(5, 5, 1, 1, CV_SHAPE_RECT, NULL);

	while( true ) 
	{
		cvGrabFrame( capture );
		Cam = cvRetrieveFrame( capture );		

		cvResize(Cam , image, CV_INTER_LINEAR);

		for(int x = 0; x<image->width; x++)
		{
			for(int y = 0; y<image->height; y++)
			{
				value = cvGet2D(image, y, x);
		
				B = value.val[0];
				G = value.val[1];
				R = value.val[2];			

				if( R > 95 && G > 40 && B > 20 && R > G && abs(R-G)>15 )
				{
					value.val[0] = 255;
					value.val[1] = 255;
					value.val[2] = 255;
				}
				else
				{
					value.val[0] = 0;
					value.val[1] = 0;
					value.val[2] = 0;
				}

				cvSet2D(skin,y,x,value);
			}
		}

		cvErode(skin, skin , element, 1);
		cvDilate(skin , skin , element, 1);

		cvCvtColor(skin , gray , COLOR_BGR2GRAY);

		/*  */
		g_storage = cvCreateMemStorage(0);

		/* 모든 외곽선 검출 */
		cvFindContours(
               gray,						// 입력영상
               g_storage,					// 검출된 외곽선을 기록하기 위한 메모리 스토리지
               &contours,					// 외곽선의 좌표들이 저장된 Sequence
               sizeof(CvContour),     
               CV_RETR_TREE,				// 어떤종류의 외곽선 찾을지, 어떻게 보여줄지에 대한정보
			   CV_CHAIN_APPROX_SIMPLE
        );

		cvApproxPoly( contours, sizeof(CvContour), g_storage, CV_POLY_APPROX_DP, 3, 1 );

		/* 외곽선 중에서 가장 큰 외곽선 찾기 -> 손 */
		if(contours) 
		{
			double  Ret = 0 , MaxArea = 0;
			CvSeq *MaxAreaContour = NULL;

			/* Contour 중에서 가장 큰 것을 찾는다. */
			while( contours != NULL ) {

				Ret = cvContourArea(contours,CV_WHOLE_SEQ);

				if( MaxArea < Ret )
				{
					MaxAreaContour = contours;
					MaxArea = Ret;
				}
				contours = contours->h_next;
			}

			cvSet(tempImage, cvScalarAll(1), NULL);

			MaxAreaContour->h_next = NULL;

			/* 가장 큰 외곽선(손)만 그리기 */
			cvDrawContours(
				tempImage,				//외곽선이 그려질 영상
				MaxAreaContour,			//외곽선 트리의 루트노드
				cvScalarAll(255),		//외부 외곽선의 색상
				cvScalarAll(0),			//내부 외곽선의 색상
				100,					//외곽선을 그릴때 이동할 깊이
				CV_FILLED
				);		
		}	
		
		/* */
		if( FileExists(_T("C:\\Deep\\GetFingerCount\\data.bin") ) == 0 )
		{
			FILE *fp = fopen("C:\\Deep\\GetFingerCount\\data.bin","wb");

			for( int row = 0; row < tempImage->height; row++ )
			{
				for ( int col = 0; col < tempImage->width; col++ )
				{
					unsigned char r,b,g;
					b = tempImage->imageData[tempImage->widthStep * row + col * 3];
					g = tempImage->imageData[tempImage->widthStep * row + col * 3 + 1];
					r = tempImage->imageData[tempImage->widthStep * row + col * 3 + 2];

					if( b <= 128 && g <= 128 && r <= 128 )
						fputc(0x00,fp);
					else if( b > 128 && g > 128 && r > 128 )
						fputc(0xFF,fp);
				}
			}
		
			fclose(fp);
		}




		/*  */
		if( FileExists(_T("C:\\Deep\\GetFingerCount\\result.dat") ) != 0 )
		{
			char	str[256];
			int		Ret;
			DWORD	Err;

			FILE *fp = fopen("C:\\Deep\\GetFingerCount\\result.dat","r");

			if( fp != NULL )
			{		
				AddFingerCountList( fgetc(fp) );
				
				sprintf(str,"%c", GetFingerCount() );
				cvPutText( Cam, str , cvPoint( 20, 40 ), &font, cvScalar( 0, 0, 0 ) );
				fclose( fp );
				Ret = DeleteFile( _T("C:\\Deep\\GetFingerCount\\result.dat") );
			}
			else
				printf("FP is null\n");
		}


		cvShowImage( "Skin", tempImage );
		cvShowImage( "Cam", Cam );

		if( cvWaitKey(10) == 32 )
		{
			char FileName[256];
			sprintf(FileName,"Test_%d.jpg",Idx++);
			cvSaveImage( FileName , tempImage);
		}

		if( cvWaitKey(10) == 27 )
		{
			break;
		}		
	}


	cvReleaseCapture( &capture );
	cvDestroyWindow( "Skin" );
	cvDestroyWindow( "Cam" );
}




void AddFingerCountList(char c)
{
	memcpy( &FingerCountList[0], &FingerCountList[1] , LIST_LENGTH - 1 );
	FingerCountList[LIST_LENGTH - 1] = c;
}






char GetFingerCount()
{
	if( FingerCountList[0] == FingerCountList[1] &&
		FingerCountList[0] == FingerCountList[2] )
	{
		CurrentCount = FingerCountList[0];
	}
	
	return CurrentCount;
}








int FileExists(TCHAR *file)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE handle = FindFirstFile(file, &FindFileData) ;
	int found = handle != INVALID_HANDLE_VALUE;

	if(found) 
		FindClose(handle);
	
	return found;
}