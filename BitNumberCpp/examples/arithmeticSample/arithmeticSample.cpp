#define NOMINMAX
#include <locale>
#include "../../common/CUnsignedBitNumber.hpp"
#include <vector>
#include <atlstr.h>
#include <utility>
#include <algorithm>

template <size_t BitSize> using ResultItem = std::pair<std::wstring, CUnsignedBitNumberW<BitSize>>;
template <size_t BitSize> using ResultArray = std::vector<ResultItem<BitSize>>;

template <size_t BitSize> void bitNumberProcess ( ResultArray <BitSize>& res, const  CUnsignedBitNumberW<BitSize>& num1, const  CUnsignedBitNumberW<BitSize>& num2 ) {
	res.push_back( ResultItem<BitSize>( L"num1 + num2", num1 + num2 ) );

	res.push_back( ResultItem<BitSize>( L"num1 - num2", num1 - num2 ) );
	res.push_back( ResultItem<BitSize>( L"num2 - num1", num2 - num1 ) );

	res.push_back( ResultItem<BitSize>( L"num1 * num2", num1 * num2 ) );

	auto n1_dr_n2 = num1.divisionWithRemainder( num2 );

	if ( n1_dr_n2.has_value( ) ) {
		res.push_back( ResultItem<BitSize>( L"num1 / num2", n1_dr_n2->first ) );
		res.push_back( ResultItem<BitSize>( L"num1 % num2", n1_dr_n2->second ) );
	}

	auto n2_dr_n1 = num2.divisionWithRemainder( num1 );

	if ( n2_dr_n1.has_value( ) ) {
		res.push_back( ResultItem<BitSize>( L"num2 / num1", n2_dr_n1->first ) );
		res.push_back( ResultItem<BitSize>( L"num2 % num1", n2_dr_n1->second ) );
	}
}

template <size_t BitSize> CAtlStringW runProcess(  size_t numberOfSubCases = 1, size_t max_value_bin_digits_num1 = BitSize, size_t max_value_bin_digits_num2 = BitSize ) {

	std::vector< ResultArray<BitSize>> respack;

	ResultArray<BitSize> currentRes;

	CUnsignedBitNumberW<BitSize>  num1, num2;


	size_t valid_random_digits_num1 = std::max( static_cast<size_t>( 1 ), std::min( max_value_bin_digits_num1, BitSize ) );
	size_t valid_random_digits_num2 = std::max( static_cast<size_t>( 1 ), std::min( max_value_bin_digits_num2, BitSize ) );


	for ( size_t i = 0; i < numberOfSubCases; i++ ) {

		currentRes.clear( );

		num1.selfUpdateRandom( 0  , valid_random_digits_num1);
		num2.selfUpdateRandom( 0  , valid_random_digits_num2);

		currentRes.push_back( ResultItem<BitSize>( L"num1", num1 ) );
		currentRes.push_back( ResultItem<BitSize>( L"num2", num2 ) );

		bitNumberProcess ( currentRes, num1, num2 );

		respack.push_back( currentRes );

	}


	using Conv = CStdBitsetUnsignedStringConversion<wchar_t>;


	CAtlStringW result;

	result.Append( L"{\n" );

	result.AppendFormat( L"\t\t\"BitSize\":%zu,\n", BitSize );
	result.AppendFormat( L"\t\t\"num1 max digits\":%zu,\n", valid_random_digits_num1 );
	result.AppendFormat( L"\t\t\"num2 max digits\":%zu,\n", valid_random_digits_num2 );

	size_t real_bin_len , real_hex_len;

	for ( size_t resindex = 0; resindex < respack.size( ); resindex++ ) {
		result.AppendFormat( L"\t\t\"SubCase %zu\":{\n", resindex+1 );

		for ( auto it = respack[resindex].begin( ); it != respack[resindex].end( ); it++ ) {

			result.AppendFormat( L"\t\t\t\"%s\":{\n", it->first.c_str( ) );


			real_bin_len = it->second.getNumberOfBinaryDigitsForDisplay( );
			real_hex_len = ( real_bin_len / 4 ) + ( ( ( real_bin_len % 4 ) != 0 ) ? 1 : 0 );
			
			result.AppendFormat( L"\t\t\t\t\"%s\":\"%s\",\n",
				L"bin",
				Conv::CreateSeparatedStringWithZeroPadded( it->second.template toBinaryString<>( ),
					4 * real_hex_len , 4 ).c_str( )
			);

			result.AppendFormat( L"\t\t\t\t\"%s\":\"%s\",\n",
				L"dec",
				Conv::CreateCommaSeparatedString( it->second.template toDecimalString<>( ), 3 ).c_str( )
			);

			result.AppendFormat( L"\t\t\t\t\"%s\":\"%s\"\n",
				L"hex",
				Conv::CreateSeparatedStringWithZeroPadded( it->second.template toHexadecimalString<>( true ),
					4 * ( ( real_hex_len / 4 ) + ( ( ( real_hex_len % 4 ) != 0 ) ? 1 : 0 ) ) , 4 ).c_str( )
			);



			result.AppendFormat( L"\t\t\t}" );

			auto next_it = it;
			next_it++;
			if ( next_it != respack[resindex].end( ) ) {
				result.AppendFormat( L"," );
			}

			result.AppendFormat( L"\n" );
		}

		result.AppendFormat( L"\t\t}" );

		if ( ( resindex + 1 ) != respack.size( ) ) result.AppendFormat( L"," );

		result.AppendFormat( L"\n" );

	}

	result.Append( L"\t}" );

	return result;
}

template <size_t BitSize> CAtlStringW runProcessFullAndHalf( size_t numberOfSubCases = 1 ) {
	return runProcess<BitSize>( numberOfSubCases, BitSize, BitSize / 2 );
}
template <size_t BitSize> CAtlStringW runProcessHalfAndHalf( size_t numberOfSubCases = 1 ) {
	return runProcess<BitSize>( numberOfSubCases, BitSize / 2, BitSize / 2 );
}


int main( ) {

	// 日本語ロケールに設定
	setlocale( LC_ALL, "Japanese" );


	std::vector<CAtlStringW> results;
	size_t commonNumberOfSubCases = 1;

	results.push_back( runProcess<8>( commonNumberOfSubCases ) );
	results.push_back( runProcessFullAndHalf<8>( commonNumberOfSubCases ) );
	results.push_back( runProcessHalfAndHalf<8>( commonNumberOfSubCases ) );


	
	results.push_back( runProcess<16>( commonNumberOfSubCases ) );
	results.push_back( runProcessFullAndHalf<16>( commonNumberOfSubCases ) );
	results.push_back( runProcessHalfAndHalf<16>( commonNumberOfSubCases ) );

	results.push_back( runProcess<24>( commonNumberOfSubCases ) );
	results.push_back( runProcessFullAndHalf<24>( commonNumberOfSubCases ) );
	results.push_back( runProcessHalfAndHalf<24>( commonNumberOfSubCases ) );

	results.push_back( runProcess<32>( commonNumberOfSubCases ) );
	results.push_back( runProcessFullAndHalf<32>( commonNumberOfSubCases ) );
	results.push_back( runProcessHalfAndHalf<32>( commonNumberOfSubCases ) );


	results.push_back( runProcess<64>( commonNumberOfSubCases ) );
	results.push_back( runProcessFullAndHalf<64>( commonNumberOfSubCases ) );
	results.push_back( runProcessHalfAndHalf<64>( commonNumberOfSubCases ) );

	results.push_back( runProcess<128>( commonNumberOfSubCases ) );
	results.push_back( runProcessFullAndHalf<128>( commonNumberOfSubCases ) );
	results.push_back( runProcessHalfAndHalf<128>( commonNumberOfSubCases ) );

	results.push_back( runProcess<256>( commonNumberOfSubCases ) );
	results.push_back( runProcessFullAndHalf<256>( commonNumberOfSubCases ) );
	results.push_back( runProcessHalfAndHalf<256>( commonNumberOfSubCases ) );


	wprintf( L"{\n" );
	for ( auto it = results.begin( ); it != results.end( ); it++ ) {

		wprintf( L"\t\"Case %zu\":%s", std::distance( results.begin( ), it ) + 1, it->GetString( ) );

		auto next_it = it;
		next_it++;
		if ( next_it != results.end( ) ) {
			wprintf( L"," );
		}

		wprintf( L"\n" );

	}

	wprintf( L"}\n" );


	return 0;
}