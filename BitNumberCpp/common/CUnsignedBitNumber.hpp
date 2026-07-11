/*

以下を利用した符号なし整数型の定義
	
	CStdBitsetUnsignedOperation
	CStdBitsetUnsignedStringConversion

開発環境：Visual Studio 2026

*/

#pragma once

#include <stdexcept>
#include <compare>

#include "CStdBitsetUnsignedOperation.hpp"
#include "CStdBitsetUnsignedStringConversion.hpp"

template<size_t BitSize , typename DefaultCharType = char> class CUnsignedBitNumber {
public:
	static_assert( BitSize > 0, "BitSizeは無効な値です。" );
	
	using StdBitset = CStdBitsetUnsignedOperation::StdBitset<BitSize>;
	using StdBoolOptional = std::optional<bool>;
	using StdSizeTPair = std::pair<size_t, size_t>;

	using SelfOptional = std::optional<CUnsignedBitNumber>;
	using SelfPair = std::pair<CUnsignedBitNumber, CUnsignedBitNumber>;
	using SelfPairOptional = std::optional<SelfPair>;

	enum struct OffsetBasis {
		Least = 0,		// 最下位ビット基準
		Most,				// 最上位ビット基準
		LSB = Least,	// 最下位ビット基準 (Leastの別名)
		MSB = Most		// 最上位ビット基準 (Mostの別名)
	};


	// rawは全ビットが符号ビット等の状態を示すビットがない純粋な数値データであり、
	// 外部から変更されても、本クラスの動作に影響しないため、公開範囲はpublicで問題ない
	StdBitset raw;


	const StdBitset& rawRefConst( void ) const {
		return raw;
	}

	CUnsignedBitNumber( )  : raw(0) {

	}

	CUnsignedBitNumber( uint64_t i64value ) : raw( i64value ) {
	
	}
	
	CUnsignedBitNumber(const StdBitset& value ) : raw( value ) {
	
	}

	template <size_t FromSize, typename FromCharType = DefaultCharType> explicit CUnsignedBitNumber( const  CUnsignedBitNumber<FromSize, FromCharType>& from , size_t self_offset_bit_number = 0, size_t from_offset_bit_number = 0 ) {
		this->fromCast( from, self_offset_bit_number, from_offset_bit_number );
	}

	~CUnsignedBitNumber( ) = default;


	/*
		インデックス検証・クリップ関数
	*/

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


	/*
		ビット値のセット・アンセット系
	*/

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
		if ( size == 0 )return true;
		return rangeUnsetIndex( start_index, start_index + size - 1 );
	}


	bool rangeUnsetIndex( size_t index1, size_t index2 ) {
		return rangeSetIndex( index1, index2, false );
	}

	bool rangeUnsetIndex( const StdSizeTPair& index_pair ) {
		return rangeUnsetIndex( index_pair.first, index_pair.second );
	}


	// ビット値取得
	StdBoolOptional get( size_t index ) const {
		if ( !IsValidIndex( index ) ) return std::nullopt;
		return this->raw[index];
	}


	/*
		値の抽出系
	*/

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

	/*
		unsigned int系への変換テンプレート関数
	*/
	
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

	/*
		unsigned int系からの変換テンプレート関数
	*/
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


	/*
		他へのキャスト系関数
	*/
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


	/*
		他からのキャスト系関数
	*/
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


	/*
		加算系の実装
	*/
	CUnsignedBitNumber additionWithCarryParam( const CUnsignedBitNumber& value, const bool input_carry = false, bool* const pLastCarry = nullptr ) const{
		auto pre_result = CStdBitsetUnsignedOperation::Addition<BitSize>( this->raw, value.raw, input_carry, pLastCarry );
		return CUnsignedBitNumber( pre_result );
	}


	CUnsignedBitNumber selfUpdateAdditionWithCarryParam( const CUnsignedBitNumber& value, const bool input_carry = false, bool* const pLastCarry = nullptr ) {
		auto pre_result = additionWithCarryParam( value, input_carry, pLastCarry );
		this->raw = pre_result.raw;
		return pre_result;
	}


	CUnsignedBitNumber addition( const CUnsignedBitNumber& value) const {
		return additionWithCarryParam( value, false, nullptr );
	}


	CUnsignedBitNumber selfUpdateAddition( const CUnsignedBitNumber& value ) {
		auto pre_result = addition( value );
		this->raw = pre_result.raw;
		return pre_result;
	}


	CUnsignedBitNumber& operator+=( const CUnsignedBitNumber& rhs ) {
		this->selfUpdateAddition( rhs );
		return *this;
	}


	CUnsignedBitNumber operator+( const CUnsignedBitNumber& rhs ) const {
		CUnsignedBitNumber lhs( *this );
		lhs += rhs;
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


	/*
	
		減算系の実装
	*/

	CUnsignedBitNumber subtraction( const CUnsignedBitNumber& value )const {
		CUnsignedBitNumber result;
		result.raw = CStdBitsetUnsignedOperation::Subtraction( this->raw, value.raw );
		return result;
	}

	CUnsignedBitNumber selfUpdateSubtraction( const CUnsignedBitNumber& value ) {
		CUnsignedBitNumber result = subtraction(value);
		this->raw = result.raw;
		return result;
	}

	CUnsignedBitNumber& operator-=( const CUnsignedBitNumber& rhs ) {
		this->selfUpdateSubtraction( rhs );
		return *this;
	}



	CUnsignedBitNumber operator-( const CUnsignedBitNumber& rhs ) const {
		CUnsignedBitNumber lhs( *this );
		lhs -= rhs;
		return lhs;
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


	/*
		掛け算
	*/

	CUnsignedBitNumber multiplication( const CUnsignedBitNumber& value ) const {
		CUnsignedBitNumber result;
		result.raw = CStdBitsetUnsignedOperation::Multiplication( this->raw, value.raw );
		return result;
	}

	CUnsignedBitNumber selfUpdateMultiplication( const CUnsignedBitNumber& value )  {
		CUnsignedBitNumber result = multiplication( value );
		this->raw = result.raw;
		return result;
	}

	CUnsignedBitNumber& operator*=( const CUnsignedBitNumber& rhs ) {
		this->selfUpdateMultiplication( rhs );
		return *this;
	}

	CUnsignedBitNumber operator*( const CUnsignedBitNumber& rhs ) const {
		CUnsignedBitNumber lhs( *this );
		lhs *= rhs;
		return lhs;
	}

	/*
		除算
	*/
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


	CUnsignedBitNumber& operator/=( const CUnsignedBitNumber& rhs ) {

		if ( rhs.raw.none( ) ) throw std::domain_error( "CUnsignedBitNumber：0除算が発生しました。" );

		SelfOptional result = this->selfUpdateDivision( rhs );

		if ( !result.has_value( ) ) {
			// 本オペレータの初回実装時において、ここのブロックは(先に0除算チェックをしている影響で)
			// 実行されないが念のため、例外を発行しておく
			throw std::domain_error( "CUnsignedBitNumber：除算の算出に失敗しました。" );
		}

		return *this;
	}

	CUnsignedBitNumber operator/( const CUnsignedBitNumber& rhs ) const {
		CUnsignedBitNumber lhs( *this );
		lhs /= rhs;
		return lhs;
	}




	/*
		剰余
	*/
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

	CUnsignedBitNumber& operator%=( const CUnsignedBitNumber& rhs ) {

		if ( rhs.raw.none( ) ) throw std::domain_error( "CUnsignedBitNumber：0除算が発生しました。" );

		SelfOptional result = this->selfUpdateRemainder( rhs );

		if ( !result.has_value( ) ) {
			// 本オペレータの初回実装時において、ここのブロックは(先に0除算チェックをしている影響で)
			// 実行されないが念のため、例外を発行しておく
			throw std::domain_error( "CUnsignedBitNumber：剰余の算出に失敗しました。" );
		}

		return *this;
	}

	CUnsignedBitNumber operator%( const CUnsignedBitNumber& rhs ) const {
		CUnsignedBitNumber lhs( *this );
		lhs %= rhs;
		return lhs;
	}

	/*
		除算・剰余
	*/


	SelfPairOptional  divisionWithRemainder( const CUnsignedBitNumber& value) const{
		auto pre_result = CStdBitsetUnsignedOperation::DivisionWithRemainder<BitSize>( this->raw, value.raw );
		if ( pre_result.has_value( ) ) {
			return SelfPair( CUnsignedBitNumber( pre_result->first ), CUnsignedBitNumber( pre_result->second ) );
		}
		return std::nullopt;
	}

	/*
		2進数として文字列化した際の表示文字列の長さを取得
	*/
	size_t getNumberOfBinaryDigitsForDisplay( void )const {
		return CStdBitsetUnsignedOperation::GetNumberOfDigitsForDisplay( this->raw );
	}



	/*
		比較系
	*/

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
			case CStdBitsetUnsignedOperation::CompareResult::RightGreater:
				return CompareResult::TargetGreater;
		}

		return CompareResult::Equal;
	}


	template <size_t NewSize, typename CharType = DefaultCharType> CompareResult compareExtend( const  CUnsignedBitNumber<NewSize, CharType>& target ) const {

		CStdBitsetUnsignedOperation::CompareResult  cr = CStdBitsetUnsignedOperation::CompareExtend( this->raw, target.raw );

		switch ( cr ) {
			case CStdBitsetUnsignedOperation::CompareResult::LeftGreater:
				return CompareResult::SelfGreater;
			case CStdBitsetUnsignedOperation::CompareResult::RightGreater:
				return CompareResult::TargetGreater;
		}

		return CompareResult::Equal;
	}




	bool  equal( const  CUnsignedBitNumber& target ) const {
		return compare( target ) == CompareResult::Equal;
	}

	template <size_t NewSize, typename CharType = DefaultCharType> bool equalExtend( const  CUnsignedBitNumber<NewSize, CharType>& target ) const {
		return compareExtend( target ) == CompareResult::Equal;
	}

	std::strong_ordering operator <=> (const  CUnsignedBitNumber& rhs ) const{
	
		CompareResult cr = this->compare( rhs );

		switch ( cr ) {
			case CompareResult::SelfGreater:
				return std::strong_ordering::greater;
			case CompareResult::TargetGreater:
				return std::strong_ordering::less;		
		}

		return std::strong_ordering::equal;
	}

	bool  operator==( const  CUnsignedBitNumber& rhs ) const {
		return this->equal( rhs );
	}


	/*
		大きい方のCUnsignedBitNumberを返すスタティック関数
	*/
	static  CUnsignedBitNumber  Max( const CUnsignedBitNumber& number1, const CUnsignedBitNumber& number2 ) {
		auto value = CStdBitsetUnsignedOperation::Max( number1.raw, number2.raw );
		return  CUnsignedBitNumber( value );
	}

	/*
		小さい方のCUnsignedBitNumberを返すスタティック関数
	*/
	static  CUnsignedBitNumber  Min( const CUnsignedBitNumber& number1, const CUnsignedBitNumber& number2 ) {
		auto value = CStdBitsetUnsignedOperation::Min( number1.raw, number2.raw );
		return  CUnsignedBitNumber( value );
	}


	/*
		文字列への変換系
	*/

	template<typename CharT = DefaultCharType> std::basic_string<CharT> toJsonLikedString( bool enableSeparate = false )const {
		std::basic_string<CharT> s , trans;

		//open block
		s.push_back( '{' );
		s.push_back( ' ' );


		// CStdBitsetUnsignedStringConversionのフルクラス名が少々長いため別名としてConvを定義
		using Conv = CStdBitsetUnsignedStringConversion<CharT>;
		
		//bin
		s.append( { '\"' ,  'b' , 'i' , 'n' , '\"' ,  ':' , '\"' } );

		trans = this->toBinaryString<CharT>( );
		if ( enableSeparate ) {
			s.append( Conv::CreateSpaceSeparatedString ( trans, 4 ) );
		} else {
			s.append( trans );
		}

		s.push_back( '\"' );


		//dec
		s.append( { ' ', ',', ' ' } );
		s.append( { '\"' ,  'd' , 'e' , 'c' , '\"' ,  ':' , '\"' } );

		trans = this->toDecimalString<CharT>( );
		if ( enableSeparate ) {
			s.append( Conv::CreateCommaSeparatedString( trans, 3 ) );
		} else {
			s.append( trans );
		}


		s.push_back( '\"' );


		//hex
		s.append( { ' ', ',', ' ' } );
		s.append( { '\"' ,  'h' , 'e' , 'x' , '\"' ,  ':' , '\"' } );

		trans = this->toHexadecimalString<CharT>( true );
		if ( enableSeparate ) {
			s.append( Conv::CreateSpaceSeparatedString( trans, 4 ) );
		} else {
			s.append( trans );
		}

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


	/*
		文字列からの変換系
	*/

	template<typename CharT = DefaultCharType> void  fromBinaryString( const std::basic_string<CharT>& str, const std::basic_string<CharT>& valid_separators = CStdBitsetUnsignedStringConversion<CharT>::DEFAULT_VALID_SEPARATORS ) {
		this->raw = CStdBitsetUnsignedStringConversion<CharT>::FromBinaryString< BitSize>( str, valid_separators );
	}

	template<typename CharT = DefaultCharType> void  fromDecimalString( const std::basic_string<CharT>& str, const std::basic_string<CharT>& valid_separators = CStdBitsetUnsignedStringConversion<CharT>::DEFAULT_VALID_SEPARATORS ) {
		this->raw = CStdBitsetUnsignedStringConversion<CharT>::FromDecimalString< BitSize>( str, valid_separators );
	}

	template<typename CharT = DefaultCharType> void  fromHexadecimalString( const std::basic_string<CharT>& str, const std::basic_string<CharT>& valid_separators = CStdBitsetUnsignedStringConversion<CharT>::DEFAULT_VALID_SEPARATORS ) {
		this->raw = CStdBitsetUnsignedStringConversion<CharT>::FromHexadecimalString< BitSize>( str, valid_separators );
	}

	template<typename CharT = DefaultCharType> static  CUnsignedBitNumber  CreateFromBinaryString( const std::basic_string<CharT>& str, const std::basic_string<CharT>& valid_separators = CStdBitsetUnsignedStringConversion<CharT>::DEFAULT_VALID_SEPARATORS ) {
		CUnsignedBitNumber ubn;
		ubn.fromBinaryString<CharT>( str, valid_separators );
		return ubn;
	}

	template<typename CharT = DefaultCharType> static CUnsignedBitNumber  CreateFromDecimalString( const std::basic_string<CharT>& str, const std::basic_string<CharT>& valid_separators = CStdBitsetUnsignedStringConversion<CharT>::DEFAULT_VALID_SEPARATORS ) {
		CUnsignedBitNumber ubn;
		ubn.fromDecimalString<CharT>( str, valid_separators );
		return ubn;
	}

	template<typename CharT = DefaultCharType> static CUnsignedBitNumber  CreateFromHexadecimalString( const std::basic_string<CharT>& str, const std::basic_string<CharT>& valid_separators = CStdBitsetUnsignedStringConversion<CharT>::DEFAULT_VALID_SEPARATORS ) {
		CUnsignedBitNumber ubn;
		ubn.fromHexadecimalString<CharT>( str, valid_separators );
		return ubn;
	}

	/*
		ランダム生成
	*/

	void selfUpdateRandom( size_t offset = 0 , size_t fill_bit_size = BitSize , bool partial = false ) {

		if ( fill_bit_size == 0 )return;

		if ( offset >= BitSize ) {
			if ( !partial ) {
				clear( );
			}
			return;
		}

		StdBitset  sb = CStdBitsetUnsignedOperation::Random<BitSize>( fill_bit_size );

		sb <<= offset;

		if ( !partial ) {
			this->raw = sb;	
		} else {
			auto setting_range = ClipRangeIndex( offset, offset + fill_bit_size - 1 );
			for ( size_t i = setting_range.first; i <= setting_range.second ; i++ ) {
				this->raw[i] = sb[i];
			}
		}

	}


	void selfUpdateRandomExtend( size_t offset = 0, size_t fill_bit_size = BitSize, OffsetBasis offset_basis = OffsetBasis::Least,  bool partial = false ) {

		if ( offset_basis == OffsetBasis::Least ) {
			selfUpdateRandom( offset, fill_bit_size, partial );
			return;
		}

		// LSBベースの位置に変換
		size_t real_offset = offset + fill_bit_size - 1;
		size_t least_base_offset; 
		size_t under_size = 0;
		if ( real_offset < BitSize ) {
			least_base_offset = BitSize - 1 - real_offset;
		} else {
			least_base_offset = 0;
			under_size = real_offset - BitSize + 1;
		}

		// アンダーフローしたサイズを考慮してフィルサイズの再設定を行う
		size_t new_fill_size;
		if ( under_size == 0 ) {
			new_fill_size = fill_bit_size;
		} else {
			if ( under_size > fill_bit_size ) {
				new_fill_size = 0;
			} else {
				new_fill_size = fill_bit_size - under_size;
			}
		}

		selfUpdateRandom( least_base_offset, new_fill_size, partial );
	}


	static CUnsignedBitNumber Random( size_t offset = 0,  size_t fill_bit_size = BitSize ) {
		CUnsignedBitNumber num;
		num.selfUpdateRandom(offset , fill_bit_size );
		return num;
	}
	static CUnsignedBitNumber RandomExtend( size_t offset = 0,  size_t fill_bit_size = BitSize , OffsetBasis offset_basis = OffsetBasis::Least ) {
		CUnsignedBitNumber num;
		num.selfUpdateRandomExtend(offset , fill_bit_size  , offset_basis);
		return num;
	}
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