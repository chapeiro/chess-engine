/*
 * Board.cpp
 *
 *  Created on: 2 ��� 2011
 *      Author: Chrysogelos Periklis
 */

#include "Board.h"
#include <string>
#include "SquareMapping.h"
#include "MoveEncoding.h"
#include "Values.h"

Board::Board(char fenBoard[], char fenPlaying, char fenCastling[], int fenEnPX, int fenEnPY, int fenHC, int fenFM){
	//General initialize
	for (int i = 0 ; i < PIECESMAX ; ++i) Pieces[i] = bitboard(0);
	castling = 0;
	zobr = 0;
	lastHistoryEntry = 0;
	int wk (0), bk(0);
	//FEN processing
	//	according to :
	//	http://en.wikipedia.org/w/index.php?title=Forsyth%E2%80%93Edwards_Notation&oldid=396200404
	// 1. Piece Placement
	int index1 = 0;
	int x, ind, sq;
	char inp;
	for (int y = 7 ; y >= 0 ; --y){
		x = 0;
		do {
			inp = fenBoard[index1++];
			if (inp < 'A'){
				x += inp-'0';
			} else {
				ind = getPieceIndex(inp);
				if (ind == (KING | white)){
					++wk;
				} else if (ind == (KING | black)){
					++bk;
				}
				sq = index(x, y);
				updatePieces(sq, ind);
				++x;
			}
		} while (x <= 7);
		++index1;
	}
	if (wk!=1) throw 1;
	if (bk!=1) throw 2;
	// 2. Active color
	if (fenPlaying=='b'){
		playing = black;
		zobr ^= zobrist::blackKey;
	} else {
		playing = white;
	}
	// 3. Castling availability
	ind = 0;
	while (fenCastling[ind] != '\0'){
		switch (fenCastling[ind++]){
		case 'K' :
			castling |= castlingc<white>::KingSide;
			zobr ^= zobrist::White_King_Castling;
			break;
		case 'Q' :
			castling |= castlingc<white>::QueenSide;
			zobr ^= zobrist::White_Queen_Castling;
			break;
		case 'k' :
			castling |= castlingc<black>::KingSide;
			zobr ^= zobrist::Black_King_Castling;
			break;
		case 'q' :
			castling |= castlingc<black>::QueenSide;
			zobr ^= zobrist::Black_Queen_Castling;
			break;
		}
	}
	// 4. En passant target square in algebraic notation
	if (fenEnPX < 0 || fenEnPX > 7 || fenEnPY < 0 || fenEnPY > 7){
		enPassant = bitboard(0);
	} else {
		enPassant = filled::normal[index(fenEnPX, fenEnPY)];
		zobr ^= zobrist::enPassant[fenEnPX];
	}
	// 5. Halfmove clock
	halfmoves = fenHC;
	// 6. Fullmove number
	fullmoves = fenFM;
	//FEN processing ended
	addToHistory(zobr);
}

int Board::getWhitePieceIndex(char p){
	const char PiecesNameSort[] = {'P', 'N', 'B', 'R', 'Q', 'K'};
	for (int i = 0 ; i < (LASTPIECE >> 1) ; ++i){
		if (PiecesNameSort[i] == p) return (i << 1) | white;
	}
	return WRONG_PIECE;
}

void Board::capture(int to){
	for (int i = playing^1 ; i < PIECESMAX ; i+=2){
		if ((Pieces[i] & filled::normal[to])!=0){
			updatePieces(to, i);
			return;
		}
	}
}

/**
 * ATTENTION!: move m has to be LEGAL (or null move)
 */
void Board::make(move m){
	if (moveIsNull(m)){
		togglePlaying();
		return;
	}
	int from = index(m.fromX, m.fromY);
	int to = index(m.toX, m.toY);
	if ((Pieces[PAWN | playing] & filled::normal[from])!=0){
		//is Pawn
		int lastRank;
		if (playing==white){
			lastRank = lastRank_w;
		} else {
			lastRank = lastRank_b;
		}
		if ((Pieces[CPIECES | playing] & filled::normal[to])!=0){
			//capture
			updatePieces(from, playing | PAWN);
			if (rank(to)==lastRank){
				int a = getWhitePieceIndex(m.promoteTo);
				if (a == WRONG_PIECE) a = QUEEN;
				updatePieces(to, a | playing);
			} else {
				updatePieces(to, playing | PAWN);
			}
			capture(to);
			enPassant = bitboard(0);
		} else {
			if (from-to==index(0, 0)-index(0, 1) || to-from==index(0, 0)-index(0, 1)){
				//normal
				updatePieces(from, playing | PAWN);
				if (rank(to)==lastRank){
					int a = getWhitePieceIndex(m.promoteTo);
					if (a == WRONG_PIECE) a = QUEEN;
					updatePieces(to, a | playing);
				} else {
					updatePieces(to, playing | PAWN);
				}
				enPassant = bitboard(0);
			} else if (from-to==index(0, 0)-index(0, 2) || to-from==index(0, 0)-index(0, 2)){
				//double
				updatePieces(from, playing | PAWN);
				updatePieces(to, playing | PAWN);
				enPassant = filled::normal[(from+to)/2];
				zobr ^= zobrist::enPassant[7&square( enPassant )];
			} else {
				//en passant
				updatePieces(from, playing | PAWN);
				updatePieces(to, playing | PAWN);
				updatePieces(index(file(to), rank(from)), (playing^1) | PAWN);
				enPassant = bitboard(0);
			}
		}
		halfmoves = 0;
		if (playing==black) ++fullmoves;
		togglePlaying();
	} else if ((Pieces[KING | playing] & filled::normal[from])!=0){
		//is King
		bool castl = false;
		if (playing == white){
			if (from == index('e'-'a', '1'-'1') && to == index('g'-'a', '1'-'1')){
				castl = true;
			} else if (from == index('e'-'a', '1'-'1') && to == index('c'-'a', '1'-'1')){
				castl = true;
			}
		} else {
			if (from == index('e'-'a', '8'-'1') && to == index('g'-'a', '8'-'1')){
				castl = true;
			} else if (from == index('e'-'a', '8'-'1') && to == index('c'-'a', '8'-'1')){
				castl = true;
			}
		}
		if (castl) {
			updatePieces(from, ROOK | playing);
			updatePieces((from+to)/2, ROOK | playing);
		}
		updatePieces(from, KING | playing);
		updatePieces(to, KING | playing);
		capture(to);
		if (playing==black) ++fullmoves;
		if (playing==white){
			deactivateCastlingRights<white>();
		} else {
			deactivateCastlingRights<black>();
		}
		togglePlaying();
		enPassant = bitboard(0);
	} else {
		for (int i = playing ; i < LASTPIECE ; i+=2){
			if ((Pieces[i] & filled::normal[from])!=0){
				updatePieces(from, i);
				updatePieces(to, i);
				if (i-playing==ROOK && (filled::normal[from] & allcastlingrights) != 0){
						zobr ^= zobrist::castling[(castling*castlingsmagic)>>59];
						castling ^= filled::normal[from];
						zobr ^= zobrist::castling[(castling*castlingsmagic)>>59];
				}
				break;
			}
		}
		capture(to);
		if (playing==black) ++fullmoves;
		togglePlaying();
		enPassant = bitboard(0);
	}
	addToHistory(zobr);
}

/**
 * Use with caution, slow method
 *	Only for use in debugging.
 **/
std::string Board::getFEN(){
	//A FEN record contains six fields.
	//The separator between fields is a space.
	//	The fields are:
	std::string fen = "";
	const char PiecesNameSort[] = {'P', 'N', 'B', 'R', 'Q', 'K'};
	//	1. Piece placement (from white's perspective).
	//		Each rank is described, starting with rank 8 and
	//		ending with rank 1; within each rank, the contents
	//		of each square are described from file a through
	//		file h. Following the Standard Algebraic Notation (SAN),
	//		each piece is identified by a single letter taken
	//		from the standard English names (pawn = "P",
	//		knight = "N", bishop = "B", rook = "R", queen = "Q"
	//		and king = "K").[1] White pieces are designated
	//		using upper-case letters ("PNBRQK") while Black
	//		take lowercase ("pnbrqk"). Blank squares are noted
	//		using digits 1 through 8 (the number of blank
	//		squares), and "/" separate ranks.
	for (int y = 7 ; y >= 0 ; --y){
		int count = 0;
		for (int x = 0 ; x < 8 ; ++x){
			bitboard a = filled::normal[index(x, y)];
			if ((Pieces[CPIECES | white] & a) !=0){
				if (count != 0){
					fen += count+'0';
					count = 0;
				}
				for (int i = white ; i < LASTPIECE ; i+=2){
					if ((Pieces[i] & a) != 0){
						fen += PiecesNameSort[i>>1];
					}
				}
			} else if ((Pieces[CPIECES | black] & a) !=0){
				if (count != 0){
						fen += count+'0';
						count = 0;
				}
				for (int i = black ; i < LASTPIECE ; i+=2){
					if ((Pieces[i] & a) != 0){
						fen += PiecesNameSort[i>>1]-'A'+'a';
					}
				}
			} else {
				++count;
			}
		}
		if (count != 0) fen += count+'0';
		if (y != 0) fen += '/';
	}
	fen += ' ';
	//	2. Active color. "w" means white moves next, "b" means black.
	fen += ( playing==white ? 'w' : 'b' );
	fen += ' ';
	//	3. Castling availability. If neither side can castle,
	//		this is "�". Otherwise, this has one or more letters:
	//		"K" (White can castle kingside),
	//		"Q" (White can castle queenside),
	//		"k" (Black can castle kingside) and/or
	//		"q" (Black can castle queenside).
	bool ncr = true;
	if ((castling & castlingc<white>::KingSide) != 0) {
		fen += 'K';
		ncr = false;
	}
	if ((castling & castlingc<white>::QueenSide) != 0){
		fen += 'Q';
		ncr = false;
	}
	if ((castling & castlingc<black>::KingSide) != 0) {
		fen += 'k';
		ncr = false;
	}
	if ((castling & castlingc<black>::QueenSide) != 0){
		fen += 'q';
		ncr = false;
	}
	if (ncr) fen += '-';
	fen += ' ';
	//	4. En passant target square in algebraic notation.
	if (enPassant==0ull){
		//	If there's no en passant target square, this is "�".
		fen += '-';
	} else {
		//	If a pawn has just made a 2-square move, this is the
		//	position "behind" the pawn. This is recorded
		//	regardless of whether there is a pawn in position
		//	to make an en passant capture.
		for (int i = 0 ; i < 8 ; ++i){
			if (enPassant == filled::normal[index(i, 5)]){
				fen += i+'a';
				fen += '6';
				break;
			}
			if (enPassant == filled::normal[index(i, 2)]){
				fen += i+'a';
				fen += '3';
				break;
			}
		}
	}
	fen += ' ';
	// 5. Halfmove clock: This is the number of halfmoves since
	//		the last pawn advance or capture. This is used to
	//		determine if a draw can be claimed under the fifty-move rule.
	char num[8];
	sprintf(num, "%d", halfmoves);
	fen += num;
	fen += ' ';
	// 6. Fullmove number: The number of the full move.
	//		It starts at 1, and is incremented after Black's move.
	sprintf(num, "%d", fullmoves);
	fen += num;
	return fen;
}

/**
 * Use with caution, slow method (if debugcc==true)
 **/
void Board::print(){
	if (debugcc){
		std::cout << ndbgline << "--------------------------------\n";
		std::cout << ndbgline << getFEN() << '\n';
		for (int i = white ; i < LASTPIECE ; i+=2){
			std::cout << ndbgline << "White " << PiecesName[i>>1] << ": \n";
			printbb(Pieces[i]);
		}
		for (int i = black ; i < LASTPIECE ; i+=2){
			std::cout << ndbgline << "Black " << PiecesName[i>>1] << ": \n";
			printbb(Pieces[i]);
		}
		std::cout << ndbgline << "Zobrist Key: " << zobr << '\n';
		std::cout << ndbgline << "--------------------------------" << std::endl;
	}
}

void Board::printbb(bitboard bb){
	int i;
	for (int y = 7 ; y >= 0 ; --y){
		std::cout << ndbgline;
		for (int x = 0 ; x < 8 ; ++x){
			i = index(x, y);
			std::cout << ((bb & filled::normal[i]) >> i);
		}
		std::cout << '\n';
	}
}

U64 Board::perft(int depth){
	horizonNodes = 0;
	if (playing == white){
		search<Perft, white>(-inf, +inf, depth);
	} else {
		search<Perft, black>(-inf, +inf, depth);
	}
	return horizonNodes;
}

bitboard Board::bishopAttacks(bitboard occ, const int &sq){
	occ &= BishopMask[sq];
	occ *= BishopMagic[sq];
#ifndef fixedShift
	occ >>= BishopShift[sq];
#else
	occ >>= 64-maxBishopBits;
#endif
	return BishopAttacks[sq][occ];
}

bitboard Board::rookAttacks(bitboard occ, const int &sq){
	occ &= RookMask[sq];
	occ *= RookMagic[sq];
#ifndef fixedShift
	occ >>= RookShift[sq];
#else
	occ >>= 64-maxRookBits;
#endif
	return RookAttacks[sq][occ];
}

bitboard Board::queenAttacks(bitboard occ, const int &sq){
	return rookAttacks(occ, sq) | bishopAttacks(occ, sq);
}
