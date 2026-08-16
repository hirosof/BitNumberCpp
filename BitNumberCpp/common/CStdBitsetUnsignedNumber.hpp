#pragma once
/*

以下を利用した符号なし整数型の定義 (CUnsignedBitNumberクラスの再構築版)

	CStdBitsetUnsignedOperation
	CStdBitsetUnsignedStringConversion

開発環境：Visual Studio 2026

*/

#include <stdexcept>
#include <compare>
#include <string>

#ifdef _DEBUG
#include <cstdio>
#endif

#include "CStdBitsetUnsignedOperation.hpp"
#include "CStdBitsetUnsignedStringConversion.hpp"
#include "CBitNumberSupport.hpp"

template<size_t BitSize, typename CharType = char> class CStdBitsetUnsignedNumber {
public:
	static_assert( BitSize > 0, "BitSizeは無効な値です。" );

	using StdBitset = CStdBitsetUnsignedOperation::StdBitset<BitSize>;
	using StdBitsetInnerReference = StdBitset::reference;
	using StdBoolOptional = std::optional<bool>;
	using StdSizeTPair = std::pair<size_t, size_t>;

	using SelfOptional = std::optional<CStdBitsetUnsignedNumber>;
	using SelfPair = std::pair<CStdBitsetUnsignedNumber, CStdBitsetUnsignedNumber>;
	using SelfPairOptional = std::optional<SelfPair>;

	using OffsetBasis = CBitNumberSupport::BitOffsetBasis;
	using ExtractedBitLocation = CBitNumberSupport::ExtractedBitLocation;
	using BitOffsetTwoPointRange = CBitNumberSupport::BitOffsetTwoPointRange;
	using BitOffsetRange = CBitNumberSupport::BitOffsetRange;


	using String = std::basic_string<CharType>;

	using CompareResult = CBitNumberSupport::CompareResult;

	using RealRangeForRandomIssue = CBitNumberSupport::RealRangeForRandomIssue;
	using RandomIssueResult = CBitNumberSupport::RandomIssueResult<CStdBitsetUnsignedNumber>;

	using StringConv = CStdBitsetUnsignedStringConversion<CharType>;
	using ZeroPaddingMode =StringConv::ZeroPaddingMode;
	using OperationForInvalidCharDetected = CBitsetStringConvSupport::OperationForInvalidCharDetected;
	using StringParseProcessedInfo = CBitsetStringConvSupport::ParseProcessedInfo<CharType>;
	using StringParsedData = CBitsetStringConvSupport::ParsedData<CStdBitsetUnsignedNumber, CharType>;

private:
	StdBitset m_raw;

public:

	const StdBitset& getStdBitsetRefConst( void ) const {
		return m_raw;
	}

	StdBitset& getStdBitsetRef( void ) {
		return m_raw;
	}

	const StdBitset getStdBitsetValueConst( void ) const {
		return m_raw;
	}

	StdBitset getStdBitsetValue( void ) {
		return m_raw;
	}

	void setStdBitsetValue( const StdBitset& value ) {
		m_raw = value;
	}

	CStdBitsetUnsignedNumber( ) : m_raw( 0 ) {

	}

	CStdBitsetUnsignedNumber( uint64_t i64value ) : m_raw( i64value ) {

	}

	CStdBitsetUnsignedNumber( const StdBitset& value ) : m_raw( value ) {

	}

	template <size_t FromSize, typename FromCharType = CharType> explicit CStdBitsetUnsignedNumber( const  CStdBitsetUnsignedNumber<FromSize, FromCharType>& from, size_t self_offset_bit_number = 0, size_t from_offset_bit_number = 0 , OffsetBasis basis = OffsetBasis::Least) {
		this->fromCast( from, self_offset_bit_number, from_offset_bit_number, basis );
	}

	~CStdBitsetUnsignedNumber( ) = default;


	StdBoolOptional get( size_t index, OffsetBasis basis = OffsetBasis::Least ) const {
		if ( !CBitNumberSupport::IsValidIndex( BitSize, index ) ) return std::nullopt;
		if ( basis == OffsetBasis::Least ) {
			return m_raw[index];
		} else {
			return m_raw[BitSize - 1 - index];
		}
	}

	bool set( size_t index, bool value = true, OffsetBasis basis = OffsetBasis::Least ) {
		if ( !CBitNumberSupport::IsValidIndex( BitSize, index ) ) return false;
		if ( basis == OffsetBasis::Least ) {
			m_raw[index] = value;
		} else {
			m_raw[BitSize - 1 - index] = value;
		}
		return true;
	}

	bool unset( size_t index, OffsetBasis basis = OffsetBasis::Least ) {
		return this->set( index, false, basis );
	}

	bool rangeSet( size_t start_index, size_t size, bool value = true, OffsetBasis basis = OffsetBasis::Least ) {

		if ( CBitNumberSupport::IsValidIndex<BitSize>( start_index ) == false ) return false;

		auto range_op = CBitNumberSupport::AdjustOffsetRange<BitSize>( start_index, size, basis );

		if ( !range_op.has_value( ) ) return false;

		auto range = range_op.value( );

		for ( size_t i = 0; i < range.size; i++ ) {
			m_raw[i + range.offset] = value;
		}

		return true;
	}

	bool rangeUnset( size_t start_index, size_t size, OffsetBasis basis = OffsetBasis::Least ) {
		return rangeSet( start_index, size, false, basis );
	}

	bool rangeSetIndex( size_t index1, size_t index2, bool value = true, OffsetBasis basis = OffsetBasis::Least ) {

		bool valid_index1 = CBitNumberSupport::IsValidIndex<BitSize>( index1 );
		bool valid_index2 = CBitNumberSupport::IsValidIndex<BitSize>( index2 );
		if(!(valid_index1 || valid_index2 ) ) return false;

		auto range = CBitNumberSupport::AdjustOffsetTwoPointRange( BitSize, index1, index2, basis );

		for ( size_t i = range.offset1; i <= range.offset2; i++ ) {
			m_raw[i] = value;
		}

		return true;
	}

	bool rangeUnsetIndex( size_t index1, size_t index2, OffsetBasis basis = OffsetBasis::Least ) {
		return rangeSetIndex( index1, index2, false, basis );
	}


	StdBitsetInnerReference operator[]( size_t index ) {
		if ( !CBitNumberSupport::IsValidIndex<BitSize>( index ) ) throw std::out_of_range( "Index is out of range." );
		return m_raw[index];
	}

	bool operator[]( size_t index ) const {
		if ( !CBitNumberSupport::IsValidIndex<BitSize>( index ) ) throw std::out_of_range( "Index is out of range." );
		return m_raw[index];
	}


	// 指定された位置から最上位ビットまでの値をクリアする (0にする)
	bool clear( size_t start_index = 0  , OffsetBasis basis = OffsetBasis::Least ) {
		if(!CBitNumberSupport::IsValidIndex<BitSize>(start_index ) ) return false;

		size_t least_based_start_index = ( basis == OffsetBasis::Least ) ? start_index : ( BitSize - 1 - start_index );

		return rangeUnsetIndex( least_based_start_index, BitSize - 1, OffsetBasis::Least );
	}


	// 指定された位置から最上位ビットまでの値をセットする (1にする)
	bool fill( size_t start_index = 0 , OffsetBasis basis = OffsetBasis::Least ) {
		if(!CBitNumberSupport::IsValidIndex<BitSize>(start_index ) ) return false;

		size_t least_based_start_index = ( basis == OffsetBasis::Least ) ? start_index : ( BitSize - 1 - start_index );

		return rangeSetIndex( least_based_start_index, BitSize - 1, true, OffsetBasis::Least );
	}

	bool isAllOne( ) const {
		return m_raw.all( );
	}
	
	bool isNotZero( ) const {
		return m_raw.any( );
	}

	bool isZero( ) const {
		return m_raw.none( );
	}

	CStdBitsetUnsignedNumber extract( size_t offset, size_t size, ExtractedBitLocation extracted_bit_location = ExtractedBitLocation::LeastSignificant ) const {
		return extract( offset, size, OffsetBasis::Least, extracted_bit_location );
	}

	template<typename T> CStdBitsetUnsignedNumber extractType( size_t offset, ExtractedBitLocation extracted_bit_location = ExtractedBitLocation::LeastSignificant ) const {
		return extract( offset, sizeof( T ) * 8, OffsetBasis::Least, extracted_bit_location );
	}
	template<typename T> CStdBitsetUnsignedNumber extractType( size_t offset, OffsetBasis basis , ExtractedBitLocation extracted_bit_location = ExtractedBitLocation::LeastSignificant ) const {
		return extract( offset, sizeof( T ) * 8, basis, extracted_bit_location );
	}

	CStdBitsetUnsignedNumber extract( size_t offset, size_t size, OffsetBasis basis, ExtractedBitLocation extracted_bit_location = ExtractedBitLocation::LeastSignificant ) const {

		std::optional<BitOffsetRange> range = CBitNumberSupport::AdjustOffsetRange( BitSize, offset, size, basis );

		// 指定された範囲が無効な場合、抽出した値は0である。
		if(!range.has_value() ) return CStdBitsetUnsignedNumber( 0 );

		CStdBitsetUnsignedNumber result;

		size_t start_index_for_result = 0;

		if ( extracted_bit_location == ExtractedBitLocation::Original ) {
			start_index_for_result = range->offset;
		} else if ( extracted_bit_location == ExtractedBitLocation::MostSignificant ) {
			start_index_for_result = BitSize - range->size;
		} else {
			start_index_for_result = 0;
		}

		for ( size_t i = 0; i < range->size; i++ ) {
			result.m_raw[start_index_for_result + i] = m_raw[range->offset + i];
		}

		return result;
	}

	CStdBitsetUnsignedNumber extract( const BitOffsetRange& range, ExtractedBitLocation extracted_bit_location = ExtractedBitLocation::LeastSignificant ) const {
		return extract( range.offset, range.size, range.basis, extracted_bit_location );
	}


	CStdBitsetUnsignedNumber extractIndex( size_t index1, size_t index2, ExtractedBitLocation extracted_bit_location = ExtractedBitLocation::LeastSignificant ) const {
		return extractIndex( index1, index2, OffsetBasis::Least, extracted_bit_location );
	}

	CStdBitsetUnsignedNumber extractIndex( size_t index1, size_t index2, OffsetBasis basis ,ExtractedBitLocation extracted_bit_location = ExtractedBitLocation::LeastSignificant ) const {


		if ( !( CBitNumberSupport::IsValidIndex<BitSize>( index1 ) || CBitNumberSupport::IsValidIndex<BitSize>( index2 ) ) ) {
			// 指定されたindexがどちらも無効な場合、 すなわち、抽出するべき値がない場合、抽出した値は0である。
			return CStdBitsetUnsignedNumber( 0 );
		}

		BitOffsetTwoPointRange range = CBitNumberSupport::AdjustOffsetTwoPointRange( BitSize, index1, index2, basis );

		return extract( range.offset1, range.offset2 - range.offset1 + 1, OffsetBasis::Least, extracted_bit_location );

	}

	CStdBitsetUnsignedNumber extractIndex( const BitOffsetTwoPointRange& range, ExtractedBitLocation extracted_bit_location = ExtractedBitLocation::LeastSignificant ) const {
		return extractIndex( range.offset1, range.offset2, range.basis, extracted_bit_location );
	}

	CStdBitsetUnsignedNumber rangeGet( size_t start_index, size_t size, OffsetBasis basis = OffsetBasis::Least ) const {
		return extract( start_index, size, basis, ExtractedBitLocation::Original );
	}

	CStdBitsetUnsignedNumber rangeGetIndex( size_t index1, size_t index2, OffsetBasis basis = OffsetBasis::Least ) const {
		return extractIndex( index1, index2, basis, ExtractedBitLocation::Original );
	}


private:

	// 符号なし整数型への変換
	template<typename UIntTypeName> UIntTypeName toUIntType( size_t offset_bit_number = 0 , OffsetBasis basis = OffsetBasis::Least ) const {

		static_assert(
			std::is_unsigned_v<UIntTypeName> && std::is_integral_v<UIntTypeName>,
			"UIntTypeName は符号なし整数型である必要があります。"
			);

		if ( !CBitNumberSupport::IsValidIndex<BitSize>( offset_bit_number ) ) {
			return 0;
		}

		CStdBitsetUnsignedNumber extracted_value = extractType<UIntTypeName>( offset_bit_number, basis, ExtractedBitLocation::LeastSignificant );

		UIntTypeName result = 0;

		size_t result_bit_size = std::min( BitSize, sizeof( UIntTypeName ) * 8 );

		for ( size_t i = 0; i < result_bit_size; i++ ) {
			if ( extracted_value.m_raw[i] ) {
				result |= static_cast<UIntTypeName>( 1 ) << i;
			}
		}

		return result;
	}


public:



	template <size_t ToSize, typename ToCharType = CharType> CStdBitsetUnsignedNumber<ToSize, ToCharType> toCast( size_t self_offset_bit_number = 0, size_t to_offset_bit_number = 0, OffsetBasis basis = OffsetBasis::Least ) const {
		CStdBitsetUnsignedNumber<ToSize, ToCharType> result;
		result.template fromCast<BitSize, CharType>( *this, to_offset_bit_number, self_offset_bit_number, basis );
		return result;
	}

	template<size_t ToSize> CStdBitsetUnsignedNumber<ToSize, CharType> toSizeCast( size_t self_offset_bit_number = 0, size_t to_offset_bit_number = 0, OffsetBasis basis = OffsetBasis::Least ) const {
		return toCast<ToSize, CharType>( self_offset_bit_number, to_offset_bit_number, basis );
	}

	template<typename ToCharType> CStdBitsetUnsignedNumber<BitSize, ToCharType> toCharCast( size_t self_offset_bit_number = 0, size_t to_offset_bit_number = 0, OffsetBasis basis = OffsetBasis::Least ) const {
		return toCast<BitSize, ToCharType>( self_offset_bit_number, to_offset_bit_number, basis );
	}	


	uint8_t toUInt8( size_t offset_bit_number = 0, OffsetBasis basis = OffsetBasis::Least ) const {
		return toUIntType<uint8_t>( offset_bit_number, basis );
	}

	uint16_t toUInt16( size_t offset_bit_number = 0, OffsetBasis basis = OffsetBasis::Least ) const {
		return toUIntType<uint16_t>( offset_bit_number, basis );
	}

	uint32_t toUInt32( size_t offset_bit_number = 0, OffsetBasis basis = OffsetBasis::Least ) const {
		return toUIntType<uint32_t>( offset_bit_number, basis );
	}

	uint64_t toUInt64( size_t offset_bit_number = 0, OffsetBasis basis = OffsetBasis::Least ) const {
		return toUIntType<uint64_t>( offset_bit_number, basis );
	}

	uint8_t toUInt8FromLeast( void ) const {
		return toUIntType<uint8_t>( 0, OffsetBasis::Least );
	}

	uint8_t toUInt8FromMost( void ) const {
		return toUIntType<uint8_t>( 0, OffsetBasis::Most );
	}

	uint16_t toUInt16FromLeast( void ) const {
		return toUIntType<uint16_t>( 0, OffsetBasis::Least );
	}
	
	uint16_t toUInt16FromMost( void ) const {
		return toUIntType<uint16_t>( 0, OffsetBasis::Most );
	}

	uint32_t toUInt32FromLeast( void ) const {
		return toUIntType<uint32_t>( 0, OffsetBasis::Least );
	}

	uint32_t toUInt32FromMost( void ) const {
		return toUIntType<uint32_t>( 0, OffsetBasis::Most );
	}

	uint64_t toUInt64FromLeast( void ) const {
		return toUIntType<uint64_t>( 0, OffsetBasis::Least );
	}

	uint64_t toUInt64FromMost( void ) const {
		return toUIntType<uint64_t>( 0, OffsetBasis::Most );
	}

private:

	template<typename UIntTypeName> void fromUIntType( UIntTypeName value, size_t self_offset_bit_number = 0 , OffsetBasis basis = OffsetBasis::Least ) {
		static_assert(
			std::is_unsigned_v<UIntTypeName> && std::is_integral_v<UIntTypeName>,
			"UIntTypeName は符号なし整数型である必要があります。"
			);

		size_t value_bit_size = std::min( BitSize, sizeof( UIntTypeName ) * 8 );

		clear( );

		if ( CBitNumberSupport::IsValidIndex<BitSize>( self_offset_bit_number ) ) {

			std::optional<BitOffsetRange> range = CBitNumberSupport::AdjustOffsetRange( BitSize, self_offset_bit_number, value_bit_size, basis );

			size_t least_based_offset = 0;
			size_t size = value_bit_size;

			if ( range.has_value( ) ) {
				least_based_offset = range->offset;
				size = range->size;
			}

			for ( size_t i = 0; i < size; i++ ) {
				if ( value & ( static_cast<UIntTypeName>( 1 ) << i ) ) {
					this->m_raw[i + least_based_offset] = true;
				}
			}
		}

	}

public:

	template <size_t FromSize, typename FromCharType = CharType> void fromCast( const  CStdBitsetUnsignedNumber<FromSize, FromCharType>& from, size_t self_offset_bit_number = 0, size_t from_offset_bit_number = 0 , OffsetBasis basis = OffsetBasis::Least ) {
		
		std::optional<BitOffsetRange> from_range = CBitNumberSupport::AdjustOffsetRange( FromSize, from_offset_bit_number, BitSize , basis );

		if ( !from_range.has_value( ) ) {
			this->m_raw.reset( );
			return;
		}

		if ( from_range->size == 0 ) {
			this->m_raw.reset( );
			return;
		}

	
		std::optional<BitOffsetRange> self_range = CBitNumberSupport::AdjustOffsetRange( BitSize, self_offset_bit_number, from_range->size , basis );

		if ( !self_range.has_value( ) ) {
			this->m_raw.reset( );
			return;
		}

		if ( self_range->size == 0 ) {
			this->m_raw.reset( );
			return;
		}
	
		if ( self_range->size < from_range->size ) {
			// コピー先のサイズがコピー元より小さい場合、コピー元を再計算する
			from_range = CBitNumberSupport::AdjustOffsetRange( FromSize, from_offset_bit_number, self_range->size, basis );


			// 以下2点のチェックは先のチェックしている影響で真にはならないが、念のために残しておく
			if ( !from_range.has_value( ) ) {
				this->m_raw.reset( );
				return;
			}

			if ( from_range->size == 0 ) {
				this->m_raw.reset( );
				return;
			}
		}


		this->m_raw = CStdBitsetUnsignedOperation::CastSize<BitSize>( from.extract(from_range.value() , ExtractedBitLocation::LeastSignificant ).getStdBitsetRefConst( ) );
		this->m_raw <<= self_range->offset;

	}

	template <size_t FromSize> void fromSizeCast( const  CStdBitsetUnsignedNumber<FromSize, CharType>& from, size_t self_offset_bit_number = 0, size_t from_offset_bit_number = 0 , OffsetBasis basis = OffsetBasis::Least ) {
		 fromCast<FromSize, CharType>( from, self_offset_bit_number, from_offset_bit_number, basis );
		 return;
	}

	template <typename FromCharType> void fromCharCast( const  CStdBitsetUnsignedNumber<BitSize, FromCharType>& from, size_t self_offset_bit_number = 0, size_t from_offset_bit_number = 0 , OffsetBasis basis = OffsetBasis::Least ) {
		fromCast<BitSize, FromCharType>( from, self_offset_bit_number, from_offset_bit_number, basis );
		return;
	}


	void fromUInt8( uint8_t value, size_t self_offset_bit_number = 0, OffsetBasis basis = OffsetBasis::Least ) {
		fromUIntType( value, self_offset_bit_number, basis );
	}

	void fromUInt16( uint16_t value, size_t self_offset_bit_number = 0, OffsetBasis basis = OffsetBasis::Least ) {
		fromUIntType( value, self_offset_bit_number, basis );
	}

	void fromUInt32( uint32_t value, size_t self_offset_bit_number = 0, OffsetBasis basis = OffsetBasis::Least ) {
		fromUIntType( value, self_offset_bit_number, basis );
	}

	void fromUInt64( uint64_t value, size_t self_offset_bit_number = 0, OffsetBasis basis = OffsetBasis::Least ) {
		fromUIntType( value, self_offset_bit_number, basis );
	}


	void fromUInt8ToLeast( uint8_t value ) {
		fromUIntType( value, 0, OffsetBasis::Least );
	}

	void fromUInt8ToMost( uint8_t value ) {
		fromUIntType( value, 0, OffsetBasis::Most );
	}

	void fromUInt16ToLeast( uint16_t value ) {
		fromUIntType( value, 0, OffsetBasis::Least );
	}

	void fromUInt16ToMost( uint16_t value ) {
		fromUIntType( value, 0, OffsetBasis::Most );
	}

	void fromUInt32ToLeast( uint32_t value ) {
		fromUIntType( value, 0, OffsetBasis::Least );
	}

	void fromUInt32ToMost( uint32_t value ) {
		fromUIntType( value, 0, OffsetBasis::Most );
	}

	void fromUInt64ToLeast( uint64_t value ) {
		fromUIntType( value, 0, OffsetBasis::Least );
	}

	void fromUInt64ToMost( uint64_t value ) {
		fromUIntType( value, 0, OffsetBasis::Most );
	}

public:

	// 有効なビット長を取得する
	size_t getSignificantBitLength( ) const {
		return CStdBitsetUnsignedOperation::GetSignificantBitLength( m_raw );
	}


	CompareResult compare( const CStdBitsetUnsignedNumber& target ) const {
		return CStdBitsetUnsignedOperation::Compare( m_raw, target.m_raw );
	}

	template <size_t TargetSize, typename TargetCharType = CharType> CompareResult compareExtend( const CStdBitsetUnsignedNumber<TargetSize, TargetCharType>& target ) const {
		return CStdBitsetUnsignedOperation::CompareExtend( m_raw, target.getStdBitsetRefConst() );
	}


	bool equal( const CStdBitsetUnsignedNumber& target ) const {
		return compare( target ) == CompareResult::Equal;
	}

	template <size_t TargetSize, typename TargetCharType = CharType> bool equalExtend( const CStdBitsetUnsignedNumber<TargetSize, TargetCharType>& target ) const {
		return compareExtend( target ) == CompareResult::Equal;
	}

	std::strong_ordering operator <=> ( const CStdBitsetUnsignedNumber& rhs ) const {

		CompareResult cr = compare( rhs );

		if ( cr == CompareResult::SelfGreater ) {
			return std::strong_ordering::greater;
		} else if ( cr == CompareResult::TargetGreater ) {
			return std::strong_ordering::less;
		} else {
			return std::strong_ordering::equal;
		}

	}

	bool  operator== ( const CStdBitsetUnsignedNumber& rhs ) const {
		return equal( rhs );
	}


	/*
	 大きい方のCStdBitsetUnsignedNumberを返すスタティック関数
	*/

	static CStdBitsetUnsignedNumber Max( const CStdBitsetUnsignedNumber& a, const CStdBitsetUnsignedNumber& b ) {
		return ( a >= b ) ? a : b;
	}

	/*
	 小さい方のCStdBitsetUnsignedNumberを返すスタティック関数
	*/
	static CStdBitsetUnsignedNumber Min( const CStdBitsetUnsignedNumber& a, const CStdBitsetUnsignedNumber& b ) {
		return ( a <= b ) ? a : b;
	}


	/*
	論理演算系
*/

	CStdBitsetUnsignedNumber logical_and( const CStdBitsetUnsignedNumber& value ) const {
		return CStdBitsetUnsignedNumber( m_raw & value.m_raw );
	}

	CStdBitsetUnsignedNumber logical_or( const CStdBitsetUnsignedNumber& value ) const {
		return CStdBitsetUnsignedNumber( m_raw | value.m_raw );
	}


	CStdBitsetUnsignedNumber logical_not( void ) const {
		return CStdBitsetUnsignedNumber( ~this->m_raw );
	}

	CStdBitsetUnsignedNumber logical_xor( const CStdBitsetUnsignedNumber& value ) const {
		return CStdBitsetUnsignedNumber( this->m_raw ^ value.m_raw );
	}


	CStdBitsetUnsignedNumber  selfUpdate_and( const CStdBitsetUnsignedNumber& value ) {
		*this = this->logical_and( value );
		return *this;
	}

	CStdBitsetUnsignedNumber  selfUpdate_or( const CStdBitsetUnsignedNumber& value ) {
		*this = this->logical_or( value );
		return *this;
	}

	CStdBitsetUnsignedNumber  selfUpdate_not( ) {
		*this = this->logical_not( );
		return *this;
	}

	CStdBitsetUnsignedNumber  selfUpdate_xor( const CStdBitsetUnsignedNumber& value ) {
		*this = this->logical_xor( value );
		return *this;
	}


	CStdBitsetUnsignedNumber& operator&=( const CStdBitsetUnsignedNumber& rhs ) {
		selfUpdate_and( rhs );
		return *this;
	}

	CStdBitsetUnsignedNumber& operator|=( const CStdBitsetUnsignedNumber	& rhs ) {
		selfUpdate_or( rhs );
		return *this;
	}

	CStdBitsetUnsignedNumber& operator^=( const CStdBitsetUnsignedNumber& rhs ) {
		selfUpdate_xor( rhs );
		return *this;
	}


	CStdBitsetUnsignedNumber operator~( ) const {
		return logical_not( );
	}

	CStdBitsetUnsignedNumber operator&( const CStdBitsetUnsignedNumber& rhs ) const {
		return logical_and( rhs );
	}

	CStdBitsetUnsignedNumber operator|( const CStdBitsetUnsignedNumber& rhs ) const {
		return logical_or( rhs );
	}

	CStdBitsetUnsignedNumber operator^( const CStdBitsetUnsignedNumber& rhs ) const {
		return logical_xor( rhs );
	}

	/*
		シフト演算
	*/

	CStdBitsetUnsignedNumber shiftLeft( size_t shift ) const {
		return CStdBitsetUnsignedNumber( this->m_raw << shift );
	}

	CStdBitsetUnsignedNumber shiftRight( size_t shift ) const {
		return CStdBitsetUnsignedNumber( this->m_raw >> shift );
	}

	CStdBitsetUnsignedNumber selfUpdateShiftLeft( size_t shift ) {
		this->m_raw <<= shift;
		return *this;
	}

	CStdBitsetUnsignedNumber selfUpdateShiftRight( size_t shift ) {
		this->m_raw >>= shift;
		return *this;
	}


	CStdBitsetUnsignedNumber operator<<( size_t shift ) const {
		return this->shiftLeft( shift );
	}

	CStdBitsetUnsignedNumber operator>>( size_t shift ) const {
		return this->shiftRight( shift );
	}

	CStdBitsetUnsignedNumber& operator<<=( size_t shift ) {
		this->selfUpdateShiftLeft( shift );
		return *this;
	}

	CStdBitsetUnsignedNumber& operator>>=( size_t shift ) {
		this->selfUpdateShiftRight( shift );
		return *this;
	}


	/*
		ローテート演算
	*/

	CStdBitsetUnsignedNumber rotateLeft( size_t rotate ) const {
		return CStdBitsetUnsignedNumber( CStdBitsetUnsignedOperation::RotateLeft( this->m_raw, rotate ) );
	}

	CStdBitsetUnsignedNumber rotateRight( size_t rotate ) const {
		return CStdBitsetUnsignedNumber( CStdBitsetUnsignedOperation::RotateRight( this->m_raw, rotate ) );
	}

	CStdBitsetUnsignedNumber selfUpdateRotateLeft( size_t rotate ) {
		this->m_raw = CStdBitsetUnsignedOperation::RotateLeft( this->m_raw, rotate );
		return *this;
	}

	CStdBitsetUnsignedNumber selfUpdateRotateRight( size_t rotate ) {
		this->m_raw = CStdBitsetUnsignedOperation::RotateRight( this->m_raw, rotate );
		return *this;
	}


	/*
		加算系の実装
	*/
	CStdBitsetUnsignedNumber additionWithCarryParam( const CStdBitsetUnsignedNumber& value, const bool input_carry = false, bool* const pLastCarry = nullptr ) const {
		auto pre_result = CStdBitsetUnsignedOperation::Addition<BitSize>( this->m_raw, value.m_raw, input_carry, pLastCarry );
		return CStdBitsetUnsignedNumber( pre_result );
	}


	CStdBitsetUnsignedNumber selfUpdateAdditionWithCarryParam( const CStdBitsetUnsignedNumber& value, const bool input_carry = false, bool* const pLastCarry = nullptr ) {
		auto pre_result = additionWithCarryParam( value, input_carry, pLastCarry );
		this->m_raw = pre_result.m_raw;
		return pre_result;
	}


	CStdBitsetUnsignedNumber addition( const CStdBitsetUnsignedNumber& value ) const {
		return additionWithCarryParam( value, false, nullptr );
	}


	CStdBitsetUnsignedNumber selfUpdateAddition( const CStdBitsetUnsignedNumber& value ) {
		auto pre_result = addition( value );
		this->m_raw = pre_result.m_raw;
		return pre_result;
	}


	CStdBitsetUnsignedNumber& operator+=( const CStdBitsetUnsignedNumber& rhs ) {
		this->selfUpdateAddition( rhs );
		return *this;
	}


	CStdBitsetUnsignedNumber operator+( const CStdBitsetUnsignedNumber& rhs ) const {
		CStdBitsetUnsignedNumber lhs( *this );
		lhs += rhs;
		return lhs;
	}


	CStdBitsetUnsignedNumber& operator++( ) {
		this->m_raw = CStdBitsetUnsignedOperation::Increment( this->m_raw );
		return *this;
	}

	CStdBitsetUnsignedNumber operator++( int ) {
		CStdBitsetUnsignedNumber old( *this );
		this->m_raw = CStdBitsetUnsignedOperation::Increment( this->m_raw );
		return old;
	}


	/*

		減算系の実装
	*/

	CStdBitsetUnsignedNumber subtraction( const CStdBitsetUnsignedNumber& value )const {
		CStdBitsetUnsignedNumber result;
		result.m_raw = CStdBitsetUnsignedOperation::Subtraction( this->m_raw, value.m_raw );
		return result;
	}

	CStdBitsetUnsignedNumber selfUpdateSubtraction( const CStdBitsetUnsignedNumber& value ) {
		CStdBitsetUnsignedNumber result = subtraction( value );
		this->m_raw = result.m_raw;
		return result;
	}

	CStdBitsetUnsignedNumber& operator-=( const CStdBitsetUnsignedNumber& rhs ) {
		this->selfUpdateSubtraction( rhs );
		return *this;
	}



	CStdBitsetUnsignedNumber operator-( const CStdBitsetUnsignedNumber& rhs ) const {
		CStdBitsetUnsignedNumber lhs( *this );
		lhs -= rhs;
		return lhs;
	}


	CStdBitsetUnsignedNumber& operator--( ) {
		this->m_raw = CStdBitsetUnsignedOperation::Decrement( this->m_raw );
		return *this;
	}

	CStdBitsetUnsignedNumber operator--( int ) {
		CStdBitsetUnsignedNumber old( *this );
		this->m_raw = CStdBitsetUnsignedOperation::Decrement( this->m_raw );
		return old;
	}


	/*
		掛け算
	*/

	CStdBitsetUnsignedNumber multiplication( const CStdBitsetUnsignedNumber& value ) const {
		CStdBitsetUnsignedNumber result;
		result.m_raw = CStdBitsetUnsignedOperation::Multiplication( this->m_raw, value.m_raw );
		return result;
	}

	CStdBitsetUnsignedNumber selfUpdateMultiplication( const CStdBitsetUnsignedNumber& value ) {
		CStdBitsetUnsignedNumber result = multiplication( value );
		this->m_raw = result.m_raw;
		return result;
	}

	CStdBitsetUnsignedNumber& operator*=( const CStdBitsetUnsignedNumber& rhs ) {
		this->selfUpdateMultiplication( rhs );
		return *this;
	}

	CStdBitsetUnsignedNumber operator*( const CStdBitsetUnsignedNumber& rhs ) const {
		CStdBitsetUnsignedNumber lhs( *this );
		lhs *= rhs;
		return lhs;
	}

	/*
		10倍
	*/

	CStdBitsetUnsignedNumber multiplication10( void ) const {
		CStdBitsetUnsignedNumber result;
		result.m_raw = CStdBitsetUnsignedOperation::Multiplication10( this->m_raw );
		return result;
	}

	CStdBitsetUnsignedNumber selfUpdateMultiplication10( void ) {
		CStdBitsetUnsignedNumber result = multiplication10( );
		this->m_raw = result.m_raw;
		return result;
	}

	/*
		除算
	*/
	SelfOptional  division( const CStdBitsetUnsignedNumber& value ) const {
		auto pre_result = this->divisionWithRemainder( value );
		if ( pre_result.has_value( ) ) {
			CStdBitsetUnsignedNumber result( pre_result->first );
			return result;
		}
		return std::nullopt;
	}

	SelfOptional  selfUpdateDivision( const CStdBitsetUnsignedNumber& value ) {
		auto result = this->division( value );
		if ( result.has_value( ) ) {
			this->m_raw = result.value( ).m_raw;
		}
		return result;
	}


	CStdBitsetUnsignedNumber& operator/=( const CStdBitsetUnsignedNumber& rhs ) {

		if ( rhs.m_raw.none( ) ) throw std::domain_error( "CStdBitsetUnsignedNumber：0除算が発生しました。" );

		SelfOptional result = this->selfUpdateDivision( rhs );

		if ( !result.has_value( ) ) {
			// 本オペレータの初回実装時において、ここのブロックは(先に0除算チェックをしている影響で)
			// 実行されないが念のため、例外を発行しておく
			throw std::domain_error( "CStdBitsetUnsignedNumber：除算の算出に失敗しました。" );
		}

		return *this;
	}

	CStdBitsetUnsignedNumber operator/( const CStdBitsetUnsignedNumber& rhs ) const {
		CStdBitsetUnsignedNumber lhs( *this );
		lhs /= rhs;
		return lhs;
	}

	/*
		剰余
	*/
	SelfOptional remainder( const CStdBitsetUnsignedNumber& value ) const {
		auto pre_result = this->divisionWithRemainder( value );
		if ( pre_result.has_value( ) ) {
			CStdBitsetUnsignedNumber result( pre_result->second );
			return result;
		}
		return std::nullopt;
	}

	SelfOptional  selfUpdateRemainder( const CStdBitsetUnsignedNumber& value ) {
		auto result = this->remainder( value );
		if ( result.has_value( ) ) {
			this->m_raw = result.value( ).m_raw;
		}
		return result;
	}

	CStdBitsetUnsignedNumber& operator%=( const CStdBitsetUnsignedNumber& rhs ) {

		if ( rhs.m_raw.none( ) ) throw std::domain_error( "CStdBitsetUnsignedNumber：0除算が発生しました。" );

		SelfOptional result = this->selfUpdateRemainder( rhs );

		if ( !result.has_value( ) ) {
			// 本オペレータの初回実装時において、ここのブロックは(先に0除算チェックをしている影響で)
			// 実行されないが念のため、例外を発行しておく
			throw std::domain_error( "CStdBitsetUnsignedNumber：剰余の算出に失敗しました。" );
		}

		return *this;
	}

	CStdBitsetUnsignedNumber operator%( const CStdBitsetUnsignedNumber& rhs ) const {
		CStdBitsetUnsignedNumber lhs( *this );
		lhs %= rhs;
		return lhs;
	}

	/*
		除算・剰余
	*/

	SelfPairOptional  divisionWithRemainder( const CStdBitsetUnsignedNumber& value ) const {
		auto pre_result = CStdBitsetUnsignedOperation::DivisionWithRemainder<BitSize>( this->m_raw, value.m_raw );
		if ( pre_result.has_value( ) ) {
			return SelfPair( CStdBitsetUnsignedNumber( pre_result->first ), CStdBitsetUnsignedNumber( pre_result->second ) );
		}
		return std::nullopt;
	}

	/*
		乱数生成系
	*/

	RealRangeForRandomIssue selfUpdateRandom( const size_t offset = 0, const size_t fill_bit_size = BitSize, const bool partial = false ) {

		return this->selfUpdateRandomExtend( offset, fill_bit_size, OffsetBasis::Least, partial );
	}


	RealRangeForRandomIssue selfUpdateRandomExtend( const  size_t offset = 0, const size_t fill_bit_size = BitSize, const OffsetBasis offset_basis = OffsetBasis::Least, const  bool partial = false ) {

		RealRangeForRandomIssue issue;
		issue.fill_bit_size = 0;
		issue.offset_of_least = 0;


		if ( !CBitNumberSupport::IsValidIndex<BitSize>( offset ) ) {
			return issue;
		}

		std::optional<BitOffsetRange> range = CBitNumberSupport::AdjustOffsetRange( BitSize, offset, fill_bit_size, offset_basis );

		if ( !range.has_value( ) ) {
			return issue;
		}

		if ( !partial ) {
			this->clear( );
		}

		StdBitset random_bits = CStdBitsetUnsignedOperation::Random<BitSize>( range->size );

		for ( size_t i = 0; i < range->size; i++ ) {
			this->m_raw[range->offset + i] = random_bits[i];
		}

		issue.fill_bit_size = range->size;
		issue.offset_of_least = range->offset;

		return issue;
	}


	static CStdBitsetUnsignedNumber Random( size_t offset = 0, size_t fill_bit_size = BitSize ) {
		CStdBitsetUnsignedNumber res;
		res.selfUpdateRandom( offset, fill_bit_size );
		return res;
	}

	static CStdBitsetUnsignedNumber RandomExtend( size_t offset = 0, size_t fill_bit_size = BitSize, OffsetBasis offset_basis = OffsetBasis::Least ) {
		CStdBitsetUnsignedNumber res;
		res.selfUpdateRandomExtend( offset, fill_bit_size, offset_basis );
		return res;
	}


	static RandomIssueResult RandomWithRangeInfo( size_t offset = 0, size_t fill_bit_size = BitSize ) {
		RandomIssueResult res;
		res.realRange = res.value.selfUpdateRandom( offset, fill_bit_size );
		return res;
	}

	static RandomIssueResult RandomExtendWithRangeInfo( size_t offset = 0, size_t fill_bit_size = BitSize, OffsetBasis offset_basis = OffsetBasis::Least ) {
		RandomIssueResult res;
		res.realRange = res.value.selfUpdateRandomExtend( offset, fill_bit_size, offset_basis );
		return res;
	}


	/*
		文字列への変換系
	*/


	String toBinaryString( ZeroPaddingMode zero_padding_mode = ZeroPaddingMode::NoPadding )const {
		return StringConv::ToBinaryString( this->m_raw, zero_padding_mode );
	}

	String toDecimalString( )const {
		return StringConv::ToDecimalString( this->m_raw );
	}

	String toHexadecimalString( bool upper_case = false , ZeroPaddingMode zero_padding_mode = ZeroPaddingMode::NoPadding ) const {
		return StringConv::ToHexadecimalString( this->m_raw, upper_case, zero_padding_mode );
	}



	String toJsonLikeString( bool enableSeparate = false , ZeroPaddingMode zero_padding_mode = ZeroPaddingMode::NoPadding )const {


		String s, trans;

		//open block
		s.push_back( '{' );
		s.push_back( ' ' );

		//bin
		s.append( { '\"' ,  'b' , 'i' , 'n' , '\"' ,  ':' , '\"' } );

		trans = this->toBinaryString(zero_padding_mode );
		if ( enableSeparate ) {
			s.append( StringConv::CreateSpaceSeparatedString( trans, 4 ) );
		} else {
			s.append( trans );
		}

		s.push_back( '\"' );


		//dec
		s.append( { ' ', ',', ' ' } );
		s.append( { '\"' ,  'd' , 'e' , 'c' , '\"' ,  ':' , '\"' } );

		trans = this->toDecimalString( );
		if ( enableSeparate ) {
			s.append( StringConv::CreateCommaSeparatedString( trans, 3 ) );
		} else {
			s.append( trans );
		}


		s.push_back( '\"' );


		//hex
		s.append( { ' ', ',', ' ' } );
		s.append( { '\"' ,  'h' , 'e' , 'x' , '\"' ,  ':' , '\"' } );

		trans = this->toHexadecimalString( true, zero_padding_mode );
		if ( enableSeparate ) {
			s.append( StringConv::CreateSpaceSeparatedString( trans, 4 ) );
		} else {
			s.append( trans );
		}

		s.push_back( '\"' );

		//close block
		s.push_back( ' ' );
		s.push_back( '}' );

		return s;


	}

	/*
		文字列からの変換系
	*/


	void  fromBinaryString( const String& str, const String& valid_separators = StringConv::DEFAULT_VALID_SEPARATORS ) {
		this->fromBinaryStringStrict( str, OperationForInvalidCharDetected::PartialReturn, valid_separators );
	}

	StringParseProcessedInfo  fromBinaryStringStrict( const String& str, const  OperationForInvalidCharDetected operation_invalid_char_detected, const String& valid_separators = StringConv::DEFAULT_VALID_SEPARATORS ) {
		auto parsed = StringConv::template FromBinaryStringStrict < BitSize>( str, operation_invalid_char_detected, valid_separators );
		this->m_raw = parsed.value;
		return parsed.info;
	}

	void  fromBinaryStringPriorityLSB( const String& str, const String& valid_separators = StringConv::DEFAULT_VALID_SEPARATORS ) {
		this->fromBinaryStringPriorityLSBStrict( str, OperationForInvalidCharDetected::PartialReturn, valid_separators );
	}

	StringParseProcessedInfo  fromBinaryStringPriorityLSBStrict( const String& str, const  OperationForInvalidCharDetected operation_invalid_char_detected, const String& valid_separators = StringConv::DEFAULT_VALID_SEPARATORS ) {
		auto parsed = StringConv::template FromBinaryStringPriorityLSBStrict< BitSize>( str, operation_invalid_char_detected, valid_separators );
		this->m_raw = parsed.value;
		return parsed.info;
	}

	void  fromDecimalString( const String& str, const String& valid_separators = StringConv::DEFAULT_VALID_SEPARATORS ) {
		this->fromDecimalStringStrict( str, OperationForInvalidCharDetected::PartialReturn, valid_separators );
	}

	StringParseProcessedInfo  fromDecimalStringStrict( const String& str, const  OperationForInvalidCharDetected operation_invalid_char_detected, const String& valid_separators = StringConv::DEFAULT_VALID_SEPARATORS ) {
		auto parsed = StringConv::template FromDecimalStringStrict < BitSize>( str, operation_invalid_char_detected, valid_separators );
		this->m_raw = parsed.value;
		return parsed.info;
	}


	void  fromHexadecimalString( const String& str, const String& valid_separators = StringConv::DEFAULT_VALID_SEPARATORS ) {
		this->fromHexadecimalStringStrict( str, OperationForInvalidCharDetected::PartialReturn, valid_separators );
	}

	StringParseProcessedInfo  fromHexadecimalStringStrict( const String& str, const  OperationForInvalidCharDetected operation_invalid_char_detected, const String& valid_separators = StringConv::DEFAULT_VALID_SEPARATORS ) {
		auto parsed = StringConv::template FromHexadecimalStringStrict< BitSize>( str, operation_invalid_char_detected, valid_separators );
		this->m_raw = parsed.value;
		return parsed.info;
	}

	void  fromHexadecimalStringPriorityLSB( const String& str, const String& valid_separators = StringConv::DEFAULT_VALID_SEPARATORS ) {
		this->fromHexadecimalStringPriorityLSBStrict( str, OperationForInvalidCharDetected::PartialReturn, valid_separators );
	}

	StringParseProcessedInfo  fromHexadecimalStringPriorityLSBStrict( const String& str, const  OperationForInvalidCharDetected operation_invalid_char_detected, const String& valid_separators = StringConv::DEFAULT_VALID_SEPARATORS ) {
		auto parsed = StringConv::template FromHexadecimalStringPriorityLSBStrict< BitSize>( str, operation_invalid_char_detected, valid_separators );
		this->m_raw = parsed.value;
		return parsed.info;
	}


	static  CStdBitsetUnsignedNumber  CreateFromBinaryString( const String& str, const String& valid_separators = StringConv::DEFAULT_VALID_SEPARATORS ) {
		CStdBitsetUnsignedNumber ubn;
		ubn.fromBinaryString( str, valid_separators );
		return ubn;
	}

	static  StringParsedData  CreateFromBinaryStringStrict( const String& str, const  OperationForInvalidCharDetected operation_invalid_char_detected, const String& valid_separators = StringConv::DEFAULT_VALID_SEPARATORS ) {
		StringParsedData res;
		res.info = res.value.fromBinaryStringStrict( str, operation_invalid_char_detected, valid_separators );
		return res;
	}

	static  CStdBitsetUnsignedNumber  CreateFromBinaryStringPriorityLSB( const String& str, const String& valid_separators = StringConv::DEFAULT_VALID_SEPARATORS ) {
		CStdBitsetUnsignedNumber ubn;
		ubn.fromBinaryStringPriorityLSB( str, valid_separators );
		return ubn;
	}

	static  StringParsedData  CreateFromBinaryStringPriorityLSBStrict( const String& str, const  OperationForInvalidCharDetected operation_invalid_char_detected, const String& valid_separators = StringConv::DEFAULT_VALID_SEPARATORS ) {
		StringParsedData res;
		res.info = res.value.fromBinaryStringPriorityLSBStrict( str, operation_invalid_char_detected, valid_separators );
		return res;
	}



	static CStdBitsetUnsignedNumber  CreateFromDecimalString( const String& str, const String& valid_separators = StringConv::DEFAULT_VALID_SEPARATORS ) {
		CStdBitsetUnsignedNumber ubn;
		ubn.fromDecimalString( str, valid_separators );
		return ubn;
	}

	static  StringParsedData  CreateFromDecimalStringStrict( const String& str, const  OperationForInvalidCharDetected operation_invalid_char_detected, const String& valid_separators = StringConv::DEFAULT_VALID_SEPARATORS ) {
		StringParsedData res;
		res.info = res.value.fromDecimalStringStrict( str, operation_invalid_char_detected, valid_separators );
		return res;
	}


	static CStdBitsetUnsignedNumber  CreateFromHexadecimalString( const String& str, const String& valid_separators = StringConv::DEFAULT_VALID_SEPARATORS ) {
		CStdBitsetUnsignedNumber ubn;
		ubn.fromHexadecimalString( str, valid_separators );
		return ubn;
	}

	static  StringParsedData  CreateFromHexadecimalStringStrict( const String& str, const  OperationForInvalidCharDetected operation_invalid_char_detected, const String& valid_separators = StringConv::DEFAULT_VALID_SEPARATORS ) {
		StringParsedData res;
		res.info = res.value.fromHexadecimalStringStrict( str, operation_invalid_char_detected, valid_separators );
		return res;
	}

	static CStdBitsetUnsignedNumber  CreateFromHexadecimalStringPriorityLSB( const String& str, const String& valid_separators = StringConv::DEFAULT_VALID_SEPARATORS ) {
		CStdBitsetUnsignedNumber ubn;
		ubn.fromHexadecimalStringPriorityLSB( str, valid_separators );
		return ubn;
	}

	static  StringParsedData  CreateFromHexadecimalStringPriorityLSBStrict( const String& str, const  OperationForInvalidCharDetected operation_invalid_char_detected, const String& valid_separators = StringConv::DEFAULT_VALID_SEPARATORS ) {
		StringParsedData res;
		res.info = res.value.fromHexadecimalStringPriorityLSBStrict( str, operation_invalid_char_detected, valid_separators );
		return res;
	}


};


template<size_t BitSize> using CStdBitsetUnsignedNumberA = CStdBitsetUnsignedNumber<BitSize, char>;
template<size_t BitSize> using CStdBitsetUnsignedNumberW = CStdBitsetUnsignedNumber<BitSize, wchar_t>;
