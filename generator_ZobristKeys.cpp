/*
 * gener_ZobristKeys.cpp
 *
 *  Created on: 20 ���� 2011
 *      Author: User
 */
#include "zobristKeys.h"
#include <vector>
#include <time.h>
#include <algorithm>
#include <stdio.h>
#include "randomc.h"
#include "cchapeiro.h"
using namespace std;

key getRandomLong(vector<key> & a, CRandomMersenne & rdm){
	while (true){
		key b = (((key) rdm.BRandom()) << 32) | rdm.BRandom();
		if (b!=0 && find(a.begin(), a.end(), b)==a.end()){
			a.push_back(b);
			return b;
		}
	}
}

void generateZobristKeys() {
	FILE * pd;
	pd = fopen("pd_ZobristKeys.cpp", "w");
	char ct[20];
	time_t rawtime;
	time(&rawtime);
	strftime(ct, 20, "%Y/%m/%d %H:%M:%S", localtime(&rawtime));
	fprintf(pd, "/**   Auto-generated file.\n");
	fprintf(pd, " * Contains pre-computed data for CChapeiro.\n");
	fprintf(pd, " * Generated : %s\n", ct);
	fprintf(pd, " **/\n\n");
	fprintf(pd, "#include \"Board.h\"\n");
	fprintf(pd, "#include \"cchapeiro.h\"\n");
	fprintf(pd, "#include \"zobristKeys.h\"\n\n");
	vector<key> a;
	a.push_back(0);
	CRandomMersenne rdm(time(0));
	fprintf(pd, "const key zobrist::keys[64][12] = {\n");
	for (int i = 0; i < 64; i++) {
		fprintf(pd, "{");
		for (int k = 0; k < 12; k++) {
			fprintf(pd, formatBitboard, getRandomLong(a, rdm));
			if (k != 11) fprintf(pd, ",");
		}
		fprintf(pd, "}");
		if (i != 63) fprintf(pd, ",");
		fprintf(pd, "\n");
	}
	fprintf(pd, "};\n\n");
	fprintf(pd, "const key zobrist::enPassant[8] = {\n");
	for (int i = 0; i < 8; i++) {
		fprintf(pd, formatBitboard, getRandomLong(a, rdm));
		if (i != 7) fprintf(pd, ",");
		fprintf(pd, "\n");
	}
	fprintf(pd, "};\n\n");
	fprintf(pd, "const key zobrist::blackKey = ");
	fprintf(pd, formatBitboard, getRandomLong(a, rdm));
	fprintf(pd, ";\n");
	fprintf(pd, "const key zobrist::White_King_Castling = ");
	fprintf(pd, formatBitboard, getRandomLong(a, rdm));
	fprintf(pd, ";\n");
	fprintf(pd, "const key zobrist::White_Queen_Castling = ");
	fprintf(pd, formatBitboard, getRandomLong(a, rdm));
	fprintf(pd, ";\n");
	fprintf(pd, "const key zobrist::Black_King_Castling = ");
	fprintf(pd, formatBitboard, getRandomLong(a, rdm));
	fprintf(pd, ";\n");
	fprintf(pd, "const key zobrist::Black_Queen_Castling = ");
	fprintf(pd, formatBitboard, getRandomLong(a, rdm));
	fprintf(pd, ";\n");
	fclose(pd);
}
