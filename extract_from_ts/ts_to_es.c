#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

#define PLEN (188)


uint8_t reverse_bits(uint8_t x)
{
	int n;
	uint8_t byte=0;
	for (n=0; n<8; n++) {
		uint8_t b=(x>>n)&0x1;
		byte=(byte<<1)|b;
	}
	return byte;
}

#define BIT(x, y) ((x>>y)&0x1)

int de_hamm(uint8_t x)
{
	x=reverse_bits(x);
	return BIT(x,1) | (BIT(x,3)<<1) | (BIT(x,5)<<2) | (BIT(x,7)<<3);
}

int main(int argc, char **argv)
{
	if (argc!=2) {
		fprintf(stderr,"Usage %s <pid>\n", argv[0]);
		return 1;
	}
	char *pointer=NULL;
	int wanted_pid=strtol(argv[1], &pointer, 0);
	if ( (pointer==NULL) || (*pointer!=0) ) {
		fprintf(stderr,"Invalid pid %s\n", argv[1]);
	}
	fprintf(stderr, "Wanted PID: 0x%x\n", wanted_pid);
	uint8_t buf[PLEN];
//	while (read(0, buf, PLEN)==PLEN) {
	while (fread(buf, PLEN, 1, stdin)>0) {
		uint32_t header=buf[0]<<24 | buf[1]<<16 | buf[2]<<8 | buf[3];
		int sync=(header>>24) & 0xff;
		int transport_error_indicator=(header >> 23) & 0x1;
		int payload_unit_start_indicator=(header >> 22) & 0x1;
		int transport_priority=(header >> 21) &0x1;
		int pid=(header >> 8) & 0x1fff;
		int transport_scrambling_control=(header >>6) &0x3;
		int adaption_field_control=(header >> 4) & 0x3;
		int continuity_counter=(header >>0) & 0xf;

//		fprintf(stderr, "PID: %d\n", pid);

		if (pid!=wanted_pid) continue;
		int start=4;
//		printf("%02x, %04x adaption_field_control=%d PUSI=%d\n", sync, pid, adaption_field_control, payload_unit_start_indicator);
		if (adaption_field_control==3) { //Adaption field present
			int adaption_field_length=buf[4];
			start=start+adaption_field_length+1;
//			printf("Adaption field length %d\n", adaption_field_length);
		}
		fwrite(&(buf[start]), PLEN-start, 1, stdout);
	}	
}
