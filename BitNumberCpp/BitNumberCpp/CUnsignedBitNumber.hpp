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

	template<size_t BitSize> using StdBitset = std::bitset<BitSize>;
	template<size_t BitSize> using StdBitsetConstRef = const StdBitset<BitSize>&;

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
	static_assert( BitSize > 0, "BitSizeは無効な値です。" );

public:

	using StdBitset = std::bitset<BitSize>;

	using ReferenceTypeWithInStdBitset = typename StdBitset::reference;
	using StdBoolOptional = std::optional<bool>;



private:
	StdBitset m_bitData;
	template<size_t , typename> friend class CUnsignedBitNumber;

public:

	CUnsignedBitNumber( )  : m_bitData(0) {

	}

	CUnsignedBitNumber(uint64_t i64value ) : m_bitData( i64value){
	}

	~CUnsignedBitNumber( ) {
	}


	StdBitset* getStdBitsetPointer( void ) {
		return &this->m_bitData;
	}

	StdBitset* operator -> ( void ) {
		return this->getStdBitsetPointer( );
	}

	void fromStdBitset( const StdBitset& ref_sb ){
		this->m_bitData = ref_sb;
	}

	StdBitset toStdBitset( void )const {
		return this->m_bitData;
	}

	StdBitset& toStdBitsetRef( void ) {
		return this->m_bitData;
	}

	template <size_t NewSize>  CUnsignedBitNumber<NewSize , DefaultCharType> castSize( )const {
		CUnsignedBitNumber<NewSize, DefaultCharType> target;
		target.m_bitData = CStdBitsetUnsignedOperation::CastSize<NewSize>( this->m_bitData );
		return target;
	}

	bool operator[]( size_t index ) const{
		return this->m_bitData[index];
	}

	ReferenceTypeWithInStdBitset operator[]( size_t index ) {
		return this->m_bitData[index];
	}


	CUnsignedBitNumber& operator+=( const CUnsignedBitNumber& rhs ) {
		this->m_bitData = CStdBitsetUnsignedOperation::Addition( this->m_bitData, rhs.m_bitData );
		return *this;
	}

	CUnsignedBitNumber& operator-=( const CUnsignedBitNumber& rhs ) {
		this->m_bitData = CStdBitsetUnsignedOperation::Subtraction( this->m_bitData, rhs.m_bitData );
		return *this;
	}

	CUnsignedBitNumber& operator*=( const CUnsignedBitNumber& rhs ) {
		this->m_bitData = CStdBitsetUnsignedOperation::Multiplication( this->m_bitData, rhs.m_bitData );
		return *this;
	}

	CUnsignedBitNumber& operator/=( const CUnsignedBitNumber& rhs ) {

		if(rhs.m_bitData.none() ) throw std::domain_error( "CUnsignedBitNumber：0除算が発生しました。" );
		auto calc_result = CStdBitsetUnsignedOperation::Division( this->m_bitData, rhs.m_bitData );

		if ( calc_result.has_value( ) ) {
			this->m_bitData = calc_result.value( );
		} else {
			throw std::domain_error( "CUnsignedBitNumber：除算の算出に失敗しました。" );
		}

		return *this;		
	}

	CUnsignedBitNumber& operator%=( const CUnsignedBitNumber& rhs ) {

		if(rhs.m_bitData.none() ) throw std::domain_error( "CUnsignedBitNumber：0除算が発生しました。" );
		auto calc_result = CStdBitsetUnsignedOperation::Remainder( this->m_bitData, rhs.m_bitData );

		if ( calc_result.has_value( ) ) {
			this->m_bitData = calc_result.value( );
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
		this->m_bitData = CStdBitsetUnsignedOperation::Increment( this->m_bitData );
		return *this;
	}

	CUnsignedBitNumber operator++( int ) {
		CUnsignedBitNumber old( *this );
		this->m_bitData = CStdBitsetUnsignedOperation::Increment( this->m_bitData );
		return old;
	}

	CUnsignedBitNumber& operator--( ) {
		this->m_bitData = CStdBitsetUnsignedOperation::Decrement( this->m_bitData );
		return *this;
	}

	CUnsignedBitNumber operator--( int ) {
		CUnsignedBitNumber old( *this );
		this->m_bitData = CStdBitsetUnsignedOperation::Decrement( this->m_bitData );
		return old;
	}

	template<typename CharT = DefaultCharType> std::basic_string<CharT> toBinaryString( )const {
		return CStdBitsetUnsignedStringConversion<CharT>::ToBinaryString( this->m_bitData );
	}

	template<typename CharT = DefaultCharType> std::basic_string<CharT> toDecimalString( )const {
		return CStdBitsetUnsignedStringConversion<CharT>::ToDecimalString( this->m_bitData );
	}

	template<typename CharT = DefaultCharType> std::basic_string<CharT> toHexadecimalString(  bool upper_case = false ) const{
		return CStdBitsetUnsignedStringConversion<CharT>::ToHexadecimalString( this->m_bitData  , upper_case);
	}

	template<typename CharT = DefaultCharType> void  fromBinaryString( const std::basic_string<CharT>& str, const std::basic_string<CharT>& valid_separators = CStdBitsetUnsignedStringConversion<CharT>::DEFAULT_VALID_SEPARATORS ) {
		this->m_bitData = CStdBitsetUnsignedStringConversion<CharT>::FromBinaryString< BitSize>( str, valid_separators );
	}

	template<typename CharT = DefaultCharType> void  fromDecimalString( const std::basic_string<CharT>& str, const std::basic_string<CharT>& valid_separators = CStdBitsetUnsignedStringConversion<CharT>::DEFAULT_VALID_SEPARATORS ) {
		this->m_bitData = CStdBitsetUnsignedStringConversion<CharT>::FromDecimalString< BitSize>( str, valid_separators );
	}

	template<typename CharT = DefaultCharType> void  fromHexadecimalString( const std::basic_string<CharT>& str, const std::basic_string<CharT>& valid_separators = CStdBitsetUnsignedStringConversion<CharT>::DEFAULT_VALID_SEPARATORS ) {
		this->m_bitData = CStdBitsetUnsignedStringConversion<CharT>::FromHexadecimalString< BitSize>( str, valid_separators );
	}


	template<typename CharT = DefaultCharType> class StringHelpers {
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

using CUnsignedBitNumber8 = CUnsignedBitNumber<8, char>;
using CUnsignedBitNumber16 = CUnsignedBitNumber<16, char>;
using CUnsignedBitNumber24 = CUnsignedBitNumber<24, char>;
using CUnsignedBitNumber32 = CUnsignedBitNumber<32, char>;
using CUnsignedBitNumber64 = CUnsignedBitNumber<64, char>;


using CUnsignedBitNumber8W = CUnsignedBitNumber<8, wchar_t>;
using CUnsignedBitNumber16W = CUnsignedBitNumber<16, wchar_t>;
using CUnsignedBitNumber24W = CUnsignedBitNumber<24, wchar_t>;
using CUnsignedBitNumber32W = CUnsignedBitNumber<32, wchar_t>;
using CUnsignedBitNumber64W = CUnsignedBitNumber<64, wchar_t>;

