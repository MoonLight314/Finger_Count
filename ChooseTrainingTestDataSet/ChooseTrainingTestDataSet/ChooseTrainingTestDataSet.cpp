// ChooseTrainingTestDataSet.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>



const int	ONE_BATCH_SIZE = 100;
const int	IMAGE_FILE_SIZE = 320 * 240;


int _tmain(int argc, _TCHAR* argv[])
{
	int		Folder , Count , FileNumber , TotalCount;
	int		NUM_OF_IMAGE[] = { 10900 , 9300 ,  8800, 9200 , 7800 , 7900 };	//53900
	int		FileCount[6] = {0, } , *SelectedFileNumber[6];
	int		i;
	TCHAR	SrcPath[256] , DstPath[256];
	FILE	*MergedTrainingData , *CorrectAns , *ImageDataFile;
	BYTE	*Buffer;

	Count = 0;

	TotalCount = 0;

	/*  */
	for(i=0; i<6; i++)
	{
		SelectedFileNumber[i] = (int *)calloc( NUM_OF_IMAGE[i] , sizeof(int));
		Count += NUM_OF_IMAGE[i];
	}

	Buffer = (BYTE *)calloc( IMAGE_FILE_SIZE , sizeof(BYTE));

	srand( (unsigned)time(NULL) );

	MergedTrainingData = _tfopen( _T("TrainingDataSet_0000.dat") , _T("wb") );
	CorrectAns = _tfopen( _T("CorrectAns_0000.dat") , _T("wt") );

	/*  */
	while( true )
	{
		/* 해당 폴더의 파일 갯수가 이미 Count만큼 다 선택했는지... */
		while( true )
		{
			Folder = rand() % 6;
			
			//if( FileCount[Folder] == Count )
			if( FileCount[Folder] == NUM_OF_IMAGE[Folder] )
				continue;
			else
				break;
		}

		/* 선택 파일이 이미 복사되었는지 확인 */
		while( true ) 
		{
			bool Duplicated = false;

			FileNumber = rand() % NUM_OF_IMAGE[Folder];

			for(i=0; i<FileCount[Folder]; i++)
			{
				if( SelectedFileNumber[Folder][i] == FileNumber )
				{
					Duplicated = true;
					break;
				}
			}

			if( Duplicated )
				continue;
			else
				break;
		}
		
		_stprintf(SrcPath , _T("C:\\Deep\\GetFingerCount\\TransformImgData\\TransformImgData\\TransformImgData\\%d\\%05d.bin"), Folder , FileNumber );

		ImageDataFile = _tfopen( SrcPath , _T("rb") );

		fread( (void *)Buffer , 1, IMAGE_FILE_SIZE , ImageDataFile);

		if( fwrite( (void *)Buffer , 1, IMAGE_FILE_SIZE , MergedTrainingData) == IMAGE_FILE_SIZE )
		{
			/* 해당하는 정답도 같이 기록 */
			_ftprintf(CorrectAns, _T("%d\n"), Folder );

			SelectedFileNumber[Folder][FileCount[Folder]] = FileNumber;
			FileCount[Folder]++;
			TotalCount++;

			if( TotalCount == Count )
				break;

			/* 진행률 */
			printf("%.1f %%\r" , float(TotalCount * 100) / float(Count));

			if( ImageDataFile != NULL )
				fclose( ImageDataFile );

			/*  */
			if( TotalCount % ONE_BATCH_SIZE == 0 )
			{
				TCHAR FileName[256];

				if( MergedTrainingData != NULL )
					fclose( MergedTrainingData );

				if( CorrectAns != NULL )
					fclose( CorrectAns );

				_stprintf(FileName , _T("TrainingDataSet_%04d.dat"), TotalCount / ONE_BATCH_SIZE );
				MergedTrainingData = _tfopen( FileName , _T("wb") );

				_stprintf(FileName , _T("CorrectAns_%04d.dat"), TotalCount / ONE_BATCH_SIZE );
				CorrectAns = _tfopen( FileName , _T("wt") );
			}
		}
		else
		{
			printf("FWRITE Fail");
		}

	}

	/* Clear Up */
	for(i=0; i<6; i++)
		free( SelectedFileNumber[i] );

	free( Buffer );

	if( MergedTrainingData != NULL )
		fclose( MergedTrainingData );

	if( CorrectAns != NULL )
		fclose( CorrectAns );
	
	if( ImageDataFile != NULL )
		fclose( ImageDataFile );

	return 0;
}