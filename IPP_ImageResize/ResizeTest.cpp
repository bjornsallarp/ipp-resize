#include "stdafx.h"
#include "ipp.h"
#include "resize.h"
#include "jpeg.h"
#include "stdfilein.h"
#include "stdfileout.h"

using namespace std;
using namespace UIC;

int main(int argc, char* argv[])
{

	char * fileName = "C:\\Users\\merrimack\\Projects\\IPP_ImageResize\\Debug\\sf.jpg";
	char * outFile = "C:\\Users\\merrimack\\Projects\\IPP_ImageResize\\Debug\\sf-resized.jpg";

	CStdFileInput input;
	PARAMS_JPEG params;
	CIppImage image;

	input.Open(fileName);

	ReadImageJPEG(input, params, image);

	CStdFileOutput out1;
	out1.Open("C:\\Users\\merrimack\\Projects\\IPP_ImageResize\\Debug\\sf1.jpg");
	params.quality = 50;
	params.comment_size = 0;
	
	SaveImageJPEG(image, params, out1);

	Resize r;
	r.SetInterpType(Resize::SUPERSAMPLE);
	r.SetFactors(0.5,0.5);
	IppiSize newSize = r.GetDestSize(image.Size());

	CIppImage resizedImage;
	resizedImage.Alloc(newSize, image.NChannels(), image.Precision());
	resizedImage.Color(image.Color());
	

	int bufferSize;
	Ipp8u *pBuffer;

	IppiRect origRect = {0, 0, image.Width(), image.Height()};
	IppiRect newRect = {0, 0, resizedImage.Width(), resizedImage.Height()};

	ippiResizeGetBufSize(origRect, newRect, image.NChannels(), IPPI_INTER_SUPER, &bufferSize);
	pBuffer = ippsMalloc_8u(bufferSize);

	if (image.NChannels() == 1)
	{
		ippiResizeSqrPixel_8u_C1R( image.DataPtr(),
			image.Size(), image.Step(), origRect,
			resizedImage.DataPtr(),
			resizedImage.Step(), newRect,
			r.xFactor_, r.yFactor_, r.xCenter_, r.yCenter_, IPPI_INTER_SUPER, pBuffer);
	}
	else if (image.NChannels() == 3)
	{
		ippiResizeSqrPixel_8u_C3R( image.DataPtr(),
			image.Size(), image.Step(), origRect,
			resizedImage.DataPtr(),
			resizedImage.Step(), newRect,
			r.xFactor_, r.yFactor_, 0, 0, IPPI_INTER_SUPER, pBuffer);
	}
	else if (image.NChannels() == 4)
	{
		ippiResizeSqrPixel_8u_C4R( image.DataPtr(),
			image.Size(), image.Step(), origRect,
			resizedImage.DataPtr(),
			resizedImage.Step(), newRect,
			r.xFactor_, r.yFactor_, r.xCenter_, r.yCenter_, IPPI_INTER_SUPER, pBuffer);
	}


	CStdFileOutput out;
	out.Open(outFile);
	params.quality = 75;
	params.comment_size = 0;
	
	SaveImageJPEG(resizedImage, params, out);

	return 0;
}
