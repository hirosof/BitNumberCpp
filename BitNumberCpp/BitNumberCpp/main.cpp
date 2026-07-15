#define NOMINMAX
#include <windows.h>
#include <locale>
#include <vector>
#include<random>
#include "../common/CUnsignedBitNumber.hpp"

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


	CUnsignedBitNumber32  n128;
	size_t fs = 1;

	for ( size_t offset = 0; offset < 64; offset++ ) {
		auto info = n128.selfUpdateRandomExtend( offset, fs , CUnsignedBitNumber32::OffsetBasis::Most, false );

		n128.clear( );
		n128.rangeSet( info.offset_of_least, info.fill_bit_size );

		printf( "\t%02zu : %s\n", offset, Conv::CreateSeparatedStringWithZeroPadded( n128.toBinaryString<char>( ), 32, 4 ).c_str( ) );
	}
	printf( "\n" );
	for ( size_t offset = 0; offset < 64; offset++ ) {
		auto info = n128.selfUpdateRandomExtend( offset, fs , CUnsignedBitNumber32::OffsetBasis::Least, false );

		n128.clear( );
		n128.rangeSet( info.offset_of_least, info.fill_bit_size );

		printf( "\t%02zu : %s\n", offset ,Conv::CreateSeparatedStringWithZeroPadded( n128.toBinaryString<char>( ), 32, 4 ).c_str());

	}


	CUnsignedBitNumber32  r32;

	r32.selfUpdateRandomExtend( 0, 8, CUnsignedBitNumber32::OffsetBasis::Least );
	r32.selfUpdateRandomExtend( 8, 8, CUnsignedBitNumber32::OffsetBasis::Most, true );


	printf( "%08X\n",
		r32.toUInt32( )
	);

	r32.selfUpdateRandomExtend( 0, 8, CUnsignedBitNumber32::OffsetBasis::Least );
	r32.selfUpdateRandomExtend( 8, 8, CUnsignedBitNumber32::OffsetBasis::Most, true );


	printf( "%08X\n",
		r32.toUInt32( )
	);


	return 0;
}




