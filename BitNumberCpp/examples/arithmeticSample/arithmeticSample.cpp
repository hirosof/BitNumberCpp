#define NOMINMAX
#include <locale>
#include "../../common/CUnsignedBitNumber.hpp"
#include <vector>
#include <atlstr.h>
#include <utility>
#include <algorithm>

template <size_t BitSize> using ResultItem = std::pair<std::wstring, std::optional<CUnsignedBitNumberW<BitSize>>>;
template <size_t BitSize> using ResultArray = std::vector<ResultItem<BitSize>>;

template <size_t BitSize> void bitNumberProcess ( ResultArray <BitSize>& res, const  CUnsignedBitNumberW<BitSize>& num1, const  CUnsignedBitNumberW<BitSize>& num2 ) {

	static_assert( BitSize > 0, "ビットサイズは1以上必要です。" );

	res.push_back( ResultItem<BitSize>( L"num1 + num2", num1 + num2 ) );

	res.push_back( ResultItem<BitSize>( L"num1 - num2", num1 - num2 ) );
	res.push_back( ResultItem<BitSize>( L"num2 - num1", num2 - num1 ) );

	res.push_back( ResultItem<BitSize>( L"num1 * num2", num1 * num2 ) );

	auto n1_dr_n2 = num1.divisionWithRemainder( num2 );

	if ( n1_dr_n2.has_value( ) ) {
		res.push_back( ResultItem<BitSize>( L"num1 / num2", n1_dr_n2->first ) );
		res.push_back( ResultItem<BitSize>( L"num1 % num2", n1_dr_n2->second ) );
	} else {
		res.push_back( ResultItem<BitSize>( L"num1 / num2", std::nullopt) );
		res.push_back( ResultItem<BitSize>( L"num1 % num2", std::nullopt ) );
	}

	auto n2_dr_n1 = num2.divisionWithRemainder( num1 );

	if ( n2_dr_n1.has_value( ) ) {
		res.push_back( ResultItem<BitSize>( L"num2 / num1", n2_dr_n1->first ) );
		res.push_back( ResultItem<BitSize>( L"num2 % num1", n2_dr_n1->second ) );
	} else {
		res.push_back( ResultItem<BitSize>( L"num2 / num1", std::nullopt ) );
		res.push_back( ResultItem<BitSize>( L"num2 % num1", std::nullopt ) );
	}
}

template <size_t BitSize> std::wstring runProcess(  size_t numberOfSubCases = 1, size_t max_value_bin_digits_num1 = BitSize, size_t max_value_bin_digits_num2 = BitSize ) {

	std::vector< ResultArray<BitSize>> respack;

	ResultArray<BitSize> currentRes;

	CUnsignedBitNumberW<BitSize>  num1, num2;


	size_t valid_random_digits_num1 = std::max( static_cast<size_t>( 1 ), std::min( max_value_bin_digits_num1, BitSize ) );
	size_t valid_random_digits_num2 = std::min( max_value_bin_digits_num2, BitSize );


	for ( size_t i = 0; i < numberOfSubCases; i++ ) {

		currentRes.clear( );

		num1.selfUpdateRandom( 0  , valid_random_digits_num1);

		if ( valid_random_digits_num2 == 0 ) num2.clear( );
		else num2.selfUpdateRandom( 0  , valid_random_digits_num2);

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
	result.AppendFormat( L"\t\t\"Number Of SubCases\":%zu,\n", respack.size() );

	size_t real_bin_len  , padd_bin_len_of_block , padd_hex_len_of_block;

	for ( size_t resindex = 0; resindex < respack.size( ); resindex++ ) {
		result.AppendFormat( L"\t\t\"SubCase %zu\":{\n", resindex+1 );

		for ( auto it = respack[resindex].begin( ); it != respack[resindex].end( ); it++ ) {

			result.AppendFormat( L"\t\t\t\"%s\":", it->first.c_str( ) );

			if ( it->second.has_value( ) ) {
				result.AppendFormat( L"{\n" );

				real_bin_len = it->second->getNumberOfBinaryDigitsForDisplay( );
				padd_bin_len_of_block = ( real_bin_len + 3 ) / 4;
				padd_hex_len_of_block = (padd_bin_len_of_block +1) / 2;

				result.AppendFormat( L"\t\t\t\t\"%s\":\"%s\",\n",
					L"bin",
					Conv::CreateSeparatedStringWithZeroPadded( it->second->template toBinaryString<>( ),
						4 * padd_bin_len_of_block, 4 ).c_str( )
				);

				result.AppendFormat( L"\t\t\t\t\"%s\":\"%s\",\n",
					L"dec",
					Conv::CreateCommaSeparatedString( it->second->template toDecimalString<>( ), 3 ).c_str( )
				);

				result.AppendFormat( L"\t\t\t\t\"%s\":\"%s\"\n",
					L"hex",
					Conv::CreateSeparatedStringWithZeroPadded( it->second->template toHexadecimalString<>( true ),
						2*padd_hex_len_of_block, 4 ).c_str( )
				);

				result.AppendFormat( L"\t\t\t}" );

			} else {

				result.AppendFormat( L"\"(値なし)\"" );

			}



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

	return std::wstring( result.GetString( ));
}

template <size_t BitSize> std::wstring runProcessFullAndHalf( size_t numberOfSubCases = 1 ) {
	return runProcess<BitSize>( numberOfSubCases, BitSize, BitSize / 2 );
}

template <size_t BitSize> std::wstring runProcessFullAndZero( size_t numberOfSubCases = 1 ) {
	return runProcess<BitSize>( numberOfSubCases, BitSize, 0 );
}

template <size_t BitSize> std::wstring runProcessHalfAndHalf( size_t numberOfSubCases = 1 ) {
	return runProcess<BitSize>( numberOfSubCases, BitSize / 2, BitSize / 2 );
}



using WStringPair = std::pair<std::wstring, std::wstring>;
using WStringPairVector = std::vector<WStringPair>;

template <size_t BitSize> void runBitUnitProcess( WStringPairVector& ret, std::wstring title_base , size_t numberOfSubCases ) {

	if ( numberOfSubCases == 0 ) return;
	ret.push_back( WStringPair( title_base + L" (Dual Full Digits)", runProcess<BitSize>( numberOfSubCases ) ) );
	ret.push_back( WStringPair( title_base + L" (Full Digits And Zero)", runProcessFullAndZero<BitSize>( numberOfSubCases ) ));
	ret.push_back( WStringPair( title_base + L" (Full Digits And Half Digits)", runProcessFullAndHalf<BitSize>( numberOfSubCases ) ));
	ret.push_back( WStringPair( title_base + L" (Dual Half Digits)", runProcessHalfAndHalf<BitSize>( numberOfSubCases ) ));
}


int main( ) {

	// 日本語ロケールに設定
	std::setlocale( LC_ALL, "ja-JP.UTF-8" );

	WStringPairVector results;
	size_t commonNumberOfSubCases = 2;

	if ( commonNumberOfSubCases == 0 ) {
		wprintf( L"ビットごとに発行するサブケース数が0です。" );
		return 0;
	}

	runBitUnitProcess<8>( results, L"8 bit", commonNumberOfSubCases );
	runBitUnitProcess<16>( results, L"16 bit", commonNumberOfSubCases );
	runBitUnitProcess<24>( results, L"24 bit", commonNumberOfSubCases );
	runBitUnitProcess<32>( results, L"32 bit", commonNumberOfSubCases );
	runBitUnitProcess<64>( results, L"64 bit", commonNumberOfSubCases );
	runBitUnitProcess<128>( results, L"128 bit", commonNumberOfSubCases );
	runBitUnitProcess<256>( results, L"256 bit", commonNumberOfSubCases );

	wprintf( L"{\n" );

	wprintf( L"\t\"Example Name\":\"%s\",\n", L"arithmeticSample" );
	wprintf( L"\t\"Example Comment\":\"%s\",\n", L"各計算結果はBitSizeのビット数を上限とした固定ビット幅に基づく算出結果となります。" );

	for ( auto it = results.begin( ); it != results.end( ); it++ ) {

		wprintf( L"\t\"Case %td : %s\":%s", std::distance( results.begin( ), it ) + 1, it->first.c_str(), it->second.c_str( ) );

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