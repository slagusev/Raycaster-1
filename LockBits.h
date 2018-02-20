/*
************************************
* Custom LockBits graphics class   *
*                                  *
* LockBits.h                       *
*                                  *
* (c) 2017, 2018 Stefan Kubsch     *
************************************
*/

class LockBits
{
public:
	LockBits(Gdiplus::Bitmap &inputBMP);
	~LockBits();

	__forceinline void Lock();
	__forceinline void SetPixel(uint_fast32_t x, uint_fast32_t y, uint_fast32_t color);
	__forceinline void SetShadedPixel(uint_fast32_t x, uint_fast32_t y, uint_fast32_t color, uint_fast32_t ShadeFactor);
	__forceinline uint_fast32_t GetPixel(uint_fast32_t x, uint_fast32_t y);
	__forceinline void Release();

private:
	uint_fast32_t* Pixels;
	Gdiplus::Bitmap* work;
	Gdiplus::BitmapData bData;
	Gdiplus::Rect Rectangle;
	uint_fast32_t StrideShift;
};

__forceinline LockBits::LockBits(Gdiplus::Bitmap &inputBMP)
{
	work = &inputBMP;
	Rectangle = Gdiplus::Rect(0, 0, work->GetWidth(), work->GetHeight());
	work->LockBits(&Rectangle, Gdiplus::ImageLockModeWrite, PixelFormat32bppPARGB, &bData);
	Pixels = (uint_fast32_t*)bData.Scan0;
	StrideShift = bData.Stride >> 2;
	work->UnlockBits(&bData);
}

__forceinline LockBits::~LockBits() {}

__forceinline void LockBits::Lock()
{
	work->LockBits(&Rectangle, Gdiplus::ImageLockModeWrite,PixelFormat32bppPARGB,&bData);
}

__forceinline void LockBits::SetPixel(uint_fast32_t x, uint_fast32_t y, uint_fast32_t color)
{
	Pixels[y * StrideShift + x] = color;
}

__forceinline void LockBits::SetShadedPixel(uint_fast32_t x, uint_fast32_t y, uint_fast32_t color, uint_fast32_t ShadeFactor)
{
	uint_fast32_t OriginalR = color & 0x00FF0000;
	uint_fast32_t OriginalG = color & 0x0000FF00;
	uint_fast32_t OriginalB = color & 0x000000FF;

	uint_fast32_t ShadedR = (OriginalR * ShadeFactor) >> 7;
	uint_fast32_t ShadedG = (OriginalG * ShadeFactor) >> 7;
	uint_fast32_t ShadedB = (OriginalB * ShadeFactor) >> 7;

	if (ShadedR > OriginalR)
	{
		ShadedR = 0;
	}

	if (ShadedG > OriginalG)
	{
		ShadedG = 0;
	}

	if (ShadedB > OriginalB)
	{
		ShadedB = 0;
	}

	Pixels[y * StrideShift + x] = (color & 0xff000000) | (ShadedR & 0x00FF0000) | (ShadedG & 0x0000FF00) | (ShadedB & 0x000000FF);
}

__forceinline uint_fast32_t LockBits::GetPixel(uint_fast32_t x, uint_fast32_t y)
{
	return Pixels[y * StrideShift + x];
}

__forceinline void LockBits::Release(void)
{
	work->UnlockBits(&bData);
}


