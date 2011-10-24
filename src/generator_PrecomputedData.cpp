/*
 * PrecomputedDataGenerator.cpp
 *
 *  Created on: 18 ��� 2011
 *      Author: Chrysogelos Periklis
 */

#include "cchapeiro.h"
#include "SquareMapping.h"
#include "MagicsAndPrecomputedData.h"
#include "Utilities.cpp"
#include <stdio.h>
#include <iostream>
using namespace std;

void precomputeData(){
	FILE * pd;
	pd = fopen("precompd_PrecomputedData.cpp", "w");
	char ct[20];
	time_t rawtime;
	time(&rawtime);
	strftime (ct, 20, "%Y/%m/%d %H:%M:%S", localtime(&rawtime));
	fprintf(pd, "/**   Auto-generated file.\n");
	fprintf(pd, " * Contains pre-computed data for CChapeiro.\n");
	fprintf(pd, " * Generated : %s\n", ct);
	fprintf(pd, " **/\n\n");
	fprintf(pd, "#include \"Board.h\"\n");
	fprintf(pd, "#include \"MagicsAndPrecomputedData.h\"\n\n");
	std::cout << ndbgline << "Generating De Bruijn's number and bitscan ..." << endl;
	CGenBitScan a;
	a.generateBitScan(pd);
	std::cout << ndbgline << "Generating squares, files and ranks ..." << endl;
	bitboard fnormal[64];
	bitboard frank[8], ffile[8];
	bitboard notFnormal[64];
	bitboard notFrank[8], notFfile[8];
	for (int y = 0 ; y < 8 ; ++y){
		for (int x = 0 ; x < 8 ; ++x){
			fnormal[index(x, y)] = bitboard(1) << index(x, y);
			notFnormal[index(x, y)] = ~(fnormal[index(x, y)]);
		}
	}
	frank[0] = bitboard(1);
	frank[0] |= frank[0] << 1;
	frank[0] |= frank[0] << 2;
	frank[0] |= frank[0] << 4;
	for (int y = 1 ; y < 8 ; ++y) frank[y] = frank[y-1] << 8;
	for (int y = 0 ; y < 8 ; ++y) notFrank[y] = ~(frank[y]);
	ffile[7] = bitboard(1);
	ffile[7] |= ffile[7] << 8;
	ffile[7] |= ffile[7] << 16;
	ffile[7] |= ffile[7] << 32;
	for (int y = 6 ; y >= 0 ; --y) ffile[y] = ffile[y+1] << 1;
	for (int y = 7 ; y >= 0 ; --y) notFfile[y] = ~(ffile[y]);
	fprintf(pd, "\nconst bitboard filled::normal[64] = {\n\t");
	for (int i = 0 ; i < 64 ; ++i){
		fprintf(pd, formatBitboard, fnormal[i]);
		if (i != 63) {
			fprintf(pd, ", ");
			if ((i&7)==7) fprintf(pd, "\n\t");
		}
	}
	fprintf(pd, "\n};\n\n");
	fprintf(pd, "const bitboard notFilled::normal[64] = {\n\t");
	for (int i = 0 ; i < 64 ; ++i){
		fprintf(pd, formatBitboard, notFnormal[i]);
		if (i != 63) {
			fprintf(pd, ", ");
			if ((i&7)==7) fprintf(pd, "\n\t");
		}
	}
	fprintf(pd, "\n};\n\n");
	fprintf(pd, "\nconst bitboard filled::rank[8] = {\n\t");
	for (int i = 0 ; i < 8 ; ++i){
		fprintf(pd, formatBitboard, frank[i]);
		if (i != 7) {
			fprintf(pd, ", ");
		} else {
			fprintf(pd, "\n");
		}
	}
	fprintf(pd, "\n};\n\n");
	fprintf(pd, "const bitboard notFilled::rank[8] = {\n\t");
	for (int i = 0 ; i < 8 ; ++i){
		fprintf(pd, formatBitboard, notFrank[i]);
		if (i != 7) {
			fprintf(pd, ", ");
		} else {
			fprintf(pd, "\n");
		}
	}
	fprintf(pd, "\n};\n\n");
	fprintf(pd, "\nconst bitboard filled::file[8] = {\n\t");
	for (int i = 0 ; i < 8 ; ++i){
		fprintf(pd, formatBitboard, ffile[i]);
		if (i != 7) {
			fprintf(pd, ", ");
		} else {
			fprintf(pd, "\n");
		}
	}
	fprintf(pd, "\n};\n\n");
	fprintf(pd, "const bitboard notFilled::file[8] = {\n\t");
	for (int i = 0 ; i < 8 ; ++i){
		fprintf(pd, formatBitboard, notFfile[i]);
		if (i != 7) {
			fprintf(pd, ", ");
		} else {
			fprintf(pd, "\n");
		}
	}
	fprintf(pd, "\n};\n\n");
	bitboard KnightMoves[64], KingMoves[64], rays[64][64];
	for (int i = 0 ; i < 64 ; ++i){
		bitboard b = bitboard(1) << i;
		KnightMoves[i] = (
			( ((b >> 15) | (b << 17)) & notFfile[7] ) |
			( ((b << 15) | (b >> 17)) & notFfile[0] ) |
			( ((b >> 6)  | (b << 10)) & notFfile[7] & notFfile[6]) |
			( ((b >> 10) | (b << 6))  & notFfile[0] & notFfile[1])
					);
		KingMoves[i] = (
			( ( ( b << 1) | ( b << 9 ) | ( b >> 7 ) ) & notFfile[7]) |
			( ( ( b >> 1) | ( b >> 9 ) | ( b << 7 ) ) & notFfile[0]) |
			(   ( b << 8) | ( b >> 8 ) )
					);
	}
	std::cout << ndbgline << "Generating Knight's Moves..." << endl;
	fprintf(pd, "const bitboard KnightMoves[64] = {\n\t");
	for (int i = 0 ; i < 64 ; ++i){
		fprintf(pd, formatBitboard, KnightMoves[i]);
		if (i != 63) {
			fprintf(pd, ", ");
			if ((i&7)==7) fprintf(pd, "\n\t");
		}
	}
	fprintf(pd, "\n};\n\n");
	std::cout << ndbgline << "Generating King's Moves..." << endl;
	fprintf(pd, "const bitboard KingMoves[64] = {\n\t");
	for (int i = 0 ; i < 64 ; ++i){
		fprintf(pd, formatBitboard, KingMoves[i]);
		if (i != 63) {
			fprintf(pd, ", ");
			if ((i&7)==7) fprintf(pd, "\n\t");
		}
	}
	fprintf(pd, "\n};\n");
	std::cout << ndbgline << "Generating Rays..." << endl;
	bitboard b, a1;
	for (int i = 0 ; i < 64 ; ++i){
		rays[i][i] = 0;
		for (int j = i + 1 ; j < 64 ; ++j){
			if (rank(i) == rank(j)){
				rays[i][j] = rays[j][i] = (1ull << j) - (1ull << (i + 1));
			} else if (file(i) == file(j)){
				rays[i][j] = rays[j][i] = ffile[file(i)] & ((1ull << j) - (1ull << (i + 1)));
			} else if (rank(i) + file(i) == rank(j) + file(j)){
				a1 = 1ull << (i + 9);
				b = 1ull << j;
				rays[j][i] = 0;
				while (a1 < b){
					rays[j][i] |= a1;
					a1 <<= 9;
				}
				rays[i][j] = rays[j][i];
			} else if (rank(i) - file(i) == rank(j) - file(j)){
				a1 = 1ull << (i + 7);
				b = 1ull << j;
				rays[j][i] = 0;
				while (a1 < b){
					rays[j][i] |= a1;
					a1 <<= 7;
				}
				rays[i][j] = rays[j][i];
			} else {
				rays[j][i] = rays[i][j] = 0;
			}
		}
	}
	fprintf(pd, "const bitboard rays[64][64] = {\n#define _____________________ 0ULL\n\t");
	for (int i = 0 ; i < 64 ; ++i){
		fprintf(pd, "{\n\t\t");
		for (int j = 0 ; j < 64 ; ++j){
			if (rays[i][j] != 0){
				fprintf(pd, formatBitboard, rays[i][j]);
			} else {
				fprintf(pd, "_____________________");
			}
			if (j != 63) {
				fprintf(pd, ", ");
				if ((j&7)==7) fprintf(pd, "\n\t\t");
			}
		}
		if (i != 63) fprintf(pd, "\n\t},\n\t");
	}
	fprintf(pd, "\n\t}\n};");
	fclose(pd);
	std::cout << ndbgline << "Generating Magics For Rooks and Bishops..." << endl;
	FILE * out;
	out = fopen ("precompd_magics.cpp", "w");
	MagicGenerator(maxRookBits, maxBishopBits, out);
	fclose(out);
}
