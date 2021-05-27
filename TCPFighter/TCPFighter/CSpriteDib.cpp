#include<Windows.h>
#include<iostream>
#include<cstdlib>
#include "CSpriteDib.h"


CSpriteDib::CSpriteDib(int iMaxSprite, DWORD dwColorKey)
{
	m_iMaxSprite = iMaxSprite;
	m_dwColorKey = dwColorKey;

	m_stpSprite = NULL;

	m_stpSprite = new st_SPRITE[iMaxSprite];
	memset(m_stpSprite, 0, sizeof(st_SPRITE) * iMaxSprite);

}

CSpriteDib::~CSpriteDib()
{
	int iCount;

	for (int iCount = 0; iCount < m_iMaxSprite; iCount++)
	{
		ReleaseSprite(iCount);
	}
}

BOOL CSpriteDib::LoadDibSprite(int iSpriteIndex, const WCHAR* szFileName, int iCenterPointX, int iCenterPointY)
{
	FILE* pFile;
	errno_t err;
	BITMAPFILEHEADER fileHeader;;
	BITMAPINFOHEADER infoHeader;


	err = _wfopen_s(&pFile, szFileName, L"rb");
	if (err != 0)
	{
		wprintf(L"File open fail\n");
		return FALSE;
	}

	ReleaseSprite(iSpriteIndex);

	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pFile);
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, pFile);

	int fileSize = infoHeader.biWidth * infoHeader.biHeight * 4;
	int iPitch = infoHeader.biWidth * 4;

	m_stpSprite[iSpriteIndex].bypImage = new BYTE[fileSize];
	m_stpSprite[iSpriteIndex].iWidth = infoHeader.biWidth;
	m_stpSprite[iSpriteIndex].iHeight = infoHeader.biHeight;
	m_stpSprite[iSpriteIndex].iPitch = iPitch;
	m_stpSprite[iSpriteIndex].iCenterPointX = iCenterPointX;
	m_stpSprite[iSpriteIndex].iCenterPointY = iCenterPointY;

	BYTE* bypTempBuffer = new BYTE[fileSize];
	BYTE* bypSprite = m_stpSprite[iSpriteIndex].bypImage;
	BYTE* bypTurnTemp;

	fread(bypTempBuffer, fileSize, 1, pFile);

	bypTurnTemp = bypTempBuffer + iPitch * (infoHeader.biHeight - 1);

	for (int iCount = 0; iCount < infoHeader.biHeight; iCount++)
	{
		memcpy(bypSprite, bypTurnTemp, iPitch);
		bypSprite += iPitch;
		bypTurnTemp -= iPitch;
	}

	delete[] bypTempBuffer;
	fclose(pFile);
	return TRUE;
}

void CSpriteDib::ReleaseSprite(int iSpriteIndex)
{
	if (iSpriteIndex >= m_iMaxSprite)
		return;

	if (m_stpSprite[iSpriteIndex].bypImage != NULL)
	{
		delete[] m_stpSprite[iSpriteIndex].bypImage;
		memset(&m_stpSprite[iSpriteIndex], 0, sizeof(st_SPRITE));
	}
}

void CSpriteDib::DrawSprite(int iSpriteIndex, int iDrawX, int iDrawY,
	BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch)
{
	if (iSpriteIndex >= m_iMaxSprite) return;

	if (m_stpSprite[iSpriteIndex].bypImage == NULL) return;

	st_SPRITE* stpSprite = &m_stpSprite[iSpriteIndex];

	int iSpriteWidth = stpSprite->iWidth;
	int iSpriteHeight = stpSprite->iHeight;
	int iCountX, iCountY;

	DWORD* dwpDest = (DWORD*)bypDest;
	DWORD* dwpSprite = (DWORD*)(stpSprite->bypImage);

	iDrawX = iDrawX - stpSprite->iCenterPointX;
	iDrawY = iDrawY - stpSprite->iCenterPointY;

	// 상단 클리핑
	if (0 > iDrawY)
	{
		iSpriteHeight = iSpriteHeight - (-iDrawY);
		dwpSprite = (DWORD*)(stpSprite->bypImage + stpSprite->iPitch * (-iDrawY));

		iDrawY = 0;
	}

	//하단 클리핑
	if (iDestHeight <= iDrawY + stpSprite->iHeight)
	{
		iSpriteHeight -= ((iDrawY + stpSprite->iHeight) - iDestHeight);
	}

	// 좌측 클리핑
	if (0 > iDrawX)
	{
		iSpriteWidth = iSpriteWidth - (-iDrawX);
		dwpSprite = dwpSprite + (-iDrawX);

		iDrawX = 0;
	}

	if (iDestWidth <= iDrawX + stpSprite->iWidth)
	{
		iSpriteWidth -= ((iDrawX + stpSprite->iWidth) - iDestWidth);
	}

	if (iSpriteWidth <= 0 || iSpriteHeight <= 0) return;

	dwpDest = (DWORD*)(((BYTE*)(dwpDest + iDrawX) + (iDrawY * iDestPitch)));

	BYTE* bypDestOrigin = (BYTE*)dwpDest;
	BYTE* bypSpriteOrigin = (BYTE*)dwpSprite;

	for (iCountY = 0; iCountY < iSpriteHeight; iCountY++)
	{
		for (iCountX = 0; iCountX < iSpriteWidth; iCountX++)
		{
			if (m_dwColorKey != (*dwpSprite & 0x00ffffff))
			{
				*dwpDest = *dwpSprite;
			}

			dwpDest++;
			dwpSprite++;
		}

		bypDestOrigin += iDestPitch;
		bypSpriteOrigin += stpSprite->iPitch;

		dwpDest = (DWORD*)bypDestOrigin;
		dwpSprite = (DWORD*)bypSpriteOrigin;
	}
}

void CSpriteDib::DrawSprite50(int iSpriteIndex, int iDrawX, int iDrawY,
	BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch)
{
	if (iSpriteIndex >= m_iMaxSprite) return;

	if (m_stpSprite[iSpriteIndex].bypImage == NULL) return;

	st_SPRITE* stpSprite = &m_stpSprite[iSpriteIndex];

	int iSpriteWidth = stpSprite->iWidth;
	int iSpriteHeight = stpSprite->iHeight;
	int iCountX, iCountY;

	DWORD* dwpDest = (DWORD*)bypDest;
	DWORD* dwpSprite = (DWORD*)(stpSprite->bypImage);

	iDrawX = iDrawX - stpSprite->iCenterPointX;
	iDrawY = iDrawY - stpSprite->iCenterPointY;

	// 상단 클리핑
	if (0 > iDrawY)
	{
		iSpriteHeight = iSpriteHeight - (-iDrawY);
		dwpSprite = (DWORD*)(stpSprite->bypImage + stpSprite->iPitch * (-iDrawY));

		iDrawY = 0;
	}

	//하단 클리핑
	if (iDestHeight <= iDrawY + stpSprite->iHeight)
	{
		iSpriteHeight -= ((iDrawY + stpSprite->iHeight) - iDestHeight);
	}

	// 좌측 클리핑
	if (0 > iDrawX)
	{
		iSpriteWidth = iSpriteWidth - (-iDrawX);
		dwpSprite = dwpSprite + (-iDrawX);

		iDrawX = 0;
	}

	if (iDestWidth <= iDrawX + stpSprite->iWidth)
	{
		iSpriteWidth -= ((iDrawX + stpSprite->iWidth) - iDestWidth);
	}

	if (iSpriteWidth <= 0 || iSpriteHeight <= 0) return;

	dwpDest = (DWORD*)(((BYTE*)(dwpDest + iDrawX) + (iDrawY * iDestPitch)));

	BYTE* bypDestOrigin = (BYTE*)dwpDest;
	BYTE* bypSpriteOrigin = (BYTE*)dwpSprite;

	for (iCountY = 0; iCountY < iSpriteHeight; iCountY++)
	{
		for (iCountX = 0; iCountX < iSpriteWidth; iCountX++)
		{
			if (m_dwColorKey != (*dwpSprite & 0x00ffffff))
			{
				// Red를 제외한 나머지 부분 다운

				*dwpDest = (((*dwpSprite) + (*dwpDest)) >> 1) & 0x007f7f7f;
			}

			dwpDest++;
			dwpSprite++;
		}

		bypDestOrigin += iDestPitch;
		bypSpriteOrigin += stpSprite->iPitch;

		dwpDest = (DWORD*)bypDestOrigin;
		dwpSprite = (DWORD*)bypSpriteOrigin;
	}

}

void CSpriteDib::DrawSpriteRed(int iSpriteIndex, int iDrawX, int iDrawY,
	BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch)
{
	if (iSpriteIndex >= m_iMaxSprite) return;

	if (m_stpSprite[iSpriteIndex].bypImage == NULL) return;

	st_SPRITE* stpSprite = &m_stpSprite[iSpriteIndex];

	int iSpriteWidth = stpSprite->iWidth;
	int iSpriteHeight = stpSprite->iHeight;
	int iCountX, iCountY;

	DWORD* dwpDest = (DWORD*)bypDest;
	DWORD* dwpSprite = (DWORD*)(stpSprite->bypImage);

	iDrawX = iDrawX - stpSprite->iCenterPointX;
	iDrawY = iDrawY - stpSprite->iCenterPointY;

	// 상단 클리핑
	if (0 > iDrawY)
	{
		iSpriteHeight = iSpriteHeight - (-iDrawY);
		dwpSprite = (DWORD*)(stpSprite->bypImage + stpSprite->iPitch * (-iDrawY));

		iDrawY = 0;
	}

	//하단 클리핑
	if (iDestHeight <= iDrawY + stpSprite->iHeight)
	{
		iSpriteHeight -= ((iDrawY + stpSprite->iHeight) - iDestHeight);
	}

	// 좌측 클리핑
	if (0 > iDrawX)
	{
		iSpriteWidth = iSpriteWidth - (-iDrawX);
		dwpSprite = dwpSprite + (-iDrawX);

		iDrawX = 0;
	}

	if (iDestWidth <= iDrawX + stpSprite->iWidth)
	{
		iSpriteWidth -= ((iDrawX + stpSprite->iWidth) - iDestWidth);
	}

	if (iSpriteWidth <= 0 || iSpriteHeight <= 0) return;

	dwpDest = (DWORD*)(((BYTE*)(dwpDest + iDrawX) + (iDrawY * iDestPitch)));

	BYTE* bypDestOrigin = (BYTE*)dwpDest;
	BYTE* bypSpriteOrigin = (BYTE*)dwpSprite;

	for (iCountY = 0; iCountY < iSpriteHeight; iCountY++)
	{
		for (iCountX = 0; iCountX < iSpriteWidth; iCountX++)
		{
			if (m_dwColorKey != (*dwpSprite & 0x00ffffff))
			{
				// Red를 제외한 나머지 부분 다운

				*dwpDest = (*dwpSprite) & 0x00ff4f4f;
			}

			dwpDest++;
			dwpSprite++;
		}

		bypDestOrigin += iDestPitch;
		bypSpriteOrigin += stpSprite->iPitch;

		dwpDest = (DWORD*)bypDestOrigin;
		dwpSprite = (DWORD*)bypSpriteOrigin;
	}

}

void CSpriteDib::DrawImage(int iSpriteIndex, int iDrawX, int iDrawY,
	BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch, int iDrawLen)
{
	if (iSpriteIndex >= m_iMaxSprite) return;

	if (m_stpSprite[iSpriteIndex].bypImage == NULL) return;

	st_SPRITE* stpSprite = &m_stpSprite[iSpriteIndex];

	int iSpriteWidth = m_stpSprite[iSpriteIndex].iWidth;
	int iSpriteHeight = m_stpSprite[iSpriteIndex].iHeight;
	int iCountY;

	iSpriteWidth = iSpriteWidth * iDrawLen / 100;


	DWORD* dwpDest = (DWORD*)bypDest;
	DWORD* dwpSprite = (DWORD*)(stpSprite->bypImage);

	iDrawX = iDrawX - stpSprite->iCenterPointX;
	iDrawY = iDrawY - stpSprite->iCenterPointY;

	// 상단 클리핑
	if (0 > iDrawY)
	{
		iSpriteHeight = iSpriteHeight - (-iDrawY);
		dwpSprite = (DWORD*)(stpSprite->bypImage + stpSprite->iPitch * (-iDrawY));

		iDrawY = 0;
	}

	//하단 클리핑
	if (iDestHeight <= iDrawY + stpSprite->iHeight)
	{
		iSpriteHeight -= ((iDrawY + stpSprite->iHeight) - iDestHeight);
	}

	// 좌측 클리핑
	if (0 > iDrawX)
	{
		iSpriteWidth = iSpriteWidth - (-iDrawX);
		dwpSprite = dwpSprite + (-iDrawX);

		iDrawX = 0;
	}

	if (iDestWidth <= iDrawX + stpSprite->iWidth)
	{
		iSpriteWidth -= ((iDrawX + stpSprite->iWidth) - iDestWidth);
	}

	if (iSpriteWidth <= 0 || iSpriteHeight <= 0) return;

	dwpDest = (DWORD*)(((BYTE*)(dwpDest + iDrawX) + (iDrawY * iDestPitch)));

	for (iCountY = 0; iCountY < iSpriteHeight; iCountY++)
	{
		memcpy(dwpDest, dwpSprite, iSpriteWidth * 4);

		dwpDest = (DWORD*)((BYTE*)dwpDest + iDestPitch);
		dwpSprite = (DWORD*)((BYTE*)dwpSprite + stpSprite->iPitch);
	}
}