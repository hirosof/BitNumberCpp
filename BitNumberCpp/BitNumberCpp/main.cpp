#define NOMINMAX
#include <windows.h>
#include <locale>
#include "CUnsignedBitNumber.hpp"

void func1( void );
void uselevel_from_test_func( void );
void arithmeticTest( void );

template<size_t BitSize, typename BitNumberDefaultCharT> void print_bitNumberWBS( std::string prefix, const CUnsignedBitNumber<BitSize, BitNumberDefaultCharT>& num, bool withNewLine = true ) {

	if ( prefix.length( ) != 0 ) printf( "%s", prefix.c_str( ) );

	printf( " (%zu Bit Type) : %s", BitSize  , num.toJsonLikedString<char>().c_str());

	if ( withNewLine ) printf( "\n" );
}




int main( ) {

	// 日本語ロケールに設定
	setlocale( LC_ALL, "Japanese" );

	// UTF-8コードページに変更
	SetConsoleOutputCP( CP_UTF8 );

	CUnsignedBitNumber16W  base = CUnsignedBitNumber16W::FromHelpers<char>::FromHexadecimalString( "C9DB" );
	CUnsignedBitNumber128 large( base, 60, 0 );

	print_bitNumberWBS( "base", base );
	print_bitNumberWBS( "large", large );
	print_bitNumberWBS( "large-base", large.subtraction(base.toCast<128,char>() ));


	CUnsignedBitNumber16W b2;

	b2.fromCast( base, 0, 8 );

	print_bitNumberWBS( "b2", b2 );



	return 0;
}


void arithmeticTest( void ) {

	CUnsignedBitNumber64 a = CUnsignedBitNumber64::FromHelpers<char>::FromHexadecimalString( "FB21 EF59" );
	CUnsignedBitNumber64 b = CUnsignedBitNumber64::FromHelpers<char>::FromHexadecimalString( "843E CD60" );

	printf( "a : %s\n", a.toJsonLikedString<char>( ).c_str( ) );
	printf( "b : %s\n", b.toJsonLikedString<char>( ).c_str( ) );
	printf( "\n" );
	printf( "a+b : %s\n", a.addition( b ).toJsonLikedString<char>( ).c_str( ) );
	printf( "a-b : %s\n", a.subtraction( b ).toJsonLikedString<char>( ).c_str( ) );
	printf( "a*b : %s\n", a.multiplication( b ).toJsonLikedString<char>( ).c_str( ) );

	auto div_rem = a.divisionWithRemainder( b );
	if ( div_rem.has_value( ) ) {
		printf( "a/b : %s\n", div_rem->first.toJsonLikedString<char>( ).c_str( ) );
		printf( "a%%b : %s\n", div_rem->second.toJsonLikedString<char>( ).c_str( ) );
	}

}

void uselevel_from_test_func( void ) {

	CUnsignedBitNumber64 n64;
	n64.fill( );

	printf( "%I64u : %s\n\n", n64.toUInt64( ), n64.toJsonLikedString<char>( ).c_str( ) );


	uint32_t  iv = 0x86214573;
	CUnsignedBitNumber<32> v;

	for ( size_t offset = 0; offset < 32; offset++ ) {
		v.fromUInt32( iv, offset );
		printf( "%u : ", iv << offset );
		printf( "%s\n", v.toJsonLikedString<char>( ).c_str( ) );
	}


	printf( "-------------------------------\n" );

	CUnsignedBitNumber<16> v16;
	for ( size_t offset = 0; offset < 16; offset++ ) {
		v16.fromUInt32( iv, offset );
		printf( "%u : ", static_cast<uint16_t>( iv << offset ) );
		printf( "%s\n", v16.toJsonLikedString<char>( ).c_str( ) );
	}


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