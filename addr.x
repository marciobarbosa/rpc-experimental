struct address {
	short flag;
	unsigned int addr_1;
	unsigned int addr_2;
	unsigned int addr_3;
	unsigned int addr_4;	
};

program ADDRESSPROG {
	version ADDRESSVERS {
		int PRINTADDRESS(struct address) = 1;
	} = 1;
} = 99;