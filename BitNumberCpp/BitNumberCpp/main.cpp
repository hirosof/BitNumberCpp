/*
	このファイルは動作チェック用のコードです。
*/


#define NOMINMAX
#include <windows.h>
#include <locale>
#include <vector>
#include<random>
#include "../common/CUnsignedBitNumber.hpp"
#include "../common/CStdBitsetUnsignedNumber.hpp"
#include "../common/CBitNumberSupport.hpp"
/*
enum struct NumberType {
	Binary = 0,
	Decimal,
	Hexadecimal
};

template <size_t BitSize>  void arithmeticSample( const std::string number1String, const std::string number2String, const NumberType numType = NumberType::Hexadecimal ) {


	CUnsignedBitNumberA<BitSize>  number1, number2;


	switch ( numType ) {
		case NumberType::Binary:
			number1.fromBinaryString( number1String );
			number2.fromBinaryString( number2String );
			break;
		case NumberType::Decimal:
			number1.fromDecimalString( number1String );
			number2.fromDecimalString( number2String );
			break;
		case NumberType::Hexadecimal:
			number1.fromHexadecimalString( number1String );
			number2.fromHexadecimalString( number2String );
			break;
	}

}

*/

template<size_t BitSize, typename CharType = char> void DumpStdBitsetUnsignedNumber( const CStdBitsetUnsignedNumber<BitSize, CharType>& number  , bool afterNewLine = false) {

	using Conv = CStdBitsetUnsignedStringConversion<char>;
	printf( "%s (0x%s)" ,
		Conv::ToBinaryString( number.getStdBitsetRefConst( ), Conv::ZeroPaddingMode::ContainerAndEightBitsPadding ).c_str( ),
		Conv::ToHexadecimalString( number.getStdBitsetRefConst( ), true ,  Conv::ZeroPaddingMode::ContainerAndEightBitsPadding ).c_str( )	
	);

	if ( afterNewLine ) {
		printf( "\n" );
	}

}

int main( ) {

	// 日本語ロケールに設定
	setlocale( LC_ALL, "Japanese" );

	// UTF-8コードページに変更
	SetConsoleOutputCP( CP_UTF8 );

	using Conv = CStdBitsetUnsignedStringConversion<char>;


	CStdBitsetUnsignedNumber<16> n1;
	CStdBitsetUnsignedNumber<8> n2;	

	n2.fromUInt8( 0xAB );

	DumpStdBitsetUnsignedNumber( n2, true );	
	printf( "\n" ); 
	for ( size_t from_offset = 0; from_offset < 8; from_offset++ ) {
		for ( size_t to_offset = 0; to_offset < 16; to_offset++ ) {
			n1.fromCast( n2, to_offset, from_offset, CBitNumberSupport::BitOffsetBasis::Most );
			printf("fromCast to_offset: %zu, from_offset: %zu, n1: ", to_offset, from_offset );
			DumpStdBitsetUnsignedNumber( n1, true );
		}
		printf( "\n" );
	}



	return 0;
}




