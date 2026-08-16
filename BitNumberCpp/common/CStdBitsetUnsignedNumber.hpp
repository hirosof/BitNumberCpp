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

	bool clear( size_t start_index = 0  , OffsetBasis basis = OffsetBasis::Least ) {
		if(!CBitNumberSupport::IsValidIndex<BitSize>(start_index ) ) return false;

		size_t least_based_start_index = ( basis == OffsetBasis::Least ) ? start_index : ( BitSize - 1 - start_index );

		return rangeUnsetIndex( least_based_start_index, BitSize - 1, OffsetBasis::Least );
	}


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

	template<typename T> CStdBitsetUnsignedNumber extract( size_t offset ) {
		return extract<T>( offset, ExtractedBitLocation::LeastSignificant );
	}
	
	template<typename T> CStdBitsetUnsignedNumber extract( size_t offset, ExtractedBitLocation extracted_bit_location = ExtractedBitLocation::LeastSignificant ) const {
		return extract<T>( offset, sizeof( T ) * 8, OffsetBasis::Least, extracted_bit_location );
	}
	template<typename T> CStdBitsetUnsignedNumber extract( size_t offset, OffsetBasis basis = OffsetBasis::Least , ExtractedBitLocation extracted_bit_location = ExtractedBitLocation::LeastSignificant ) const {
		return extract( offset, sizeof( T ) * 8, basis, extracted_bit_location );
	}

	CStdBitsetUnsignedNumber extract( size_t offset, size_t size, OffsetBasis basis = OffsetBasis::Least, ExtractedBitLocation extracted_bit_location = ExtractedBitLocation::LeastSignificant ) const {

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

	CStdBitsetUnsignedNumber extractIndex( size_t index1, size_t index2, OffsetBasis basis = OffsetBasis::Least ,ExtractedBitLocation extracted_bit_location = ExtractedBitLocation::LeastSignificant ) const {


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

		CStdBitsetUnsignedNumber extracted_value = extract<UIntTypeName>( offset_bit_number, basis, ExtractedBitLocation::LeastSignificant );

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
		this->m_raw = CStdBitsetUnsignedOperation::CastSize<BitSize>( from.extract( from_offset_bit_number , BitSize , basis).getStdBitsetRefConst() );
		if ( basis == OffsetBasis::Least ) {
			if ( self_offset_bit_number > 0 ) this->m_raw <<= self_offset_bit_number;
		} else {
			this->m_raw <<= ( BitSize - self_offset_bit_number - std::min( BitSize, FromSize ) );
		}
	}

	template <size_t FromSize> void fromSizeCast( const  CStdBitsetUnsignedNumber<FromSize, CharType>& from, size_t self_offset_bit_number = 0, size_t from_offset_bit_number = 0 , OffsetBasis basis = OffsetBasis::Least ) {
		return fromCast<FromSize, CharType>( from, self_offset_bit_number, from_offset_bit_number, basis );
	}

	template <typename FromCharType> void fromCharCast( const  CStdBitsetUnsignedNumber<BitSize, FromCharType>& from, size_t self_offset_bit_number = 0, size_t from_offset_bit_number = 0 , OffsetBasis basis = OffsetBasis::Least ) {
		return fromCast<BitSize, FromCharType>( from, self_offset_bit_number, from_offset_bit_number, basis );
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

	size_t getNumberOfActualBits( ) const {
		return CStdBitsetUnsignedOperation::GetNumberOfDigitsForDisplay( m_raw );
	}

};