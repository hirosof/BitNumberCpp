#pragma once
#include <algorithm>
#include <optional>

class CBitNumberSupport {

public:

	enum struct BitOffsetBasis {
		Least = 0,		// 最下位ビット基準
		Most,				// 最上位ビット基準
		LSB = Least,	// 最下位ビット基準 (Leastの別名)
		MSB = Most		// 最上位ビット基準 (Mostの別名)
	};

	enum struct ExtractedBitLocation {

		// 最下位ビットから始める
		LeastSignificant = 0,

		// オリジナル位置に配置する
		Original,

		// 最上位ビットから始める
		MostSignificant

	};

	enum struct CompareResult {

		// 左側の値が大きい
		LeftGreater = 0,

		// 左右の値が等しい
		Equal,

		// 右側の値が大きい
		RightGreater,

		// 自身の値が大きい
		SelfGreater = LeftGreater,

		// 対象の値が大きい
		TargetGreater = RightGreater

	};


	struct BitOffsetRange {
		size_t offset;
		size_t size;
		BitOffsetBasis basis;
		BitOffsetRange( ) : offset( 0 ), size( 0 ), basis( BitOffsetBasis::Least ) {}
		BitOffsetRange( size_t offset, size_t size, BitOffsetBasis basis = BitOffsetBasis::Least ) : offset( offset ), size( size ), basis( basis ) {}
	};

	struct BitOffsetTwoPointRange {
		size_t offset1;
		size_t offset2;
		BitOffsetBasis basis;
		BitOffsetTwoPointRange( ) : offset1( 0 ), offset2( 0 ), basis( BitOffsetBasis::Least ) {}
		BitOffsetTwoPointRange( size_t offset1, size_t offset2, BitOffsetBasis basis = BitOffsetBasis::Least ) : offset1( offset1 ), offset2( offset2 ), basis( basis ) {}
	};


	struct RealRangeForRandomIssue {
		size_t offset_of_least;
		size_t fill_bit_size;
		RealRangeForRandomIssue( ) : offset_of_least( 0 ), fill_bit_size( 0 ) {}
	};

	template<typename ValueType> struct RandomIssueResult {
		ValueType value;
		RealRangeForRandomIssue realRange;
		RandomIssueResult( ) : value(), realRange( ) {}
	};

	/*
		インデックス検証・クリップ関数
	*/


	static bool IsValidIndex( size_t bit_size, size_t index ) {
		return ( index < bit_size );
	}

	template<size_t BitSize> static bool IsValidIndex( size_t index ) {
		return IsValidIndex( BitSize, index );
	}


	static size_t ClipIndex( size_t bit_size, size_t index ) {
		return std::min( index, bit_size - 1 );
	}

	template<size_t BitSize> static size_t ClipIndex( size_t index ) {
		return ClipIndex( BitSize, index );
	}


	static bool IsValidTwoPointRange( size_t bit_size, size_t index1, size_t index2 ) {
		return IsValidIndex( bit_size, index1 ) && IsValidIndex( bit_size, index2 );
	}

	template <size_t BitSize> static bool IsValidTwoPointRange( size_t index1, size_t index2 ) {
		return IsValidTwoPointRange( BitSize, index1, index2 );
	}

	static bool IsValidRange( size_t bit_size, size_t index, size_t size  ) {

		if ( bit_size == 0 ) return false;
		if ( size == 0 ) return false;

		if ( index >= bit_size ) return false;
		if ( size > bit_size ) return false;
		return ( index + size <= bit_size );
	}

	template <size_t BitSize> static bool IsValidRange( size_t index, size_t size ) {
		return IsValidRange( BitSize, index, size );
	}


	static std::optional<BitOffsetRange> AdjustOffsetRange( size_t bit_size, const BitOffsetRange& range ) {
		return AdjustOffsetRange( bit_size, range.offset, range.size, range.basis );
	}
	static std::optional<BitOffsetRange> AdjustOffsetRange( size_t bit_size, size_t offset, size_t size , BitOffsetBasis basis = BitOffsetBasis::Least ) {

		if ( offset >= bit_size ) return std::nullopt;
		if ( size == 0 ) { 
			if ( basis == BitOffsetBasis::Least ) {
				return BitOffsetRange( offset, 0, BitOffsetBasis::Least );
			} else {
				return BitOffsetRange( bit_size - 1 - offset, 0, BitOffsetBasis::Least );
			}
		}

		BitOffsetRange range( offset, size , BitOffsetBasis::Least );

		if ( (range.size - 1 ) >= (bit_size - range.offset) ) {
			range.size = bit_size - range.offset;
		}

		if ( basis == BitOffsetBasis::Least ) return range;

		range.offset = bit_size - range.offset - range.size;

		return range;
	}



	template <size_t BitSize> static std::optional<BitOffsetRange> AdjustOffsetRange( const BitOffsetRange& range ) {
		return AdjustOffsetRange( BitSize, range );
	}

	template <size_t BitSize> static std::optional<BitOffsetRange> AdjustOffsetRange( size_t offset, size_t size, BitOffsetBasis basis = BitOffsetBasis::Least ) {
		return AdjustOffsetRange( BitSize, offset, size, basis );
	}


	static BitOffsetTwoPointRange ClipOffsetTwoPointRange( size_t bit_size, const BitOffsetTwoPointRange& range ) {
		return ClipOffsetTwoPointRange( bit_size, range.offset1, range.offset2, range.basis );
	}

	static BitOffsetTwoPointRange ClipOffsetTwoPointRange( size_t bit_size, size_t offset1, size_t offset2, BitOffsetBasis basis = BitOffsetBasis::Least ) {
		size_t n1 = ClipIndex( bit_size, offset1 );
		size_t n2 = ClipIndex( bit_size, offset2 );
		if ( n1 <= n2 ) {
			return BitOffsetTwoPointRange( n1, n2, basis );
		} else {
			return BitOffsetTwoPointRange( n2, n1, basis );
		}
	}

	static BitOffsetTwoPointRange AdjustOffsetTwoPointRange( size_t bit_size, const BitOffsetTwoPointRange& range ) {
		return AdjustOffsetTwoPointRange( bit_size, range.offset1, range.offset2, range.basis );
	}

	static BitOffsetTwoPointRange AdjustOffsetTwoPointRange( size_t bit_size, size_t offset1, size_t offset2, BitOffsetBasis basis = BitOffsetBasis::Least ) {
		BitOffsetTwoPointRange range = ClipOffsetTwoPointRange( bit_size, offset1, offset2, BitOffsetBasis::Least );
		if ( basis == BitOffsetBasis::Most ) {
			return BitOffsetTwoPointRange( bit_size - 1 - range.offset2, bit_size - 1 - range.offset1, BitOffsetBasis::Least );
		} 
		return range;
	}

	static std::optional<BitOffsetTwoPointRange>OffsetRangeToTwoPointRange(const BitOffsetRange& range ) {

		BitOffsetTwoPointRange result;

		result.basis = range.basis;

		if ( range.size == 0 ) {
			return std::nullopt;
		} else {
			result.offset1 = range.offset;
			result.offset2 = range.offset + range.size - 1;
		}

		return result;
	}

	static BitOffsetRange TwoPointRangeToOffsetRange( const BitOffsetTwoPointRange& range ) {
		BitOffsetRange result;
		result.basis = range.basis;
		if ( range.offset2 < range.offset1 ) {
			result.offset = range.offset2;
			result.size = range.offset1 - range.offset2 + 1;
		} else {
			result.offset = range.offset1;
			result.size = range.offset2 - range.offset1 + 1;
		}
		return result;
	}

};