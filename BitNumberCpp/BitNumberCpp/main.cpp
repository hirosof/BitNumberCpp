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


int main( ) {

	// 日本語ロケールに設定
	setlocale( LC_ALL, "Japanese" );

	// UTF-8コードページに変更
	SetConsoleOutputCP( CP_UTF8 );

	using Conv = CStdBitsetUnsignedStringConversion<char>;


	CStdBitsetUnsignedNumber<64> n1;
	CStdBitsetUnsignedNumber<32> n2;	

	/*
	n1.getStdBitsetRef( ) = CStdBitsetUnsignedOperation::Random<64>( );

	CBitNumberSupport::BitOffsetBasis basis = CBitNumberSupport::BitOffsetBasis::Most;

	for ( size_t i = 0; i < 16; i++ ) {
		for ( size_t j = 0; j < 16; j++ ) {
			auto ret = CBitNumberSupport::AdjustOffsetRange( 16, i, j + 1,basis );

			if ( ret.has_value( ) ) {
				printf( "%zu %zu %s => %zu %zu %s\n",  i , j +1, ( basis == CBitNumberSupport::BitOffsetBasis::Least ) ? "Least" : "Most",
					ret->offset, ret->size, ( ret->basis == CBitNumberSupport::BitOffsetBasis::Least ) ? "Least" : "Most" );
			}
		}

		printf( "\n" );
	}
	*/


	n2.fromUInt16ToMost( 0x2a1b );

	n1.fromCast( n2, 4, 0, CBitNumberSupport::BitOffsetBasis::Most);


	printf( "n2(No Padding): %s\n", Conv::ToHexadecimalString( n1.getStdBitsetRefConst( ), false, Conv::ZeroPaddingMode::NoPadding ).c_str( ) );
	printf( "n2(Eight Bits Padding): %s\n", Conv::ToHexadecimalString( n1.getStdBitsetRefConst( ), false, Conv::ZeroPaddingMode::EightBitsPadding ).c_str( ) );
	printf( "n2(Container Bits Padding): %s\n", Conv::ToHexadecimalString( n1.getStdBitsetRefConst( ), false, Conv::ZeroPaddingMode::ContainerBitsPadding ).c_str( ) );
	printf( "n2(Container And Eight Bits Padding): %s\n", Conv::ToHexadecimalString( n1.getStdBitsetRefConst( ), false, Conv::ZeroPaddingMode::ContainerAndEightBitsPadding ).c_str( ) );

	printf( "%u %x\n", n1.toUInt8(8 , CBitNumberSupport::BitOffsetBasis::Most ),
		n1.toUInt8( 8, CBitNumberSupport::BitOffsetBasis::Most ) );



	return 0;
}




