void itoh(unsigned int n,char *s)
{
    char base[16] = {'0','1','2','3',
                     '4','5','6','7',
                     '8','9','A','B',
                     'C','D','E','F'};
   s[7]=base[n&0x0f];
   s[6]=base[(n>>4)&0x0f];
   s[5]=base[(n>>8)&0x0f];
   s[4]=base[(n>>12)&0x0f];
   s[3]=base[(n>>16)&0x0f];
   s[2]=base[(n>>20)&0x0f];
   s[1]=base[(n>>24)&0x0f];
   s[0]=base[(n>>28)&0x0f];
   s[8]='\0';
}
