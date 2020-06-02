#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<strings.h>

#define PLEN 46

#define BIT(x, y) ((x>>y)&0x1)
int de_hamm(uint8_t x)
{
	return BIT(x,1) | (BIT(x,3)<<1) | (BIT(x,5)<<2) | (BIT(x,7)<<3);
}


uint8_t rev(uint8_t b)
{
	return  BIT(b,0)<<7 | BIT(b,1)<<6 | BIT (b,2)<<5 | BIT(b,3)<<4 |
		BIT(b,4)<<3 | BIT(b,5)<<2 | BIT (b,6)<<1 | BIT(b,7);
}

int main(int argc, char *argv[])
{
	uint8_t packet[42];
	while (fread(packet, 1,sizeof(packet), stdin)>0) {
		int mpag=de_hamm(packet[1])<<4 | de_hamm(packet[0]);
		int magazine=mpag&0x7;
		int row=mpag>>3;
		int start=2;
		printf("%d %02d ", magazine, row);
		if (row==0) {
			int page=de_hamm(packet[3])<<4 | de_hamm(packet[2]);
			int sub=(de_hamm(packet[4])) | (de_hamm(packet[6])<<4) | (de_hamm(packet[6])<<8) | (de_hamm(packet[7])<<12);
			int subpage=sub&0x3f7f;
			printf("%02x-%04x ", page, subpage);
			start=8;
		}
		int n;
		for (n=start; n<42; n++) {
			int c=(packet[n])&0x7f;
			if ( (c>' ') && (c<0x7f) ) printf("%c", c); else printf(" ");
		}

		printf("\n");
		
	}
}
