#define NOMINMAX
#include <windows.h>
#include <locale>
#include "CUnsignedBitNumber.hpp"



void func1( void);



int main( ) {

	// 日本語ロケールに設定
	setlocale( LC_ALL, "Japanese" );

	// UTF-8コードページに変更
	SetConsoleOutputCP( CP_UTF8 );


	CUnsignedBitNumber8 a;

	a.fromDecimalString<char>( "0" );
	for ( size_t i = 0; i < 256; i++ ) {

		printf( "%s : %s\n",
			CStdBitsetUnsignedStringConversion<char>::CreateSeparatedStringWithZeroPadded( a.toBinaryString( ), 8, 4 ).c_str(),
			a.toDecimalString().c_str()
		);

		++a;
	}

	printf( "\n" );

	a.fromDecimalString<char>( "255" );
	for ( size_t i = 0; i < 256; i++ ) {

		printf( "%s : %s\n",
			CStdBitsetUnsignedStringConversion<char>::CreateSeparatedStringWithZeroPadded( a.toBinaryString( ), 8, 4 ).c_str( ),
			a.toDecimalString( ).c_str( )
		);

		--a;
	}

	return 0;
}


void func1( void ) {

	const int bitSize = 32;

	std::bitset<bitSize> a = CStdBitsetUnsignedStringConversion<wchar_t>::FromHexadecimalString<bitSize>( L"1000ADD0", { ' ' } );
	std::bitset<bitSize> b = CStdBitsetUnsignedStringConversion<char>::FromHexadecimalString<bitSize>( "110", { ' ' } );

	auto dc = CStdBitsetUnsignedOperation::DivisionWithRemainder( a, b );

	printf( "%u ÷ %u = ", a.to_ulong( ), b.to_ulong( ) );

	if ( dc.has_value( ) ) {

		printf( "%u あまり %u\n", dc->first.to_ulong( ), dc->second.to_ulong( ) );
	} else {
		printf( "なし\n" );
	}


	printf( "%s\n", CStdBitsetUnsignedStringConversion<char>::ToHexadecimalString( a ).c_str( ) );
	printf( "%S\n", CStdBitsetUnsignedStringConversion<wchar_t>::ToHexadecimalString( b ).c_str( ) );

	std::wstring w( { 'a'  , 'B' , 0 } );

	printf( "%S\n", w.c_str( ) );
}