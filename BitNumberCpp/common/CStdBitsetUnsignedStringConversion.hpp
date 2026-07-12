
/*
以下にまとめた内容をベースに文字列変換関連のクラス実装化 + 追加独自実装あり
https://gist.github.com/hirosof/2dad279fc120d476a7079506cfab2572

開発環境：Visual Studio 2026
*/


#pragma once

#include <cstdint>
#include <string>
#include <map>
#include <vector>
#include "CStdBitsetUnsignedOperation.hpp"

template <typename CharT> class CStdBitsetUnsignedStringConversion {
public:
	using String = std::basic_string<CharT>;

	template<size_t BitSize> using StdBitset = CStdBitsetUnsignedOperation::StdBitset<BitSize>;
	template<size_t BitSize> using OptionalStdBitset = CStdBitsetUnsignedOperation::OptionalStdBitset<BitSize>;
	template<size_t BitSize> using StdBitsetConstRef = CStdBitsetUnsignedOperation::StdBitsetConstRef<BitSize>;
	template <size_t BitSize> using OptionalStdBitsetPair = CStdBitsetUnsignedOperation::OptionalStdBitsetPair<BitSize>;


	inline static const String DEFAULT_VALID_SEPARATORS = String( { ' ', ',' } );

	// From系の関数で無効文字検出時の動作モード
	enum struct OperationForInvalidCharDetected {

		// 部分パース(成功している部分までの値)の結果を返す
		// 例：10進数文字列として "1234X5678"が指定された場合 → 1234 を表す値が返る
		PartialReturn = 0,

		// 0を表す値を返す
		ZeroValueReturn,

		// '0' が指定したものと解釈し、処理を続行する
		// 例：10進数文字列として "1234X5678"が指定された場合 → 123405678 を表す値が返る
		AssumeZeroContinue,

		// その値を無視して処理を続行する
		// 例：10進数文字列として "1234X5678"が指定された場合 → 12345678 を表す値が返る
		SkipContinue
	};

	using InvalidCharMapType = std::map<CharT, std::vector<size_t>>;

	template<size_t BitSize>	 class ParseResult {
	public:
		StdBitset<BitSize>  value;
		size_t countOfInvalidChars;
		InvalidCharMapType invalidCharMap;

		ParseResult( ) : value( 0 ), countOfInvalidChars( 0 ), invalidCharMap(){
		}
	};


	template<size_t BitSize>	static String ToBinaryString( StdBitsetConstRef<BitSize> bin ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		size_t msb_pos = CStdBitsetUnsignedOperation::GetNumberOfDigitsForDisplay( bin ) - 1;
		String result_string;
		for ( size_t i = 0; i <= msb_pos; i++ ) {
			result_string.push_back( ( bin[msb_pos - i] ) ? '1' : '0' );
		}
		return result_string;
	}

	template<size_t BitSize>	static ParseResult<BitSize> FromBinaryStringPriorityLSBStrict( const String& str, const OperationForInvalidCharDetected operation_invalid_char_detected, const String& valid_separators = DEFAULT_VALID_SEPARATORS ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );

		ParseResult<BitSize> result;
		size_t bit_pos = 0;
		CharT c;

		for ( auto rev_it = str.rbegin( ); ( rev_it != str.rend( ) ) && ( bit_pos < BitSize ); rev_it++ ) {
			c = *rev_it;
			if ( c == '0' ) {
				result.value[bit_pos] = false;
				bit_pos++;
			} else if ( c == '1' ) {
				result.value[bit_pos] = true;
				bit_pos++;
			} else  if ( valid_separators.find( c ) != String::npos ) {
				continue;
			} else {

				result.countOfInvalidChars++;

				result.invalidCharMap[c].push_back( str.length( ) - std::distance( str.rbegin( ), rev_it ) - 1 );

				switch ( operation_invalid_char_detected ) {
					case OperationForInvalidCharDetected::PartialReturn:
						return result;
					case OperationForInvalidCharDetected::ZeroValueReturn:
						result.value = StdBitset<BitSize>( 0 );
						return result;
					case OperationForInvalidCharDetected::AssumeZeroContinue:
						result.value[bit_pos] = false;
						bit_pos++;
						break;
					case OperationForInvalidCharDetected::SkipContinue:
						break;
				}
			}
		}
		return result;
	}

	template<size_t BitSize>	static ParseResult<BitSize> FromBinaryStringStrict( const String& str, const OperationForInvalidCharDetected operation_invalid_char_detected, const String& valid_separators = DEFAULT_VALID_SEPARATORS ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );

		ParseResult<BitSize> result;
		size_t current_bit_size = 0;
		CharT c;

		for ( auto it = str.begin( ); ( it != str.end( ) ) && ( current_bit_size < BitSize ); it++ ) {
			c = *it;
			if ( c == '0' ) {
				result.value <<= 1;
				result.value[0] = false;
				current_bit_size++;
			} else if ( c == '1' ) {
				result.value <<= 1;
				result.value[0] = true;
				current_bit_size++;
			} else  if ( valid_separators.find( c ) != String::npos ) {
				continue;
			} else {

				result.countOfInvalidChars++;

				result.invalidCharMap[c].push_back( std::distance( str.begin( ), it ) );

				switch ( operation_invalid_char_detected ) {
					case OperationForInvalidCharDetected::PartialReturn:
						return result;
					case OperationForInvalidCharDetected::ZeroValueReturn:
						result.value = StdBitset<BitSize>( 0 );
						return result;
					case OperationForInvalidCharDetected::AssumeZeroContinue:
						result.value <<= 1;
						result.value[0] = false;
						current_bit_size++;
						break;
					case OperationForInvalidCharDetected::SkipContinue:
						break;
				}
			}
		}
		return result;
	}

	template<size_t BitSize>	static StdBitset<BitSize> FromBinaryStringPriorityLSB( const String& str, const String& valid_separators = DEFAULT_VALID_SEPARATORS ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		ParseResult<BitSize> pr = FromBinaryStringPriorityLSBStrict<BitSize>( str, OperationForInvalidCharDetected::PartialReturn, valid_separators );
		return pr.value;
	}

	template<size_t BitSize>	static StdBitset<BitSize> FromBinaryString( const String& str, const String& valid_separators = DEFAULT_VALID_SEPARATORS ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		ParseResult<BitSize> pr = FromBinaryStringStrict<BitSize>( str, OperationForInvalidCharDetected::PartialReturn, valid_separators );
		return pr.value;
	}


	
	template<size_t BitSize>	static String ToDecimalString( StdBitsetConstRef<BitSize> bin ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		if ( BitSize < 4 ) {
			StdBitset<4> bs( bin.to_ulong( ) );
			return  ToDecimalString( bs );
		}

		String result_string;

		const StdBitset<BitSize>  bit_of_ten( 10 );
		StdBitset<BitSize> auxiliary( bin );
		uint8_t current_digit_value;
		OptionalStdBitsetPair<BitSize> div_10_result;

		do {
			div_10_result = CStdBitsetUnsignedOperation::DivisionWithRemainder( auxiliary, bit_of_ten );
			if ( div_10_result.has_value( ) == false ) return String( );
			auxiliary = div_10_result->first;
			current_digit_value = div_10_result->second.to_ulong( ) & 0xff;
			result_string.push_back( '0' + current_digit_value );
		} while ( auxiliary.none( ) == false );

		std::reverse( result_string.begin( ), result_string.end( ) );
		return result_string;
	}

	template <size_t BitSize> static  StdBitset<BitSize>  FromDecimalString( const String& str, const String& valid_separators = DEFAULT_VALID_SEPARATORS ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		ParseResult<BitSize> parsed = FromDecimalStringStrict< BitSize>( str, OperationForInvalidCharDetected::PartialReturn, valid_separators );
		return parsed.value;
	}

	template <size_t BitSize> static  ParseResult<BitSize>  FromDecimalStringStrict( const String& str, const OperationForInvalidCharDetected operation_invalid_char_detected ,  const String& valid_separators = DEFAULT_VALID_SEPARATORS ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		ParseResult<BitSize> result;

		if ( BitSize < 4 ) {
			ParseResult<4> pr4 = FromDecimalStringStrict<4>( str, operation_invalid_char_detected, valid_separators );
			result.countOfInvalidChars = pr4.countOfInvalidChars;
			result.invalidCharMap = std::move( pr4.invalidCharMap );
			result.value = CStdBitsetUnsignedOperation::CastSize<BitSize, 4>( pr4.value );
			return result;
		}

		CharT c;
		const StdBitset<BitSize>  bit_of_ten( 10 );
		StdBitset<BitSize> current_digit_bitset( 0 );
		uint8_t current_digit_value;

		for ( auto it = str.begin( ); it != str.end( ) ; it++ ) {
			c = *it;

			if ( c >= '0' && c <= '9' ) {
				current_digit_value = c - '0';
				current_digit_bitset[0] = current_digit_value & 1;
				current_digit_bitset[1] = ( current_digit_value >> 1 ) & 1;
				current_digit_bitset[2] = ( current_digit_value >> 2 ) & 1;
				current_digit_bitset[3] = ( current_digit_value >> 3 ) & 1;
				result.value = CStdBitsetUnsignedOperation::Multiplication( result.value, bit_of_ten );
				result.value = CStdBitsetUnsignedOperation::Addition( result.value, current_digit_bitset );
			} else if ( valid_separators.find( c ) == String::npos ) {

				result.countOfInvalidChars++;

				result.invalidCharMap[c].push_back( std::distance( str.begin( ), it ) );

				switch ( operation_invalid_char_detected ) {
					case OperationForInvalidCharDetected::PartialReturn:
						return result;
					case OperationForInvalidCharDetected::ZeroValueReturn:
						result.value = StdBitset<BitSize>( 0 );
						return result;
					case OperationForInvalidCharDetected::AssumeZeroContinue:
						result.value = CStdBitsetUnsignedOperation::Multiplication( result.value, bit_of_ten );
						break;
					case OperationForInvalidCharDetected::SkipContinue:
						break;
				}
			}

		}

		return result;
	}


	template<size_t BitSize>	static String ToHexadecimalString( StdBitsetConstRef<BitSize> bin, bool upper_case = false ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		if ( BitSize < 4 ) {
			StdBitset<4> bs( bin.to_ulong( ) );
			return  ToHexadecimalString( bs, upper_case );
		}
		String result_string;
		StdBitset<BitSize> auxiliary( bin );
		uint8_t current_digit_value;

		do {
			current_digit_value = auxiliary[0] | ( auxiliary[1] << 1 ) | ( auxiliary[2] << 2 ) | ( auxiliary[3] << 3 );
			auxiliary >>= 4;
			if ( current_digit_value < 10 ) result_string.push_back( '0' + current_digit_value );
			else result_string.push_back( ( ( upper_case ) ? 'A' : 'a' ) + ( current_digit_value - 10 ) );
		} while ( auxiliary.none( ) == false );

		std::reverse( result_string.begin( ), result_string.end( ) );
		return result_string;
	}

	template <size_t BitSize> static  StdBitset<BitSize>  FromHexadecimalString( const String& str, const String& valid_separators = DEFAULT_VALID_SEPARATORS ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		ParseResult<BitSize> parsed = FromHexadecimalStringStrict< BitSize>( str, OperationForInvalidCharDetected::PartialReturn, valid_separators );
		return parsed.value;
	}

	template <size_t BitSize> static  ParseResult<BitSize>  FromHexadecimalStringStrict( const String& str, const OperationForInvalidCharDetected operation_invalid_char_detected, const String& valid_separators = DEFAULT_VALID_SEPARATORS ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		ParseResult<BitSize> result;

		if ( BitSize < 4 ) {
			ParseResult<4> pr4 = FromHexadecimalStringStrict<4>( str, operation_invalid_char_detected, valid_separators );
			result.countOfInvalidChars = pr4.countOfInvalidChars;
			result.invalidCharMap = std::move( pr4.invalidCharMap );
			result.value = CStdBitsetUnsignedOperation::CastSize<BitSize, 4>( pr4.value );
			return result;
		}

		uint8_t current_digit_value = 0;		
		size_t slen = str.length( );

		size_t maxBlocks = BitSize / 4;
		const size_t finalRestBits = BitSize % 4;

		if ( finalRestBits != 0 ) maxBlocks++;

		CharT c;

		size_t currentBlockBits = 4;

		for ( size_t i = 0 , blocks = 0 ;  (i < slen) && (blocks < maxBlocks) ; i++ ) {

			c = str[i];

			if ( c >= '0' && c <= '9' ) {
				current_digit_value = c - '0';
			} else if ( c >= 'A' && c <= 'F' ) {
				current_digit_value = 10 + c - 'A';
			} else if ( c >= 'a' && c <= 'f' ) {
				current_digit_value = 10 + c - 'a';
			} else  if ( valid_separators.find( c ) != String::npos ) {
				continue;
			} else {
				result.countOfInvalidChars++;
				result.invalidCharMap[c].push_back( i );
				switch ( operation_invalid_char_detected ) {
					case OperationForInvalidCharDetected::PartialReturn:
						return result;
					case OperationForInvalidCharDetected::ZeroValueReturn:
						result.value = StdBitset<BitSize>( 0 );
						return result;
					case OperationForInvalidCharDetected::AssumeZeroContinue:
						current_digit_value = 0;
						break;
					case OperationForInvalidCharDetected::SkipContinue:
						continue;
				}
			}

			blocks++;
			if ( (blocks == maxBlocks) &&(finalRestBits !=0) ) currentBlockBits = finalRestBits;

			result.value <<= currentBlockBits;

			for ( size_t bid = 0; bid < currentBlockBits; bid++ ) {
				result.value[currentBlockBits - 1 - bid] = ( current_digit_value >> ( 3 - bid ) ) & 1;
			}

		}


		return result;
	}


	template <size_t BitSize> static  StdBitset<BitSize>  FromHexadecimalStringPriorityLSB( const String& str, const String& valid_separators = DEFAULT_VALID_SEPARATORS ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		ParseResult<BitSize> parsed = FromHexadecimalStringPriorityLSBStrict< BitSize>( str, OperationForInvalidCharDetected::PartialReturn, valid_separators );
		return parsed.value;
	}
	template <size_t BitSize> static  ParseResult<BitSize>  FromHexadecimalStringPriorityLSBStrict( const String& str, const OperationForInvalidCharDetected operation_invalid_char_detected, const String& valid_separators = DEFAULT_VALID_SEPARATORS ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		ParseResult<BitSize> result;

		if ( BitSize < 4 ) {
			ParseResult<4> pr4 = FromHexadecimalStringPriorityLSBStrict<4>( str, operation_invalid_char_detected, valid_separators );
			result.countOfInvalidChars = pr4.countOfInvalidChars;
			result.invalidCharMap = std::move( pr4.invalidCharMap );
			result.value = CStdBitsetUnsignedOperation::CastSize<BitSize, 4>( pr4.value );
			return result;
		}

		uint8_t current_digit_value = 0;
		size_t slen = str.length( );

		size_t maxBlocks = BitSize / 4;
		const size_t finalRestBits = BitSize % 4;

		if ( finalRestBits != 0 ) maxBlocks++;

		CharT c;

		size_t currentBlockBits = 4;
		size_t realIndex;

		for ( size_t i = 0, blocks = 0; ( i < slen ) && ( blocks < maxBlocks ); i++ ) {

			realIndex = slen - 1 - i;
			c = str[realIndex];

			if ( c >= '0' && c <= '9' ) {
				current_digit_value = c - '0';
			} else if ( c >= 'A' && c <= 'F' ) {
				current_digit_value = 10 + c - 'A';
			} else if ( c >= 'a' && c <= 'f' ) {
				current_digit_value = 10 + c - 'a';
			} else  if ( valid_separators.find( c ) != String::npos ) {
				continue;
			} else {
				result.countOfInvalidChars++;
				result.invalidCharMap[c].push_back( realIndex );
				switch ( operation_invalid_char_detected ) {
					case OperationForInvalidCharDetected::PartialReturn:
						return result;
					case OperationForInvalidCharDetected::ZeroValueReturn:
						result.value = StdBitset<BitSize>( 0 );
						return result;
					case OperationForInvalidCharDetected::AssumeZeroContinue:
						current_digit_value = 0;
						break;
					case OperationForInvalidCharDetected::SkipContinue:
						continue;
				}
			}

			blocks++;
			if ( ( blocks == maxBlocks ) && ( finalRestBits != 0 ) ) currentBlockBits = finalRestBits;

			for ( size_t bid = 0; bid < currentBlockBits; bid++ ) {
				result.value[4 * ( blocks - 1 ) + bid] = ( current_digit_value >> bid ) & 1;
			}

		}

		return result;
	}



	static String CreateSeparatedString( const String& str, size_t  group_size = 3, const CharT separate_char = ' ' ) {
		if ( group_size == 0 ) return str;
		String s;
		for ( size_t i = 0; i < str.length( ); i++ ) {
			s.push_back( str[str.length( ) - 1 - i] );
			if ( ( i + 1 ) != str.length( ) ) {
				if ( ( ( i + 1 ) % group_size ) == 0 ) {
					s.push_back( separate_char );
				}
			}
		}
		std::reverse( s.begin( ), s.end( ) );
		return s;
	}

	static String CreateSpaceSeparatedString( const String& str, size_t  group_size = 3 ) {
		return CreateSeparatedString( str, group_size, ' ' );
	}

	static String CreateCommaSeparatedString( const String& str, size_t  group_size = 3 ) {
		return CreateSeparatedString( str, group_size, ',' );
	}

	static String CreateSeparatedStringWithPadded( const String& str, size_t  size_of_padded_str_min_length, const CharT padding_char, size_t  group_size = 3, const CharT separate_char = ' ' ) {
		return CreateSeparatedString(
			CreatePaddedString( str, size_of_padded_str_min_length, padding_char ),
			group_size,
			separate_char
		);
	}

	static String CreateSeparatedStringWithZeroPadded( const String& str, size_t  size_of_padded_str_min_length, size_t  group_size = 3, const CharT separate_char = ' ' ) {
		return CreateSeparatedString(
			CreateZeroPaddedString( str, size_of_padded_str_min_length ),
			group_size,
			separate_char
		);
	}

	static String CreateSeparatedStringWithSpacePadded( const String& str, size_t  size_of_padded_str_min_length, size_t  group_size = 3, const CharT separate_char = ' ' ) {
		return CreateSeparatedString(
			CreateSpacePaddedString( str, size_of_padded_str_min_length ),
			group_size,
			separate_char
		);
	}


	static String CreatePaddedString( const String& str, size_t  size_of_padded_str_min_length, const CharT padding_char ) {
		size_t str_len = str.length( );
		if ( str_len >= size_of_padded_str_min_length ) return str;
		size_t padding_size = size_of_padded_str_min_length - str_len;
		String s( padding_size, padding_char );
		s.append( str );
		return s;
	}

	static String CreateZeroPaddedString( const String& str, size_t  size_of_padded_str_min_length ) {
		return CreatePaddedString( str, size_of_padded_str_min_length, '0' );
	}

	static String CreateSpacePaddedString( const String& str, size_t  size_of_padded_str_min_length ) {
		return CreatePaddedString( str, size_of_padded_str_min_length, ' ' );
	}


};