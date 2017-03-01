void rgb2yuv420p(std::vector<BYTE>& rgb, std::vector<BYTE>& yuv420p)
{
  unsigned int i = 0;
  unsigned int numpixels = width * height;
  unsigned int ui = numpixels;
  unsigned int vi = numpixels + numpixels / 4;
  unsigned int s = 0;

#define sR (BYTE)(rgb[s+2])
#define sG (BYTE)(rgb[s+1])
#define sB (BYTE)(rgb[s+0])

  yuv420p.resize(numpixels * 3 / 2);

  for (int j = 0; j < height; j++)
    for (int k = 0; k < width; k++)
    {
      yuv420p[i] = (BYTE)( (66*sR + 129*sG + 25*sB + 128) >> 8) + 16;

      if (0 == j%2 && 0 == k%2)
      {
        yuv420p[ui++] = (BYTE)( (-38*sR - 74*sG + 112*sB + 128) >> 8) + 128;
        yuv420p[vi++] = (BYTE)( (112*sR - 94*sG - 18*sB + 128) >> 8) + 128;
      }
      i++;
      s += colors;
    }
}