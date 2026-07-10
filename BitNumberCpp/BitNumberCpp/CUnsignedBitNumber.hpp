/*
以下にまとめた内容をベースにクラス実装化
https://gist.github.com/hirosof/2dad279fc120d476a7079506cfab2572
*/

#pragma once
#include <bitset>
#include <cstdint>
#include <optional>
#include <algorithm>
#include <utility>
#include <string>
#include <stdexcept>

class CStdBitsetUnsignedOperation {
public:
	template<size_t BitSize> using StdBitset = std::bitset<BitSize>;
	template<size_t BitSize> using StdBitsetPointer = std::bitset<BitSize>*;
	template<size_t BitSize> using StdBitsetConst = const StdBitset<BitSize>;
	template<size_t BitSize> using StdBitsetConstRef = const StdBitset<BitSize>&;

	template <size_t BitSize> using StdBitsetPair = std::pair<std::bitset<BitSize>, std::bitset<BitSize>>;
	template <size_t BitSize> using OptionalStdBitset = std::optional<StdBitset<BitSize>>;
	template <size_t BitSize> using OptionalStdBitsetPair = std::optional<StdBitsetPair<BitSize>>;

	enum struct CompareResult {
		LeftGreater = 0,
		Equal,
		RightGreater
	};

	template< size_t ToBitSize, size_t FromBitSize>  static StdBitset<ToBitSize> CastSize( StdBitsetConstRef<FromBitSize> input ) {
		static_assert( FromBitSize > 0, "FromBitSizeは無効な値です。" );
		static_assert( ToBitSize > 0, "ToBitSizeは無効な値です。" );

		StdBitset<ToBitSize> target;

		size_t copySize = std::min( FromBitSize, ToBitSize );
		for ( size_t i = 0; i < copySize; i++ ) {
			target[i] = input[i];
		}

		return target;
	}

	template<size_t BitSize>  static StdBitset<BitSize> Increment( StdBitsetConstRef<BitSize> input ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );

		StdBitset<BitSize>  ox( input );

		bool A, X, Cn, Cb = true;

		for ( size_t i = 0; ( i < BitSize ) && Cb; i++ ) {

			A = ox[i];

			X = A ^ Cb;
			Cn = A && Cb;

			ox[i] = X;
			Cb = Cn;
		}

		return ox;
	}

	template<size_t BitSize>  static StdBitset<BitSize> Decrement( StdBitsetConstRef<BitSize> input ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );

		StdBitset<BitSize>  ox( input );

		bool A, X, Cn, Cb = false;

		for ( size_t i = 0; i < BitSize; i++ ) {

			A = ox[i];

			X = !( A ^ Cb );
			Cn = A || Cb;

			ox[i] = X;

			Cb = Cn;
		}

		return ox;
	}

	template<size_t BitSize>  static StdBitset<BitSize> Addition( StdBitsetConstRef<BitSize> input_a, StdBitsetConstRef<BitSize> input_b, const bool input_carry = false, bool* const pLastCarry = nullptr ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		std::bitset<BitSize>  ox( 0 );

		bool a, b, x;
		bool c = 0, Cb = input_carry;

		for ( size_t i = 0; i < BitSize; i++ ) {
			a = input_a[i];
			b = input_b[i];

			x = a ^ b ^ Cb;
			c = ( a && ( b ^ Cb ) ) || ( b && Cb );

			ox[i] = x;
			Cb = c;
		}

		if ( pLastCarry ) *pLastCarry = Cb;

		return ox;
	}

	template<size_t BitSize>  static StdBitset<BitSize> Subtraction( StdBitsetConstRef<BitSize> input_a, StdBitsetConstRef<BitSize> input_b) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		std::bitset<BitSize>  ox( 0 );

		bool a, b, x;
		bool c = 0, Cb = false;

		for ( size_t i = 0; i < BitSize; i++ ) {
			a = input_a[i];
			b = input_b[i];

			x = a ^ b ^ Cb;
			c = ( ( !a ) && ( b ^ Cb ) ) || ( b && Cb );

			ox[i] = x;
			Cb = c;
		}

		return ox;
	}


	template<size_t BitSize>  static StdBitset<BitSize> Multiplication( StdBitsetConstRef<BitSize> input_a, StdBitsetConstRef<BitSize> input_b ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		StdBitset<BitSize> ox( 0 ), auxiliary( input_a );
		for ( size_t i = 0; i < BitSize; i++ ) {
			if ( input_b[i] ) {
				ox = Addition( ox, auxiliary );
			}
			auxiliary <<= 1;
		}
		return ox;
	}

	template<size_t BitSize>  static OptionalStdBitset<BitSize> Division( StdBitsetConstRef<BitSize> input_a, StdBitsetConstRef<BitSize> input_b ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		OptionalStdBitsetPair<BitSize> dwr = DivisionWithRemainder( input_a, input_b );
	
		if ( dwr.has_value( ) ) {		
			return dwr->first;
		}

		return std::nullopt;
	}

	template<size_t BitSize>  static OptionalStdBitset<BitSize> Remainder( StdBitsetConstRef<BitSize> input_a, StdBitsetConstRef<BitSize> input_b ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		OptionalStdBitsetPair<BitSize> dwr = DivisionWithRemainder( input_a, input_b );

		if ( dwr.has_value( ) ) {
			return dwr->second;
		}

		return std::nullopt;
	}

	template<size_t BitSize>  static OptionalStdBitsetPair<BitSize> DivisionWithRemainder( StdBitsetConstRef<BitSize> input_a, StdBitsetConstRef<BitSize> input_b ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );

		// input_b == 0 の時 (input_b においてtrueのビットが1つもない時) は 0除算となり、
		// 計算結果は無いため std::nullopt を返す
		if(input_b.none() ) return std::nullopt;


		CompareResult input_compared = Compare( input_a, input_b );

		if ( input_compared == CompareResult::Equal ) {
			// input_a == input_b の時、商：1、あまり：0
			return std::pair( StdBitset<BitSize>( 1 ), StdBitset<BitSize>( 0 ));
		}

		if ( input_compared == CompareResult::RightGreater ) {
			// input_a < input_b の時、商：0、あまり：input_a
			return std::pair( StdBitset<BitSize>( 0 ), input_a );
		}

		const size_t digit_ia = GetNumberOfDigitsForDisplay( input_a );
		const size_t digit_ib = GetNumberOfDigitsForDisplay( input_b );
		const size_t digit_ox = digit_ia - digit_ib + 1;


		StdBitset<BitSize>  auxiliary( 0 );
		for ( size_t i = 0; i < digit_ib; i++ ) {
			auxiliary[digit_ib - 1 - i] = input_a[digit_ia - 1 - i];
		}

		StdBitset<BitSize>  ox( 0 );

		for ( size_t i = 0; i < digit_ox; i++ ) {

			ox[digit_ox - 1 - i] = (Compare( auxiliary, input_b ) != CompareResult::RightGreater);

			if ( ox[digit_ox - 1 - i] ) {
				auxiliary = Subtraction( auxiliary, input_b );
			}

			if ( ( i + 1 ) < digit_ox ) {
				auxiliary = auxiliary << 1;
				auxiliary[0] = input_a[digit_ia - digit_ib - i - 1];
			}
		}

		// ox：商、auxiliary：余り
		return std::pair( ox, auxiliary );
	}


	template<size_t BitSize>  static size_t GetNumberOfDigitsForDisplay( StdBitsetConstRef<BitSize> input ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		for ( size_t i = BitSize-1; i > 0; i-- ) {
			if ( input[i] ) return i + 1;
		}
		return 1;
	}


	template<size_t BitSize>  static  CompareResult Compare( StdBitsetConstRef<BitSize> input_left, StdBitsetConstRef<BitSize> input_right ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		bool isStillEqual = true;
		bool isLeftAbove = false;
		size_t index;

		for ( size_t i = 0; ( i < BitSize ) && isStillEqual; i++ ) {
			index = BitSize - 1 - i;
			isLeftAbove = input_left[index] && ( !input_right[index] );
			isStillEqual = !( input_left[index] ^ input_right[index] );
			if ( isLeftAbove ) return CompareResult::LeftGreater;
		}

		return  ( isStillEqual ) ? CompareResult::Equal : CompareResult::RightGreater;
	}

	template<size_t BitSize>  static  StdBitset<BitSize> Max( StdBitsetConstRef<BitSize> input_left, StdBitsetConstRef<BitSize> input_right ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		return ( Compare( input_left, input_right ) == CompareResult::LeftGreater ) ? input_left : input_right;
	}

	template<size_t BitSize>  static  StdBitset<BitSize> Min( StdBitsetConstRef<BitSize> input_left, StdBitsetConstRef<BitSize> input_right ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );

		return ( Compare( input_left, input_right ) != CompareResult::LeftGreater ) ? input_left : input_right;
	}

};



template <typename CharT> class CStdBitsetUnsignedStringConversion {
public:
	using String = std::basic_string<CharT>;

	template<size_t BitSize> using StdBitset = CStdBitsetUnsignedOperation::StdBitset<BitSize>;
	template<size_t BitSize> using StdBitsetConstRef = CStdBitsetUnsignedOperation::StdBitsetConstRef<BitSize>;
	template <size_t BitSize> using OptionalStdBitsetPair = CStdBitsetUnsignedOperation::OptionalStdBitsetPair<BitSize>;


	inline static const String DEFAULT_VALID_SEPARATORS = String( { ' ', ',' } );

	template<size_t BitSize>	static String ToBinaryString( StdBitsetConstRef<BitSize> bin ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		size_t msb_pos = CStdBitsetUnsignedOperation::GetNumberOfDigitsForDisplay( bin ) - 1;
		String result_string;
		for ( size_t i = 0; i <= msb_pos; i++ ) {
			result_string.push_back( ( bin[msb_pos - i] ) ? '1' : '0' );
		}
		return result_string;
	}

	template<size_t BitSize>	static StdBitset<BitSize> FromBinaryString( const String& str, const String& valid_separators = DEFAULT_VALID_SEPARATORS ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );

		StdBitset<BitSize> result;
		size_t bit_pos = 0;
		CharT c;

		for ( auto rev_it = str.rbegin( ); ( rev_it != str.rend( ) ) && ( bit_pos < BitSize ); rev_it++ ) {
			c = *rev_it;
			if ( c == '0' ) {
				result[bit_pos] = false;
				bit_pos++;
			} else if ( c == '1' ) {
				result[bit_pos] = true;
				bit_pos++;
			} else  if ( valid_separators.find( c ) !=  String::npos ) {
				continue;
			} else {
				break;
			}
		}

		return result;
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

	template <size_t BitSize> static  StdBitset<BitSize>  FromDecimalString( const String& str , const String& valid_separators = DEFAULT_VALID_SEPARATORS ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		StdBitset<BitSize> result;
		if ( BitSize < 4 ) {
			std::bitset<4> bs = FromDecimalString<4>( str );
			for ( size_t i = 0; i < BitSize; i++ )  result[i] = bs[i];
			return result;
		} 
		const StdBitset<BitSize>  bit_of_ten( 10 );
		StdBitset<BitSize> current_digit_bitset( 0 );
		uint8_t current_digit_value;
		for ( CharT c : str ) {
			if ( c >= '0' && c <= '9' ) {
				current_digit_value = c - '0';
				current_digit_bitset[0] = current_digit_value & 1;
				current_digit_bitset[1] = (current_digit_value >> 1) & 1;
				current_digit_bitset[2] = ( current_digit_value >> 2 ) & 1;
				current_digit_bitset[3] = ( current_digit_value >> 3 ) & 1;
				result = CStdBitsetUnsignedOperation::Multiplication( result, bit_of_ten );
				result = CStdBitsetUnsignedOperation::Addition( result, current_digit_bitset );
			} else if ( valid_separators.find( c ) == String::npos ) {
				break;
			}
		}
		return result;
	}



	template<size_t BitSize>	static String ToHexadecimalString( StdBitsetConstRef<BitSize> bin , bool upper_case = false ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		if ( BitSize < 4 ) {
			StdBitset<4> bs( bin.to_ulong( ) );
			return  ToHexadecimalString( bs );
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

		StdBitset<BitSize> result;
		if ( BitSize < 4 ) {
			std::bitset<4> bs = FromHexadecimalString<4>( str );
			for ( size_t i = 0; i < BitSize; i++ )  result[i] = bs[i];
			return result;
		}
		uint8_t current_digit_value = 0;
		for ( CharT c : str ) {
			if ( c >= '0' && c <= '9' ) {
				current_digit_value = c - '0';
			} else if ( c >= 'A' && c <= 'F' ) {
				current_digit_value = 10 + c - 'A';
			} else if ( c >= 'a' && c <= 'f' ) {
				current_digit_value = 10 + c - 'a';
			} else  if ( valid_separators.find( c ) != String::npos ) {
				continue;
			} else {
				break;
			}

			result <<= 4;
			result[0] = current_digit_value & 1;
			result[1] = ( current_digit_value >> 1 ) & 1;
			result[2] = ( current_digit_value >> 2 ) & 1;
			result[3] = ( current_digit_value >> 3 ) & 1;
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

	static String CreateSeparatedStringWithPadded( const String& str, size_t  size_of_padded_str_min_length , const CharT padding_char , size_t  group_size = 3, const CharT separate_char = ' ' ) {
		return CreateSeparatedString(
			CreatePaddedString( str, size_of_padded_str_min_length  , padding_char),
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


	static String CreatePaddedString( const String& str, size_t  size_of_padded_str_min_length, const CharT padding_char) {
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

template<size_t BitSize , typename DefaultCharType = char> class CUnsignedBitNumber {
public:
	static_assert( BitSize > 0, "BitSizeは無効な値です。" );
	
	using StdBitset = std::bitset<BitSize>;
	using StdBoolOptional = std::optional<bool>;
	using StdSizeTPair = std::pair<size_t, size_t>;

	using SelfOptional = std::optional<CUnsignedBitNumber>;
	using SelfPair = std::pair<CUnsignedBitNumber, CUnsignedBitNumber>;
	using SelfPairOptional = std::optional<SelfPair>;




	// rawは外部から変更されても、本クラスの動作に影響しないため、公開範囲をpublicとする
	StdBitset raw;

	const StdBitset& rawRefConst( void ) const {
		return raw;
	}

	CUnsignedBitNumber( )  : raw(0) {

	}

	CUnsignedBitNumber( uint64_t i64value ) : raw( i64value ) {}
	CUnsignedBitNumber(const StdBitset& value ) : raw( value ) {}

	template <size_t FromSize, typename FromCharType = DefaultCharType> explicit CUnsignedBitNumber( const  CUnsignedBitNumber<FromSize, FromCharType>& from , size_t self_offset_bit_number = 0, size_t from_offset_bit_number = 0 ) {
		this->fromCast( from, self_offset_bit_number, from_offset_bit_number );
	}

	~CUnsignedBitNumber( ) {

	}



	CUnsignedBitNumber& operator+=( const CUnsignedBitNumber& rhs ) {
		this->raw = CStdBitsetUnsignedOperation::Addition( this->raw, rhs.raw );
		return *this;
	}

	CUnsignedBitNumber& operator-=( const CUnsignedBitNumber& rhs ) {
		this->raw = CStdBitsetUnsignedOperation::Subtraction( this->raw, rhs.raw );
		return *this;
	}

	CUnsignedBitNumber& operator*=( const CUnsignedBitNumber& rhs ) {
		this->raw = CStdBitsetUnsignedOperation::Multiplication( this->raw, rhs.raw );
		return *this;
	}

	CUnsignedBitNumber& operator/=( const CUnsignedBitNumber& rhs ) {

		if(rhs.raw.none() ) throw std::domain_error( "CUnsignedBitNumber：0除算が発生しました。" );
		auto calc_result = CStdBitsetUnsignedOperation::Division( this->raw, rhs.raw );

		if ( calc_result.has_value( ) ) {
			this->raw = calc_result.value( );
		} else {
			throw std::domain_error( "CUnsignedBitNumber：除算の算出に失敗しました。" );
		}

		return *this;		
	}

	CUnsignedBitNumber& operator%=( const CUnsignedBitNumber& rhs ) {

		if(rhs.raw.none() ) throw std::domain_error( "CUnsignedBitNumber：0除算が発生しました。" );
		auto calc_result = CStdBitsetUnsignedOperation::Remainder( this->raw, rhs.raw );

		if ( calc_result.has_value( ) ) {
			this->raw = calc_result.value( );
		} else {
			throw std::domain_error( "CUnsignedBitNumber：剰余の算出に失敗しました。" );
		}

		return *this;
	}



	CUnsignedBitNumber operator+( const CUnsignedBitNumber& rhs ) const{
		CUnsignedBitNumber lhs( *this );
		lhs += rhs;
		return lhs;
	}

	CUnsignedBitNumber operator-( const CUnsignedBitNumber& rhs ) const{
		CUnsignedBitNumber lhs( *this );
		lhs -= rhs;
		return lhs;
	}

	CUnsignedBitNumber operator*( const CUnsignedBitNumber& rhs ) const {
		CUnsignedBitNumber lhs( *this );
		lhs *= rhs;
		return lhs;
	}

	CUnsignedBitNumber operator/( const CUnsignedBitNumber& rhs ) const {
		CUnsignedBitNumber lhs( *this );
		lhs /= rhs;
		return lhs;
	}
	CUnsignedBitNumber operator%( const CUnsignedBitNumber& rhs ) const {
		CUnsignedBitNumber lhs( *this );
		lhs %= rhs;
		return lhs;
	}





	CUnsignedBitNumber& operator++( ) {
		this->raw = CStdBitsetUnsignedOperation::Increment( this->raw );
		return *this;
	}

	CUnsignedBitNumber operator++( int ) {
		CUnsignedBitNumber old( *this );
		this->raw = CStdBitsetUnsignedOperation::Increment( this->raw );
		return old;
	}

	CUnsignedBitNumber& operator--( ) {
		this->raw = CStdBitsetUnsignedOperation::Decrement( this->raw );
		return *this;
	}

	CUnsignedBitNumber operator--( int ) {
		CUnsignedBitNumber old( *this );
		this->raw = CStdBitsetUnsignedOperation::Decrement( this->raw );
		return old;
	}


	static bool IsValidIndex( size_t index ) {
		return ( index < BitSize );
	}

	static bool IsValidRangeIndex( size_t index1 , size_t index2 ) {
		return IsValidIndex( index1 ) && IsValidIndex( index2 );
	}

	static size_t ClipIndex( size_t index ) {
		return std::min( index, BitSize - 1 );
	}

	static StdSizeTPair  ClipRangeIndex( size_t index1 , size_t index2 ) {
		size_t n1 = ClipIndex( index1 );
		size_t n2 = ClipIndex( index2 );
		return ( n1 <= n2 ) ? StdSizeTPair( n1, n2 ) : StdSizeTPair( n2, n1 );
	}

	static StdSizeTPair  ClipRangeIndex( const StdSizeTPair& index_pair ) {
		return ClipRangeIndex( index_pair.first, index_pair.second );
	}

	bool clear( size_t start_offset = 0 ) {
		if ( start_offset == 0 ) {
			this->raw.reset( );
			return true;
		}
		if ( start_offset >= BitSize ) return false;
		return this->rangeSetIndex( start_offset, BitSize - 1, false );
	}

	bool fill( size_t start_offset = 0 ) {
		if ( start_offset == 0 ) {
			this->raw.set( );
			return true;
		} 
		if ( start_offset >= BitSize ) return false;
		return this->rangeSetIndex( start_offset, BitSize - 1  , true);
	}


	bool set( size_t index, bool  value = true ) {
		if ( !IsValidIndex(index) ) return false;
		this->raw[index] = value;
		return true;
	}

	bool rangeSet( size_t start_index , size_t size, bool value = true ) {
		if ( size == 0 )return true;
		return rangeSetIndex( start_index, start_index + size - 1, value );
	}

	bool rangeSetIndex( size_t index1, size_t index2, bool value = true ) {
		if ( !IsValidRangeIndex( index1, index2 ) ) return false;
		if ( index1 == index2 ) return this->set( index1, value );

		size_t start_index, end_index;

		if ( index1 < index2 ) {
			start_index = index1;
			end_index = index2;
		} else {
			start_index = index2;
			end_index = index1;
		}

		for ( size_t i = start_index; i <= end_index ; i++ ) {
			this->raw[i] = value;
		}

		return true;
	}

	bool rangeSetIndex(const StdSizeTPair& index_pair, bool value = true ) {
		return rangeSetIndex( index_pair.first, index_pair.second, value );
	}


	bool unset( size_t index ) {
		return this->set( index, false );
	}

	bool rangeUnset( size_t start_index, size_t size ) {
		return rangeUnsetIndex( start_index, start_index + size - 1 );
	}


	bool rangeUnsetIndex( size_t index1, size_t index2 ) {
		return rangeSetIndex( index1, index2, false );
	}

	bool rangeUnsetIndex( const StdSizeTPair& index_pair ) {
		return rangeUnsetIndex( index_pair.first, index_pair.second );
	}


	StdBoolOptional get( size_t index ) const {
		if ( !IsValidIndex( index ) ) return std::nullopt;
		return this->raw[index];
	}


	enum struct ExtractedBitLocation {

		// 最下位ビットから始める
		LeastSignificant = 0,

		// オリジナル位置に配置する
		Original

	};

	CUnsignedBitNumber extract( size_t start_index, size_t size, ExtractedBitLocation extracted_bit_location = ExtractedBitLocation::LeastSignificant ) const{
		CUnsignedBitNumber result;
		if ( size == 0 ) return result;
		if ( !IsValidIndex( start_index ) ) return result;
		return this->extractIndex( start_index, ClipIndex(start_index + std::min(BitSize , size) - 1), extracted_bit_location );
	}




	template <typename T> CUnsignedBitNumber extract( size_t start_index, ExtractedBitLocation extracted_bit_location = ExtractedBitLocation::LeastSignificant ) const{
		return this->extract( start_index, sizeof( T ) * 8, extracted_bit_location );
	}

	CUnsignedBitNumber extractIndex( size_t index1, size_t index2, ExtractedBitLocation extracted_bit_location = ExtractedBitLocation::LeastSignificant ) const {

		if ( !( IsValidIndex( index1 ) || IsValidIndex( index2 ) ) ) {
			// 指定されたindexがどちらも無効な場合、 すなわち、抽出するべき値がない場合、抽出した値は0である。
			return CUnsignedBitNumber( 0 );
		} 
		
		CUnsignedBitNumber result;

		// 以降の処理において、どちらかのindexが無効 (>=BitSize)である場合、
		// インデックスがBitSize以降のビットの値は0(false)を持っているものとして処理を行う。
		// なお、本クラスのインスタンス作成直後の初期値は全ビット0である。
		//  (この位置において、変数resultの値は全ビット0である)
		// そのため、有効範囲内の値のみを抽出することで、当該処理を完了できる。
		// 以上が前提にあるため、以降は、有効範囲内の値を抽出する処理となっている。

				
		// 範囲外アクセスを防止するため、指定されたindex範囲をクリッピングする。
		// また、ClipRangeIndexの処理にて、小さい方がfirstに配置されるので、
		// indexの大小関係もついでに解決できる。
		StdSizeTPair range = ClipRangeIndex( index1, index2 );

		size_t start_index_for_result = ( extracted_bit_location == ExtractedBitLocation::Original ) ? range.first : 0;

		for ( size_t i = range.first; i <= range.second; i++ ) {
			result.raw[i + start_index_for_result - range.first] = this->raw[i];
		}
		return result;

	}

protected:


	template<typename UIntTypeName> UIntTypeName toUIntType( size_t offset_bit_number = 0 ) const{

		if ( !IsValidIndex( offset_bit_number ) ) {
			return 0;
		}

		CUnsignedBitNumber extracted_value = extract<UIntTypeName>( offset_bit_number, ExtractedBitLocation::LeastSignificant );

		UIntTypeName result = 0;

		size_t result_bit_size = std::min(BitSize , sizeof( UIntTypeName ) * 8);

		for ( size_t i = 0; i < result_bit_size; i++ ) {
			if ( extracted_value.raw[i] ) {
				result |= static_cast<UIntTypeName>(1) << i;
			}
		}

		return result;
	}

	template<typename UIntTypeName> void fromUIntType( UIntTypeName value , size_t self_offset_bit_number = 0 )  {

		size_t value_bit_size = std::min( BitSize, sizeof( UIntTypeName ) * 8 );
		
		clear( );

		if ( IsValidIndex( self_offset_bit_number ) ) {

			size_t size = std::min( BitSize - self_offset_bit_number, value_bit_size );

			for ( size_t i = 0; i < size; i++ ) {
				if ( value & ( static_cast<UIntTypeName>( 1 ) << i ) ) {
					this->raw[i + self_offset_bit_number] = true;
				}
			}
		}

	}


public:

	template <size_t NewSize , typename CharType = DefaultCharType>  CUnsignedBitNumber<NewSize, CharType> toCast( size_t self_offset_bit_number = 0 , size_t to_offset_bit_number = 0 )const {
		return 	CUnsignedBitNumber<NewSize, CharType>( this->extract(self_offset_bit_number , NewSize ), to_offset_bit_number );
	}

	uint8_t toUInt8( size_t offset_bit_number = 0 ) const{
		return toUIntType<uint8_t>( offset_bit_number );
	}

	uint16_t toUInt16( size_t offset_bit_number = 0 ) const{
		return toUIntType<uint16_t>( offset_bit_number );
	}
	
	uint32_t toUInt32( size_t offset_bit_number = 0 ) const{
		return toUIntType<uint32_t>( offset_bit_number );
	}

	uint64_t toUInt64( size_t offset_bit_number = 0 ) const{
		return toUIntType<uint64_t>( offset_bit_number );
	}


	template <size_t FromSize, typename FromCharType = DefaultCharType> void fromCast(const  CUnsignedBitNumber<FromSize, FromCharType>& from, size_t self_offset_bit_number = 0 , size_t from_offset_bit_number = 0 ) {
		this->raw = CStdBitsetUnsignedOperation::CastSize<BitSize>( from.extract(from_offset_bit_number , BitSize).raw );
		if ( self_offset_bit_number > 0 ) this->raw <<= self_offset_bit_number;
	}


	void fromUInt8( uint8_t value, size_t self_offset_bit_number = 0 ) {
		fromUIntType( value, self_offset_bit_number );
	}

	void fromUInt16( uint16_t value, size_t self_offset_bit_number = 0 ) {
		fromUIntType( value, self_offset_bit_number );
	}

	void fromUInt32( uint32_t value, size_t self_offset_bit_number = 0 ) {
		fromUIntType( value, self_offset_bit_number );
	}

	void fromUInt64( uint64_t value, size_t self_offset_bit_number = 0 ) {
		fromUIntType( value, self_offset_bit_number );
	}




	CUnsignedBitNumber additionWithCarry( const CUnsignedBitNumber& value, const bool input_carry = false, bool* const pLastCarry = nullptr ) const{
		auto pre_result = CStdBitsetUnsignedOperation::Addition<BitSize>( this->raw, value.raw, input_carry, pLastCarry );
		return CUnsignedBitNumber( pre_result );
	}


	CUnsignedBitNumber selfUpdateAdditionWithCarry( const CUnsignedBitNumber& value, const bool input_carry = false, bool* const pLastCarry = nullptr ) {
		auto pre_result = additionWithCarry( value, input_carry, pLastCarry );
		this->raw = pre_result.raw;
		return pre_result;
	}


	CUnsignedBitNumber addition( const CUnsignedBitNumber& value) const {
		return additionWithCarry( value, false, nullptr );
	}


	CUnsignedBitNumber selfUpdateAddition( const CUnsignedBitNumber& value ) {
		auto pre_result = addition( value );
		this->raw = pre_result.raw;
		return pre_result;
	}



	CUnsignedBitNumber subtraction( const CUnsignedBitNumber& value )const {
		CUnsignedBitNumber result( *this );
		result -= value;
		return result;
	}

	CUnsignedBitNumber selfUpdateSubtraction( const CUnsignedBitNumber& value ) {
		CUnsignedBitNumber result = subtraction(value);
		this->raw = result.raw;
		return result;
	}

	CUnsignedBitNumber multiplication( const CUnsignedBitNumber& value ) const {
		CUnsignedBitNumber result( *this );
		result *= value;
		return result;
	}

	CUnsignedBitNumber selfUpdateMultiplication( const CUnsignedBitNumber& value )  {
		CUnsignedBitNumber result = multiplication( value );
		this->raw = result.raw;
		return result;
	}


	SelfOptional  division( const CUnsignedBitNumber& value) const{
		auto pre_result = this->divisionWithRemainder( value );
		if ( pre_result.has_value( ) ) {
			CUnsignedBitNumber result( pre_result->first );
			return result;
		}
		return std::nullopt;
	}

	SelfOptional  selfUpdateDivision( const CUnsignedBitNumber& value )  {
		auto result = this->division( value );
		if ( result.has_value( ) ) {
			this->raw = result.value().raw;
		}
		return result;
	}

	SelfOptional remainder( const CUnsignedBitNumber& value ) const{
		auto pre_result = this->divisionWithRemainder( value );
		if ( pre_result.has_value( ) ) {
			CUnsignedBitNumber result( pre_result->second);
			return result;
		}
		return std::nullopt;
	}

	SelfOptional  selfUpdateRemainder( const CUnsignedBitNumber& value ) {
		auto result = this->remainder( value );
		if ( result.has_value( ) ) {
			this->raw = result.value( ).raw;
		}
		return result;
	}

	SelfPairOptional  divisionWithRemainder( const CUnsignedBitNumber& value) const{
		auto pre_result = CStdBitsetUnsignedOperation::DivisionWithRemainder<BitSize>( this->raw, value.raw );
		if ( pre_result.has_value( ) ) {
			return SelfPair( CUnsignedBitNumber( pre_result->first ), CUnsignedBitNumber( pre_result->second ) );
		}
		return std::nullopt;
	}

	size_t getNumberOfBinaryDigitsForDisplay( void )const {
		return CStdBitsetUnsignedOperation::GetNumberOfDigitsForDisplay( this->raw );
	}


	enum struct CompareResult {
		SelfGreater = 0,
		Equal,
		TargetGreater
	};


	CompareResult compare( const CUnsignedBitNumber& target ) const{

		CStdBitsetUnsignedOperation::CompareResult  cr = CStdBitsetUnsignedOperation::Compare( this->raw, target.raw );

		switch ( cr ) {
			case CStdBitsetUnsignedOperation::CompareResult::LeftGreater:
				return CompareResult::SelfGreater;
			case CStdBitsetUnsignedOperation::CompareResult::Equal:
				return CompareResult::Equal;
			case CStdBitsetUnsignedOperation::CompareResult::RightGreater:
				return CompareResult::TargetGreater;
		}

		// ここには到達しないが、Visual Studio (Visual C++)における、 warning C4715 対策と、
		// 予期しないバグに対応するため、例外を発行しておく。
		throw std::domain_error( "CUnsignedBitNumber::compare：予定外の例外が発生しました。" );
	}


	static  CUnsignedBitNumber  Max( const CUnsignedBitNumber& number1, const CUnsignedBitNumber& number2 ) {
		auto value = CStdBitsetUnsignedOperation::Max( number1.raw, number2.raw );
		return  CUnsignedBitNumber( value );
	}

	static  CUnsignedBitNumber  Min( const CUnsignedBitNumber& number1, const CUnsignedBitNumber& number2 ) {
		auto value = CStdBitsetUnsignedOperation::Min( number1.raw, number2.raw );
		return  CUnsignedBitNumber( value );
	}

	template<typename CharT = DefaultCharType> std::basic_string<CharT> toJsonLikedString( )const {
		std::basic_string<CharT> s;

		//open block
		s.push_back( '{' );
		s.push_back( ' ' );


		// CStdBitsetUnsignedStringConversionのフルクラス名が少々長いため別名としてConvを定義
		using Conv = CStdBitsetUnsignedStringConversion<CharT>;
		
		//bin
		s.append( { '\"' ,  'b' , 'i' , 'n' , '\"' ,  ':' , '\"' } );

		s.append( Conv::CreateSeparatedStringWithZeroPadded( this->toBinaryString<CharT>( ), BitSize, 4 ) );

		s.push_back( '\"' );


		//dec
		s.append( { ' ', ',', ' ' } );
		s.append( { '\"' ,  'd' , 'e' , 'c' , '\"' ,  ':' , '\"' } );

		s.append( this->toDecimalString<CharT>( ));

		s.push_back( '\"' );


		//hex
		s.append( { ' ', ',', ' ' } );
		s.append( { '\"' ,  'h' , 'e' , 'x' , '\"' ,  ':' , '\"' } );

		s.append( this->toHexadecimalString<CharT>( true)  );

		s.push_back( '\"' );


		//close block
		s.push_back( ' ' );
		s.push_back( '}' );

		return s;
	}

	template<typename CharT = DefaultCharType> std::basic_string<CharT> toBinaryString( )const {
		return CStdBitsetUnsignedStringConversion<CharT>::ToBinaryString( this->raw );
	}

	template<typename CharT = DefaultCharType> std::basic_string<CharT> toDecimalString( )const {
		return CStdBitsetUnsignedStringConversion<CharT>::ToDecimalString( this->raw );
	}

	template<typename CharT = DefaultCharType> std::basic_string<CharT> toHexadecimalString(  bool upper_case = false ) const{
		return CStdBitsetUnsignedStringConversion<CharT>::ToHexadecimalString( this->raw  , upper_case);
	}

	template<typename CharT = DefaultCharType> void  fromBinaryString( const std::basic_string<CharT>& str, const std::basic_string<CharT>& valid_separators = CStdBitsetUnsignedStringConversion<CharT>::DEFAULT_VALID_SEPARATORS ) {
		this->raw = CStdBitsetUnsignedStringConversion<CharT>::FromBinaryString< BitSize>( str, valid_separators );
	}

	template<typename CharT = DefaultCharType> void  fromDecimalString( const std::basic_string<CharT>& str, const std::basic_string<CharT>& valid_separators = CStdBitsetUnsignedStringConversion<CharT>::DEFAULT_VALID_SEPARATORS ) {
		this->raw = CStdBitsetUnsignedStringConversion<CharT>::FromDecimalString< BitSize>( str, valid_separators );
	}

	template<typename CharT = DefaultCharType> void  fromHexadecimalString( const std::basic_string<CharT>& str, const std::basic_string<CharT>& valid_separators = CStdBitsetUnsignedStringConversion<CharT>::DEFAULT_VALID_SEPARATORS ) {
		this->raw = CStdBitsetUnsignedStringConversion<CharT>::FromHexadecimalString< BitSize>( str, valid_separators );
	}


	template<typename CharT = DefaultCharType> class FromHelpers {
	public:
		static CUnsignedBitNumber  FromBinaryString( const std::basic_string<CharT>& str, const std::basic_string<CharT>& valid_separators = CStdBitsetUnsignedStringConversion<CharT>::DEFAULT_VALID_SEPARATORS ) {
			CUnsignedBitNumber ubn;
			ubn.fromBinaryString<CharT>( str, valid_separators );
			return ubn;
		}

		static CUnsignedBitNumber  FromDecimalString( const std::basic_string<CharT>& str, const std::basic_string<CharT>& valid_separators = CStdBitsetUnsignedStringConversion<CharT>::DEFAULT_VALID_SEPARATORS ) {
			CUnsignedBitNumber ubn;
			ubn.fromDecimalString<CharT>( str, valid_separators );
			return ubn;
		}

		static CUnsignedBitNumber  FromHexadecimalString( const std::basic_string<CharT>& str, const std::basic_string<CharT>& valid_separators = CStdBitsetUnsignedStringConversion<CharT>::DEFAULT_VALID_SEPARATORS ) {
			CUnsignedBitNumber ubn;
			ubn.fromHexadecimalString<CharT>( str, valid_separators );
			return ubn;
		}

	};

};

template <size_t BitSize> using CUnsignedBitNumberA = CUnsignedBitNumber<BitSize, char>;
using CUnsignedBitNumber8 = CUnsignedBitNumberA<8>;
using CUnsignedBitNumber16 = CUnsignedBitNumberA<16>;
using CUnsignedBitNumber24 = CUnsignedBitNumberA<24>;
using CUnsignedBitNumber32 = CUnsignedBitNumberA<32>;
using CUnsignedBitNumber64 = CUnsignedBitNumberA<64>;
using CUnsignedBitNumber128 = CUnsignedBitNumberA<128>;

template <size_t BitSize> using CUnsignedBitNumberW = CUnsignedBitNumber<BitSize, wchar_t>;
using CUnsignedBitNumber8W = CUnsignedBitNumberW<8>;
using CUnsignedBitNumber16W = CUnsignedBitNumberW<16>;
using CUnsignedBitNumber24W = CUnsignedBitNumberW<24>;
using CUnsignedBitNumber32W = CUnsignedBitNumberW<32>;
using CUnsignedBitNumber64W = CUnsignedBitNumberW<64>;
using CUnsignedBitNumber128W = CUnsignedBitNumberW<128>;


