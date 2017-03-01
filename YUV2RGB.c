static void YUV2RGB(const unsigned char y, const unsigned char u,const unsigned char v, unsigned char* r,
                unsigned char* g, unsigned char* b)
{
   const int y2 = (int)y;
   const int u2 = (int)u - 128;
   const int v2 = (int)v - 128;
   //std::cerr << "YUV=("<<y2<<","<<u2<<","<<v2<<")"<<std::endl;

   // This is the normal YUV conversion, but
   // appears to be incorrect for the firewire cameras
   //   int r2 = y2 + ( (v2*91947) >> 16);
   //   int g2 = y2 - ( ((u2*22544) + (v2*46793)) >> 16 );
   //   int b2 = y2 + ( (u2*115999) >> 16);
   // This is an adjusted version (UV spread out a bit)

   int r2 = y2 + ((v2 * 37221) >> 15);
   int g2 = y2 - (((u2 * 12975) + (v2 * 18949)) >> 15);
   int b2 = y2 + ((u2 * 66883) >> 15);
   //std::cerr << "   RGB=("<<r2<<","<<g2<<","<<b2<<")"<<std::endl;

   // Cap the values.
   *r = CLIPVALUE(r2);
   *g = CLIPVALUE(g2);
   *b = CLIPVALUE(b2);
}