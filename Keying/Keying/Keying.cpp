// Keying.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
using namespace std;
#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")

int main()
{
	GDALDataset* poSrcDS1;
	GDALDataset* poSrcDS2;
	GDALDataset* poDstDS;
	int imgXlen1, imgYlen1, imgXlen2, imgYlen2;
	char* srcPath1 = "space.jpg";
	char* srcPath2 = "superman.jpg";
	char* dstPath = "space.tif";
	GByte ** buffTmp, **buff;
	int i, j, k, bandNum1, bandNum2;

	GDALAllRegister();

	poSrcDS1 = (GDALDataset*)GDALOpenShared(srcPath1, GA_ReadOnly);
	poSrcDS2 = (GDALDataset*)GDALOpenShared(srcPath2, GA_ReadOnly);

	imgXlen1 = poSrcDS1->GetRasterXSize();
	imgYlen1 = poSrcDS1->GetRasterYSize();
	bandNum1 = poSrcDS1->GetRasterCount();
	imgXlen2 = poSrcDS2->GetRasterXSize();
	imgYlen2 = poSrcDS2->GetRasterYSize();
	bandNum2 = poSrcDS2->GetRasterCount();

	buffTmp = (GByte**)CPLMalloc(3 * imgXlen1 * imgYlen1 * sizeof(GByte));
	buff = (GByte**)CPLMalloc(3 * imgXlen2 * imgYlen2 * sizeof(GByte));
	for (i = 0; i < 3; i++) {
		buffTmp[i] = (GByte*)CPLMalloc(imgXlen1 * imgYlen1 * sizeof(GByte));
		buff[i] = (GByte*)CPLMalloc(imgXlen2 * imgYlen2 * sizeof(GByte));
	}


	poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath, imgXlen1, imgYlen1, bandNum1, GDT_Byte, NULL
	);

	for (i = 0; i < 3; i++) {
		poSrcDS2->GetRasterBand(i + 1)->RasterIO(
			GF_Read, 0, 0, imgXlen2, imgYlen2, buff[i], imgXlen2, imgYlen2, GDT_Byte, 0, 0
		);
		printf("... ...pic1 band %d processing ... ...\n", i);
	}

	for (i = 0; i < 3; i++) {
		poSrcDS1->GetRasterBand(i + 1)->RasterIO(
			GF_Read, 0, 0, imgXlen1, imgYlen1, buffTmp[i], imgXlen1, imgYlen1, GDT_Byte, 0, 0
		);
		printf("... ...pic2 band %d processing ... ...\n", i);
	}
	
	for (j = 0; j < imgYlen2; j++) {
		for (i = 0; i < imgXlen2; i++) {
			if ( (buff[0][j*imgXlen2 + i] > 10 && buff[0][j*imgXlen2 + i] < 160) &&
				(buff[1][j*imgXlen2 + i] > 100 && buff[1][j*imgXlen2 + i] < 220) &&
				(buff[2][j*imgXlen2 + i] > 10 && buff[2][j*imgXlen2 + i] < 110)) {

				for(k = 0 ; k < 3 ; k++){
					buff[k][j*imgXlen2 + i] = buffTmp[k][j*imgXlen2 + i];
				}
			}
		}
	}
	
	for (i = 0; i < 3; i++) {
		poDstDS->GetRasterBand(i+1)->RasterIO(
			GF_Write, 0, 0, imgXlen1, imgYlen1, buff[i], imgXlen1, imgYlen1, GDT_Byte, 0, 0
		);
	}

	for (i = 0; i < 3; i++) {
		CPLFree(buffTmp[i]);
		CPLFree(buff[i]);
	}
	CPLFree(buffTmp);
	CPLFree(buff);

	GDALClose(poDstDS);
	GDALClose(poSrcDS1);
	GDALClose(poSrcDS2);

	system("PAUSE");
	return 0;
}

