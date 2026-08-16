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

	using ZeroPaddingMode = CStdBitsetUnsignedNumber<16>::ZeroPaddingMode;

	printf( "%s" ,
		 number.toJsonLikeString( true, ZeroPaddingMode::ContainerAndEightBitsPadding ).c_str( )
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


	CStdBitsetUnsignedNumber<256> n1;

	for ( size_t i = 0; i < 10; i++ ) {
		n1.selfUpdateRandom( );
//		n1.selfUpdateRandomExtend( 0, 8, CBitNumberSupport::BitOffsetBasis::MSB, true );
		DumpStdBitsetUnsignedNumber( n1, true );
	}

	return 0;
}




