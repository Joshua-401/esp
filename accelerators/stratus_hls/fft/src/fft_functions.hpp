// Copyright (c) 2011-2019 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0

// complex number multiplication
void compMul(const CompNum &x, const CompNum &y, CompNum &res)
{
    res.re = x.re * y.re - x.im * y.im;
    res.im = x.re * y.im + x.im * y.re;
}

// complex number addition
void compAdd(const CompNum &x, const CompNum &y, CompNum &res)
{
    res.re = x.re + y.re;
    res.im = x.im + y.im;
}

// complex number substraction
void compSub(const CompNum &x, const CompNum &y, CompNum &res)
{
    res.re = x.re - y.re;
    res.im = x.im - y.im;
}

FPDATA myCos(int m)
{
	switch(m) {
	case 1: return 2;
	case 2: return 0.999999940395355;
	case 3: return 0.292893260717392;
	case 4: return 0.0761204659938812;
	case 5: return 0.0192147195339203;
	case 6: return 0.00481527345255017;
	case 7: return 0.00120454386342317;
	case 8: return 0.000301181309623644;
	case 9: return 7.52981650293805e-05;
	case 10: return 1.88247176993173e-05;
	case 11: return 4.70619079351309e-06;
	case 12: return 1.1765483804993e-06;
	case 13: return 2.94137151968243e-07;
	case 14: return 7.35342879920609e-08;
	case 15: return 1.83835719980152e-08;
	case 16: return 4.5958929995038e-09;
	default: return 0.0;
	}
}

FPDATA mySin(int m)
{
	switch(m) {
	case 1: return 8.74227765734759e-08;
	case 2: return -1;
	case 3: return -0.70710676908493;
	case 4: return -0.382683455944061;
	case 5: return -0.1950903236866;
	case 6: return -0.0980171412229538;
	case 7: return -0.0490676760673523;
	case 8: return -0.0245412290096283;
	case 9: return -0.0122715383768082;
	case 10: return -0.00613588467240334;
	case 11: return -0.00306795677170157;
	case 12: return -0.00153398024849594;
	case 13: return -0.000766990357078612;
	case 14: return -0.000383495207643136;
	case 15: return -0.000191747603821568;
	case 16: return -9.58738019107841e-05;
        default:  return 0.0;
	}
}
