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


	CUnsignedBitNumber32 ubn1 = CUnsignedBitNumber32::CreateFromHexadecimalStringPriorityLSB( std::string( "CE6FFFEB" ) );
	CUnsignedBitNumber32 ubn2 = CUnsignedBitNumber32::CreateFromHexadecimalStringPriorityLSB( std::string( "CE6FEFEB" ) );

	CUnsignedBitNumber32 ubn_xor = ubn1.logical_xor( ubn2 );


	ubn_xor.selfUpdate_not( );
	ubn_xor.selfUpdate_not( );

	//	ubn_xor.raw = ubn1.raw ^ ubn2.raw;

	printf( "%s\n", ubn1.toHexadecimalString( ).c_str( ) );
	printf( "%s\n", ubn2.toHexadecimalString( ).c_str( ) );
	printf( "%s\n", (ubn_xor << 4).toHexadecimalString( ).c_str( ) );
	

	CUnsignedBitNumber32 ubn3 = CUnsignedBitNumber32::CreateFromHexadecimalStringPriorityLSB( std::string( "1" ) );


	for ( size_t i = 0; i < 32; i++ ) {
		printf( "ubn3 << %02zu  = %s\n" , i,  Conv::CreateSeparatedStringWithZeroPadded( (ubn3 << i).toBinaryString( ) , 32 , 4).c_str( ) ); 
	}

	printf( "\n" );
	printf( "\n" );

	CUnsignedBitNumber32 ubn4;

	ubn4.set( 31 );


	for ( size_t i = 0; i < 32; i++ ) {
		printf( "ubn4 >> %02zu  = %s\n", i, Conv::CreateSeparatedStringWithZeroPadded( ( ubn4 >> i ).toBinaryString( ), 32, 4 ).c_str( ) );
	}


	return 0;
}




