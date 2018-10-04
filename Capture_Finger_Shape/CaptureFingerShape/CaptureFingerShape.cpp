//#include <cv.h>
//#include <highgui.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"

#include <windows.h>
#include <tchar.h>

using namespace cv;


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

		/* ��� �ܰ��� ���� */
		cvFindContours(
               gray,						// �Է¿���
               g_storage,					// ����� �ܰ����� ����ϱ� ���� �޸� ���丮��
               &contours,					// �ܰ����� ��ǥ���� ����� Sequence
               sizeof(CvContour),     
               CV_RETR_TREE,				// ������� �ܰ��� ã����, ��� ���������� ��������
			   CV_CHAIN_APPROX_SIMPLE
        );

		cvApproxPoly( contours, sizeof(CvContour), g_storage, CV_POLY_APPROX_DP, 3, 1 );

		/* �ܰ��� �߿��� ���� ū �ܰ��� ã�� -> �� */
		if(contours) 
		{
			double  Ret = 0 , MaxArea = 0;
			CvSeq *MaxAreaContour = NULL;

			/* Contour �߿��� ���� ū ���� ã�´�. */
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

			/* ���� ū �ܰ���(��)�� �׸��� */
			cvDrawContours(
				tempImage,				//�ܰ����� �׷��� ����
				MaxAreaContour,			//�ܰ��� Ʈ���� ��Ʈ���
				cvScalarAll(255),		//�ܺ� �ܰ����� ����
				cvScalarAll(0),			//���� �ܰ����� ����
				100,					//�ܰ����� �׸��� �̵��� ����
				CV_FILLED
				);		
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