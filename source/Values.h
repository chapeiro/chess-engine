/*
 * Values.h
 *
 *  Created on: 2 ��� 2011
 *      Author: Chrysogelos Periklis
 */

#ifndef VALUES_H_
#define VALUES_H_

namespace Value{
	const int MAT = 1 << 20;
	extern const int piece[12];
	extern const int knightSq[64];
	extern const int kingSq[64];
	extern const int WpawnSq[64];
	extern const int BpawnSq[64];
	const int rookOnOpenFile = 50;
};

#define mated(x) ((x) <= (100-Value::MAT))
#define isMat(x) ((abs(x)) >= (Value::MAT-100))
const int inf = Value::MAT << 1;
#endif /* VALUES_H_ */
