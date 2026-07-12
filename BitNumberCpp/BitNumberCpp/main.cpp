#define NOMINMAX
#include <windows.h>
#include <locale>
#include <vector>
#include<random>
#include "../common/CUnsignedBitNumber.hpp"

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

	arithmeticSample( number1, number2 );	
}



template <size_t BitSize>  void arithmeticSample( const CUnsignedBitNumberA<BitSize>& number1, const CUnsignedBitNumberA<BitSize>& number2 ) {
	using Pair = std::pair<std::string, CUnsignedBitNumberA<BitSize>>;
	using PairVector = std::vector<Pair>;

	PairVector result_items;

	result_items.push_back( Pair( "number1", number1 ) );
	result_items.push_back( Pair( "number2", number2 ) );
	result_items.push_back( Pair( "number1 + number2", number1 + number2 ) );
	result_items.push_back( Pair( "number1 - number2", number1 - number2 ) );
	result_items.push_back( Pair( "number2 - number1", number2 - number1 ) );

	result_items.push_back( Pair( "number1 * number2", number1 * number2 ) );


	auto temp_div_remain = number1.divisionWithRemainder( number2 );
	if ( temp_div_remain.has_value( ) ) {
		result_items.push_back( Pair( "number1 / number2", temp_div_remain->first ) );
		result_items.push_back( Pair( "number1 % number2", temp_div_remain->second ) );
	}

	temp_div_remain = number2.divisionWithRemainder( number1 );
	if ( temp_div_remain.has_value( ) ) {
		result_items.push_back( Pair( "number2 / number1", temp_div_remain->first ) );
		result_items.push_back( Pair( "number2 % number1", temp_div_remain->second ) );
	}

	size_t  num_of_result_items = result_items.size( );

	if ( num_of_result_items > 0 ) {

		printf( "```json\n{\n" );

		printf( "\t\"BitSize\":\"%zu\",\n", BitSize );
		for ( size_t i = 0; i < num_of_result_items; i++ ) {

			printf( "\t\"%s\":%s", result_items[i].first.c_str( ), result_items[i].second.toJsonLikedString<char>( true ).c_str( ) );

			if ( ( i + 1 ) != num_of_result_items ) {
				printf( "," );
			}
			printf( "\n" );
		}
		printf( "}\n```\n\n" );
	}
}

template <size_t BitSize>  void arithmeticSampleRandom( void ) {

	CUnsignedBitNumberA<BitSize>  number1, number2;

	std::random_device rd;
	std::mt19937 re( rd() );

	std::uniform_int_distribution<int>   dist( 0, 1 );


	// number1 のセットするビット数は BitSize固定で発行
	for ( size_t i = 0; i < BitSize; i++ ) {
		number1.raw[i] = ( dist( re ) == 1 );
	}

	// number2 のセットするビット数は 可変で発行
	std::uniform_int_distribution<int>   dist_n2_set_size( BitSize / 2, BitSize );
	size_t size2 = dist_n2_set_size( re );
	for ( size_t i = 0; i < size2; i++ ) {
		number2.raw[i] = ( dist( re ) == 1 );
	}

	arithmeticSample( number1, number2 );

}


void arithmeticSampleMain( void ) {

	printf( "### 8 bit\n\n" );
	arithmeticSampleRandom<8>( );
	arithmeticSampleRandom<8>( );

	printf( "### 16 bit\n\n" );
	arithmeticSampleRandom<16>( );
	arithmeticSampleRandom<16>( );

	printf( "### 24 bit\n\n" );
	arithmeticSampleRandom<24>( );
	arithmeticSampleRandom<24>( );

	printf( "### 32 bit\n\n" );
	arithmeticSampleRandom<32>( );
	arithmeticSampleRandom<32>( );

	printf( "### 64 bit\n\n" );
	arithmeticSampleRandom<64>( );
	arithmeticSampleRandom<64>( );

	printf( "### 128 bit\n\n" );
	arithmeticSampleRandom<128>( );
	arithmeticSampleRandom<128>( );

}

int main( ) {

	// 日本語ロケールに設定
	setlocale( LC_ALL, "Japanese" );

	// UTF-8コードページに変更
	SetConsoleOutputCP( CP_UTF8 );

	using Conv = CStdBitsetUnsignedStringConversion<char>;

	auto a  = Conv::FromHexadecimalStringPriorityLSBStrict<18>( "FFFF3", Conv::OperationForInvalidCharDetected::SkipContinue);

	printf( "ic = %zu\n", a.countOfInvalidChars );

	for ( auto item : a.invalidCharMap ) {

		printf( "\t%c : ", item.first );

		for ( auto vi : item.second ) {
			printf( "%zu ", vi );
		}

		printf( "\n" );
	}
	
	printf( "\n\n\n%s\n", CUnsignedBitNumberA( a.value ).toJsonLikedString( ).c_str( ) );



	CUnsignedBitNumberA<18> n( a.value );

	printf( "\n\n\n%s\n", n.toJsonLikedString( ).c_str( ) );




	return 0;
}




