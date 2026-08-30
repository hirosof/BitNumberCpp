/**
	@file	  CBitNumberSupport.hpp
	@brief     ビット操作に関するサポート機能を提供するクラスが実装されているヘッダーファイル
	@author    hirosof
	@copyright (C) 2026 hirosof.
**/
#pragma once
#include <algorithm>
#include <optional>


/**
	@brief   ビット操作に関するサポート機能を提供するクラス
**/
class CBitNumberSupport {

public:

	/**
		@brief  ビットオフセットの基準を示す列挙型
	**/
	enum struct BitOffsetBasis {
		/// 最下位ビット基準
		Least = 0,

		/// 最上位ビット基準
		Most,

		/// 最下位ビット基準 (Leastの別名)
		LSB = Least,

		/// 最上位ビット基準 (Mostの別名)
		MSB = Most
	};


	/**
		@brief  抽出されたビットの抽出先位置を示す列挙型
	**/
	enum struct ExtractedBitLocation {

		/// 最下位ビットから始める
		LeastSignificant = 0,

		/// オリジナル位置に配置する
		Original,

		/// 最上位ビットから始める
		MostSignificant

	};


	/**
		@brief  比較結果を示す列挙型
	**/
	enum struct CompareResult {

		/// 左側の値が大きい
		LeftGreater = 0,

		/// 値が等しい
		Equal,

		/// 右側の値が大きい
		RightGreater,

		/// 自身の値が大きい (LeftGreaterの別名です。用途に合わせて使い分けてください。)
		SelfGreater = LeftGreater,

		/// 対象の値が大きい (RightGreaterの別名です。用途に合わせて使い分けてください。)
		TargetGreater = RightGreater

	};


	/**
		@brief  ビットオフセット範囲を表す構造体
	**/
	struct BitOffsetRange {

		/// ビットオフセットの開始位置
		size_t offset;

		/// ビットオフセットのサイズ
		size_t size;

		/// ビットオフセットの基準
		BitOffsetBasis basis;

		/**
			@brief BitOffsetRange のデフォルトコンストラクタ
			@details offset は 0、size は 0、basis は BitOffsetBasis::Least に初期化されます。
		**/
		BitOffsetRange( ) : offset( 0 ), size( 0 ), basis( BitOffsetBasis::Least ) {}

		/**
			@brief BitOffsetRange のコンストラクタ
			@param offset - ビットオフセットの開始位置
			@param size   - ビットオフセットのサイズ
			@param basis  - ビットオフセットの基準
			@details 全てのメンバ変数は指定された値で初期化されます。basis は省略可能で、デフォルトでは BitOffsetBasis::Least が使用されます。
			@remark 指定された値の有効性はこのコンストラクタ内ではチェックされません。必要に応じて、外部で有効性を確認してください。
		**/
		BitOffsetRange( size_t offset, size_t size, BitOffsetBasis basis = BitOffsetBasis::Least ) : offset( offset ), size( size ), basis( basis ) {}

	};


	/**
		@brief  2点間のビットオフセット範囲を表す構造体
	**/
	struct BitOffsetTwoPointRange {

		/// ビットオフセットの開始位置1
		size_t offset1;

		/// ビットオフセットの開始位置2
		size_t offset2;

		/// ビットオフセットの基準
		BitOffsetBasis basis;

		/**
			@brief BitOffsetTwoPointRange のデフォルトコンストラクタ
			@details offset1 は 0、offset2 は 0、basis は BitOffsetBasis::Least に初期化されます。
		**/
		BitOffsetTwoPointRange( ) : offset1( 0 ), offset2( 0 ), basis( BitOffsetBasis::Least ) {}

		/**
			@brief BitOffsetTwoPointRange のコンストラクタ
			@param offset1 - ビットオフセットの開始位置1
			@param offset2 - ビットオフセットの開始位置2
			@param basis   - ビットオフセットの基準
			@details 全てのメンバ変数は指定された値で初期化されます。basis は省略可能で、デフォルトでは BitOffsetBasis::Least が使用されます。
			@remark 指定された値の有効性はこのコンストラクタ内ではチェックされません。必要に応じて、外部で有効性を確認してください。
		**/
		BitOffsetTwoPointRange( size_t offset1, size_t offset2, BitOffsetBasis basis = BitOffsetBasis::Least ) : offset1( offset1 ), offset2( offset2 ), basis( basis ) {}
	};


	/**
		@brief  ランダム発行時の実際のビット範囲を表す構造体
	**/
	struct RealRangeForRandomIssue {
		/// 最下位ビットからのオフセット
		size_t offset_of_least;

		/// ランダム生成したビットサイズ
		size_t fill_bit_size;

		/**
			@brief RealRangeForRandomIssue のコンストラクタ
			@details offset_of_least は 0、fill_bit_size は 0 に初期化されます。
		**/
		RealRangeForRandomIssue( ) : offset_of_least( 0 ), fill_bit_size( 0 ) {}
	};

	/**
		@brief  ランダム発行結果を表す構造体
		@tparam ValueType ランダム発行結果の値の型
	**/
	template<typename ValueType> struct RandomIssueResult {

		/// ランダム発行結果の値
		ValueType value;

		/// ランダム発行時の実際のビット範囲
		RealRangeForRandomIssue realRange;

		/**
			@brief RandomIssueResult のコンストラクタ
			@details value は ValueType のデフォルト値、realRange は RealRangeForRandomIssue のデフォルト値に初期化されます。
			**/
		RandomIssueResult( ) : value( ), realRange( ) {}
	};


	/**
		@brief  インデックスが有効かどうかを判定する
		@param  bit_size - ビットサイズ
		@param  index    - インデックス
		@return   有効な場合はtrue、無効な場合はfalse
		@remark  bit_size が0の場合、常にfalseを返却します。
	**/
	static bool IsValidIndex( size_t bit_size, size_t index ) {
		if ( bit_size == 0 ) return false;
		return ( index < bit_size );
	}

	/**
		@brief  インデックスが有効かどうかを判定する
		@tparam BitSize -  ビットサイズ
		@param  index   - インデックス
		@return         有効な場合はtrue、無効な場合はfalse
		@remark  BitSize が0の場合、static_assertによりコンパイルエラーとなります。
	**/
	template<size_t BitSize> static bool IsValidIndex( size_t index ) {
		static_assert( BitSize > 0, "BitSizeの値が無効です。" );
		return IsValidIndex( BitSize, index );
	}


	/**
		@brief  インデックスをクリップする
		@param  bit_size - ビットサイズ
		@param  index   - インデックス
		@return  クリップされたインデックス
		@remark  bit_size が0の場合、常に0を返却します。
	**/
	static size_t ClipIndex( size_t bit_size, size_t index ) {
		if ( bit_size == 0 ) return 0;
		return std::min( index, bit_size - 1 );
	}

	/**
		@brief  インデックスをクリップする
		@tparam BitSize - ビットサイズ
		@param  index   - インデックス
		@return         クリップされたインデックス
		@remark  BitSize が0の場合、static_assertによりコンパイルエラーとなります。
	**/
	template<size_t BitSize> static size_t ClipIndex( size_t index ) {
		static_assert( BitSize > 0, "BitSizeの値が無効です。" );
		return ClipIndex( BitSize, index );
	}


	/**
		@brief  2点間のインデックス範囲が有効かどうかを判定する
		@param  bit_size - ビットサイズ
		@param  index1   - インデックス1
		@param  index2   - インデックス2
		@return  両方のインデックスが有効な場合はtrue、それ以外はfalse
		@remark  bit_size が0の場合、常にfalseを返却します。
	**/
	static bool IsValidTwoPointRange( size_t bit_size, size_t index1, size_t index2 ) {
		return IsValidIndex( bit_size, index1 ) && IsValidIndex( bit_size, index2 );
	}

	/**
		@brief  2点間のインデックス範囲が有効かどうかを判定する
		@tparam BitSize - ビットサイズ
		@param  index1  - インデックス1
		@param  index2  - インデックス2
		@return  両方のインデックスが有効な場合はtrue、それ以外はfalse
		@remark  BitSize が0の場合、static_assertによりコンパイルエラーとなります。
	**/
	template <size_t BitSize> static bool IsValidTwoPointRange( size_t index1, size_t index2 ) {
		static_assert( BitSize > 0, "BitSizeの値が無効です。" );
		return IsValidTwoPointRange( BitSize, index1, index2 );
	}

	/**
		@brief  インデックス範囲が有効かどうかを判定する
		@param  bit_size - ビットサイズ
		@param  index   - インデックス
		@param  size     - サイズ
		@return				 有効な場合はtrue、無効な場合はfalse
		@remark  bit_size が0の場合、常にfalseを返却します。
	**/
	static bool IsValidRange( size_t bit_size, size_t index, size_t size ) {

		if ( bit_size == 0 ) return false;
		if ( size == 0 ) return false;

		if ( index >= bit_size ) return false;
		if ( size > bit_size ) return false;
		return ( index + size <= bit_size );
	}

	/**
		@brief  インデックス範囲が有効かどうかを判定する
		@tparam BitSize - ビットサイズ
		@param  index   - インデックス
		@param  size    - サイズ
		@return         有効な場合はtrue、無効な場合はfalse
		@remark  BitSize が0の場合、static_assertによりコンパイルエラーとなります。
	**/
	template <size_t BitSize> static bool IsValidRange( size_t index, size_t size ) {
		static_assert( BitSize > 0, "BitSizeの値が無効です。" );
		return IsValidRange( BitSize, index, size );
	}

	/**
		@brief  オフセット範囲を調整する
		@param  bit_size - ビットサイズ
		@param  range    - オフセット範囲
		@return	bit_size のサイズを超えないように range.size が 調整された BitOffsetBasis::Least 基準のオフセット範囲、指定されたオフセット範囲が無効な場合は std::nullopt
		@remark range.basis に指定された値が BitOffsetBasis::Most の場合、この関数は、 BitOffsetBasis::Least 基準に変換された値で返却します。
	**/
	static std::optional<BitOffsetRange> AdjustOffsetRange( size_t bit_size, const BitOffsetRange& range ) {
		return AdjustOffsetRange( bit_size, range.offset, range.size, range.basis );
	}

	/**
		@brief  オフセット範囲を調整する
		@param  bit_size - ビットサイズ
		@param  offset   - オフセット
		@param  size     - サイズ
		@param  basis    - 基準
		@return	bit_size のサイズを超えないように size が 調整された BitOffsetBasis::Least 基準のオフセット範囲、指定されたオフセット範囲が無効な場合は std::nullopt
		@remark basis に指定された値が BitOffsetBasis::Most の場合、この関数は、 BitOffsetBasis::Least 基準に変換された値で返却します。
	**/
	static std::optional<BitOffsetRange> AdjustOffsetRange( size_t bit_size, size_t offset, size_t size, BitOffsetBasis basis = BitOffsetBasis::Least ) {
		if ( bit_size == 0 ) return std::nullopt;
		if ( offset >= bit_size ) return std::nullopt;
		if ( size == 0 ) {
			if ( basis == BitOffsetBasis::Least ) {
				return BitOffsetRange( offset, 0, BitOffsetBasis::Least );
			} else {
				return BitOffsetRange( bit_size - 1 - offset, 0, BitOffsetBasis::Least );
			}
		}

		BitOffsetRange range( offset, size, BitOffsetBasis::Least );

		if ( ( range.size - 1 ) >= ( bit_size - range.offset ) ) {
			range.size = bit_size - range.offset;
		}

		if ( basis == BitOffsetBasis::Least ) return range;

		range.offset = bit_size - range.offset - range.size;

		return range;
	}


	/**
		@brief  オフセット範囲を調整する
		@tparam BitSize - ビットサイズ
		@param  range   - オフセット範囲
		@return	BitSize のサイズを超えないように range.size が 調整された BitOffsetBasis::Least 基準のオフセット範囲、指定されたオフセット範囲が無効な場合は std::nullopt
		@remark range.basis に指定された値が BitOffsetBasis::Most の場合、この関数は、 BitOffsetBasis::Least 基準に変換された値で返却します。<br>
						なお、 BitSize が0の場合、static_assertによりコンパイルエラーとなります。
	**/
	template <size_t BitSize> static std::optional<BitOffsetRange> AdjustOffsetRange( const BitOffsetRange& range ) {
		static_assert( BitSize > 0, "BitSizeの値が無効です。" );
		return AdjustOffsetRange( BitSize, range );
	}

	/**
		@brief  オフセット範囲を調整する
		@tparam BitSize - ビットサイズ
		@param  offset  - オフセット
		@param  size    - サイズ
		@param  basis   - 基準
		@return BitSize のサイズを超えないように size が 調整された BitOffsetBasis::Least 基準のオフセット範囲、指定されたオフセット範囲が無効な場合は std::nullopt
		@remark basis に指定された値が BitOffsetBasis::Most の場合、この関数は、 BitOffsetBasis::Least 基準に変換された値で返却します。<br>
						なお、 BitSize が0の場合、static_assertによりコンパイルエラーとなります。
	**/
	template <size_t BitSize> static std::optional<BitOffsetRange> AdjustOffsetRange( size_t offset, size_t size, BitOffsetBasis basis = BitOffsetBasis::Least ) {
		static_assert( BitSize > 0, "BitSizeの値が無効です。" );
		return AdjustOffsetRange( BitSize, offset, size, basis );
	}

	/**
		@brief  2点オフセット範囲をクリップする
		@param  bit_size  - ビットサイズ
		@param  range    - オフセット範囲
		@return			クリップ後のオフセット範囲
		@details クリップ後の大小関係は BitOffsetTwoPointRange::offset1 <= BitOffsetTwoPointRange::offset2 となるように格納されます。<br>
							なお、basisの値は入力値のまま格納されます。
		@remark  bit_size が0の場合、常に BitOffsetTwoPointRange( 0, 0, range.basis ) を返却します。
	**/
	static BitOffsetTwoPointRange ClipOffsetTwoPointRange( size_t bit_size, const BitOffsetTwoPointRange& range ) {
		return ClipOffsetTwoPointRange( bit_size, range.offset1, range.offset2, range.basis );
	}


	/**
		@brief  2点オフセット範囲をクリップする
		@param  bit_size - ビットサイズ
		@param  offset1  - オフセット1
		@param  offset2  - オフセット2
		@param  basis    - 基準
		@return  クリップ後のオフセット範囲
		@details		クリップ後の大小関係は BitOffsetTwoPointRange::offset1 <= BitOffsetTwoPointRange::offset2 となるように格納されます。<br>
							なお、basisの値は入力値のまま格納されます。
		@remark  bit_size が0の場合、常に BitOffsetTwoPointRange( 0, 0, basis ) を返却します。
	**/
	static BitOffsetTwoPointRange ClipOffsetTwoPointRange( size_t bit_size, size_t offset1, size_t offset2, BitOffsetBasis basis = BitOffsetBasis::Least ) {

		if ( bit_size == 0 ) return BitOffsetTwoPointRange( 0, 0, basis );

		size_t n1 = ClipIndex( bit_size, offset1 );
		size_t n2 = ClipIndex( bit_size, offset2 );
		if ( n1 <= n2 ) {
			return BitOffsetTwoPointRange( n1, n2, basis );
		} else {
			return BitOffsetTwoPointRange( n2, n1, basis );
		}
	}

	/**
		@brief  2点オフセット範囲を調整する
		@param  bit_size - ビットサイズ
		@param  range    - オフセット範囲
		@return   bit_size を基準に オーバーフローしないように調整された BitOffsetBasis::Least 基準の2点オフセット範囲
		@details  調整後の大小関係は BitOffsetTwoPointRange::offset1 <= BitOffsetTwoPointRange::offset2 となるように格納されます。<br>
						 また、 range.basis が BitOffsetBasis::Most の場合、 BitOffsetBasis::Least 基準に変換された値で返却されます。
		@remark  bit_size が0の場合、常に BitOffsetTwoPointRange( 0, 0, BitOffsetBasis::Least ) を返却します。
	**/
	static BitOffsetTwoPointRange AdjustOffsetTwoPointRange( size_t bit_size, const BitOffsetTwoPointRange& range ) {
		return AdjustOffsetTwoPointRange( bit_size, range.offset1, range.offset2, range.basis );
	}

	/**
		@brief  2点オフセット範囲を調整する
		@param  bit_size - ビットサイズ
		@param  offset1  - オフセット1
		@param  offset2  - オフセット2
		@param  basis    - 基準
		@return   bit_size を基準に オーバーフローしないように調整された BitOffsetBasis::Least 基準の2点オフセット範囲
		@details 調整後の大小関係は BitOffsetTwoPointRange::offset1 <= BitOffsetTwoPointRange::offset2 となるように格納されます。<br>
						 また、 basis が BitOffsetBasis::Most の場合、 BitOffsetBasis::Least 基準に変換された値で返却されます。
		@remark  bit_size が0の場合、常に BitOffsetTwoPointRange( 0, 0, BitOffsetBasis::Least ) を返却します。
	**/
	static BitOffsetTwoPointRange AdjustOffsetTwoPointRange( size_t bit_size, size_t offset1, size_t offset2, BitOffsetBasis basis = BitOffsetBasis::Least ) {
		if ( bit_size == 0 ) return BitOffsetTwoPointRange( 0, 0, BitOffsetBasis::Least );
		BitOffsetTwoPointRange range = ClipOffsetTwoPointRange( bit_size, offset1, offset2, BitOffsetBasis::Least );
		if ( basis == BitOffsetBasis::Most ) {
			return BitOffsetTwoPointRange( bit_size - 1 - range.offset2, bit_size - 1 - range.offset1, BitOffsetBasis::Least );
		}
		return range;
	}

	/**
	 * @brief  オフセット範囲を2点範囲に変換する
	 * @param range - オフセット範囲
	 * @return      2点範囲、無効な場合は std::nullopt
	 */
	static std::optional<BitOffsetTwoPointRange>OffsetRangeToTwoPointRange( const BitOffsetRange& range ) {

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

	/**
		@brief  2点範囲をオフセット範囲に変換する
		@param  range - 2点範囲
		@return       オフセット範囲
	**/
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