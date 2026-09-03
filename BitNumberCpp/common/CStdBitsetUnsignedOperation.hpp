/**

	@file      CStdBitsetUnsignedOperation.hpp
	@brief     標準ビットセット(std::bitset)を用いた符号なし整数の演算操作クラスが実装されているヘッダーファイル
	@author    hirosof
	@copyright (C) 2026 hirosof.
**/

#pragma once
#include <bitset>
#include <optional>
#include <algorithm>
#include <utility>
#include <random>
#include <cstdint>
#include "CBitNumberSupport.hpp"


/**

	@class   CStdBitsetUnsignedOperation
	@brief   標準ビットセット(std::bitset)を用いた符号なし整数の演算操作クラス
	@details
		このクラスは、標準ビットセット(std::bitset)を用いた符号なし整数の演算操作を提供します。<br>
		加算、減算、乗算、除算などの基本的な算術演算に加え、ビット操作もサポートしています。<br>
		また、ビットサイズの変換や比較操作も提供されており、柔軟なビット演算が可能です。<br>
		<br>
		なお、一部は過去に以下にまとめた内容をベースに実装したものです。<br>
		https://gist.github.com/hirosof/2dad279fc120d476a7079506cfab2572
**/
class CStdBitsetUnsignedOperation {
public:

	/**
	 * @brief 標準ビットセット(std::bitset)のエイリアス
	 * @tparam BitSize ビットセットのサイズ
	 */
	template<size_t BitSize> using StdBitset = std::bitset<BitSize>;

	/**
	 * @brief 標準ビットセット(std::bitset)のポインタのエイリアス
	 * @tparam BitSize ビットセットのサイズ
	 */
	template<size_t BitSize> using StdBitsetPointer = StdBitset<BitSize>*;
	
	/**
	 * @brief 標準ビットセット(std::bitset)のconstエイリアス
	 * @tparam BitSize ビットセットのサイズ
	 */
	template<size_t BitSize> using StdBitsetConst = const StdBitset<BitSize>;
	
	/**
	 * @brief 標準ビットセット(std::bitset)のconst参照のエイリアス
	 * @tparam BitSize ビットセットのサイズ
	 */
	template<size_t BitSize> using StdBitsetConstRef = const StdBitset<BitSize>&;

	/**
	 * @brief 標準ビットセット(std::bitset)のペアのエイリアス
	 * @tparam BitSize ビットセットのサイズ
	 */
	template <size_t BitSize> using StdBitsetPair = std::pair<StdBitset<BitSize>, StdBitset<BitSize>>;
	
	/**
	 * @brief 標準ビットセット(std::bitset)のオプショナル型のエイリアス
	 * @tparam BitSize ビットセットのサイズ
	 */
	template <size_t BitSize> using OptionalStdBitset = std::optional<StdBitset<BitSize>>;
	
	/**
	 * @brief 標準ビットセット(std::bitset)のペアのオプショナル型のエイリアス
	 * @tparam BitSize ビットセットのサイズ
	 */
	template <size_t BitSize> using OptionalStdBitsetPair = std::optional<StdBitsetPair<BitSize>>;

	/**
	 * @brief 比較結果の型のエイリアス
	 * @see CBitNumberSupport::CompareResult
	 */
	using CompareResult = CBitNumberSupport::CompareResult;

	/**
		@brief  StdBitsetのビットサイズを変換する
		@tparam ToBitSize -  変換先のビットサイズ
		@tparam FromBitSize - 変換元のビットサイズ
		@param  input - 変換元のStdBitset
		@pre ToBitSize > 0 かつ FromBitSize > 0 (満たさない場合はstatic_assertによりコンパイルエラーとなります)
		@return             変換先のStdBitset
		@details 以下のルールにて変換を行います。
		   |条件|変換方法|
		   |---|---|
		   |ToBitSize > FromBitSize|変換元のビットセットの値をそのまま変換先のビットセットにコピーし、残りのビットは0で埋めます。|
		   |ToBitSize = FromBitSize|変換元のビットセットの値をそのまま変換先のビットセットにコピーします。|
		   |ToBitSize < FromBitSize|変換元のビットセットの下位ToBitSizeビットを変換先のビットセットにコピーします。(変換先に入り切らない上位側のビットは切り捨てられます)|
		@warning ToBitSize < FromBitSize の場合、変換元のビットセットの上位側のビットは切り捨てられるため、情報が失われる可能性があります。
	**/
	template< size_t ToBitSize, size_t FromBitSize>  static StdBitset<ToBitSize> CastSize( StdBitsetConstRef<FromBitSize> input ) {
		static_assert( FromBitSize > 0, "FromBitSizeは無効な値です。" );
		static_assert( ToBitSize > 0, "ToBitSizeは無効な値です。" );

		if constexpr ( FromBitSize == ToBitSize ) {
			return input;
		}

		StdBitset<ToBitSize> target;

		size_t copySize = std::min( FromBitSize, ToBitSize );
		for ( size_t i = 0; i < copySize; i++ ) {
			target[i] = input[i];
		}

		return target;
	}


	/**
		@brief  ビットセットの値をインクリメントする
		@tparam BitSize - ビットセットのサイズ
		@param  input   - インクリメントするビットセット
		@pre BitSize > 0 (満たさない場合はstatic_assertによりコンパイルエラーとなります)
		@return         - インクリメント後のビットセット
	**/
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

	/**
		@brief  ビットセットの値をデクリメントする
		@tparam BitSize - ビットセットのサイズ
		@param  input   - デクリメントするビットセット
		@pre BitSize > 0 (満たさない場合はstatic_assertによりコンパイルエラーとなります)
		@return         - デクリメント後のビットセット
	**/
	template<size_t BitSize>  static StdBitset<BitSize> Decrement( StdBitsetConstRef<BitSize> input ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );


		// このクラスは、1減算するのではなく、全ビットが1である値を加算する方法でデクリメントを実装している
		// 全ビットが1である値を加算するということは、1の2の補数(-1) を加算することと同義となり、1減算を実装できる

		StdBitset<BitSize>  ox( input );

		bool A, X, Cn, Cb = false;

		// 前述した通り、全ビットが1である値を加算するため、全ビット分のループによる演算が必要となる
		for ( size_t i = 0; i < BitSize; i++ ) {

			A = ox[i];

			X = !( A ^ Cb );
			Cn = A || Cb;

			ox[i] = X;

			Cb = Cn;
		}

		return ox;
	}

	/**
		@brief  ビットセットの値を加算する
		@tparam BitSize     - ビットセットのサイズ
		@param  input_a     - 入力値A
		@param  input_b     - 入力値B
		@param  input_carry - キャリー入力
		@param  pLastCarry  - 最後のキャリー出力
		@pre BitSize > 0 (満たさない場合はstatic_assertによりコンパイルエラーとなります)
		@return             - input_a + input_b を算出した結果のビットセット
	**/
	template<size_t BitSize>  static StdBitset<BitSize> Addition( StdBitsetConstRef<BitSize> input_a, StdBitsetConstRef<BitSize> input_b, const bool input_carry = false, bool* const pLastCarry = nullptr ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		std::bitset<BitSize>  ox( 0 );

		bool a, b, x;
		bool c, Cb = input_carry;

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

	/**
		@brief  ビットセットの値を減算する
		@tparam BitSize - ビットセットのサイズ
		@param  input_a - 入力値A
		@param  input_b - 入力値B
		@pre BitSize > 0 (満たさない場合はstatic_assertによりコンパイルエラーとなります)
		@return         - input_a - input_b を算出した結果のビットセット
	**/
	template<size_t BitSize>  static StdBitset<BitSize> Subtraction( StdBitsetConstRef<BitSize> input_a, StdBitsetConstRef<BitSize> input_b ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		std::bitset<BitSize>  ox( 0 );

		bool a, b, x;
		bool c, Cb = false;

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

	/**
		@brief  ビットセットの値を乗算する
		@tparam BitSize - ビットセットのサイズ
		@param  input_a - 入力値A
		@param  input_b - 入力値B
		@pre BitSize > 0 (満たさない場合はstatic_assertによりコンパイルエラーとなります)
		@return         - input_a * input_b を算出した結果のビットセット
	**/
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

	/**
		@brief  ビットセットの値を10倍する
		@tparam BitSize - ビットセットのサイズ
		@param  input   - 入力値
		@pre BitSize > 0 (満たさない場合はstatic_assertによりコンパイルエラーとなります)
		@return         - input * 10 を算出した結果のビットセット
	**/
	template<size_t BitSize>  static StdBitset<BitSize> Multiplication10( StdBitsetConstRef<BitSize> input ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );

		// input == 0  の時、 input * 10 = 0
		if ( input.none( ) ) {
			return StdBitset<BitSize>( 0 );
		}

		// input * 10 =  input * 8 + input * 2 = (input << 3 ) + (input << 1)
		return Addition( input << 3, input << 1 );
	}

	/**
		@brief  ビットセットの値を除算する
		@tparam BitSize - ビットセットのサイズ
		@param  input_a - 入力値A
		@param  input_b - 入力値B
		@pre BitSize > 0 (満たさない場合はstatic_assertによりコンパイルエラーとなります)
		@return input_a / input_b を算出した結果のビットセット<br>
		ただし、input_b が 0 の場合(0除算の発生時)は std::nullopt を返却します。
		@remark この関数は、 #DivisionWithRemainder を内部で呼び出しており、商のみを返却する実装になっております。<br>
		そのため、商と余りの両方を取得したい場合は、 #DivisionWithRemainder を直接呼び出すことを推奨します。
		@remark 10で割った商を取得したい場合は、 #Division10 を使用することを推奨します。 (詳細は #Division10 のドキュメントを参照してください。)
	**/
	template<size_t BitSize>  static OptionalStdBitset<BitSize> Division( StdBitsetConstRef<BitSize> input_a, StdBitsetConstRef<BitSize> input_b ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		OptionalStdBitsetPair<BitSize> dwr = DivisionWithRemainder( input_a, input_b );

		if ( dwr.has_value( ) ) {
			return dwr->first;
		}

		return std::nullopt;
	}

	/**
		@brief  ビットセットの値を除算した余りを算出する
		@tparam BitSize - ビットセットのサイズ
		@param  input_a - 入力値A
		@param  input_b - 入力値B
		@pre BitSize > 0 (満たさない場合はstatic_assertによりコンパイルエラーとなります)
		@return input_a % input_b を算出した結果のビットセット<br>
		ただし、input_b が 0 の場合(0除算の発生時)は std::nullopt を返却します。
		@remark この関数は、 #DivisionWithRemainder を内部で呼び出しており、余りのみを返却する実装になっております。<br>
		そのため、商と余りの両方を取得したい場合は、 #DivisionWithRemainder を直接呼び出すことを推奨します。
		@remark 10で割った余りを取得したい場合は、 #Remainder10 を使用することを推奨します。 (詳細は #Remainder10 のドキュメントを参照してください。)
		**/
	template<size_t BitSize>  static OptionalStdBitset<BitSize> Remainder( StdBitsetConstRef<BitSize> input_a, StdBitsetConstRef<BitSize> input_b ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		OptionalStdBitsetPair<BitSize> dwr = DivisionWithRemainder( input_a, input_b );

		if ( dwr.has_value( ) ) {
			return dwr->second;
		}

		return std::nullopt;
	}

	/**
		@brief  ビットセットの値を除算した商と余りを算出する
		@tparam BitSize - ビットセットのサイズ
		@param  input_a - 入力値A
		@param  input_b - 入力値B
		@pre BitSize > 0 (満たさない場合はstatic_assertによりコンパイルエラーとなります)
		@return input_a / input_b と input_a % input_b を算出した結果のビットセットのペア (first が商、second が余り)<br>
		ただし、input_b が 0 の場合(0除算の発生時)は std::nullopt を返却します。
		@remark 10で割った商と余りを取得したい場合は、 #Division10WithRemainder を使用することを推奨します。 (詳細は #Division10WithRemainder のドキュメントを参照してください。)
	**/
	template<size_t BitSize>  static OptionalStdBitsetPair<BitSize> DivisionWithRemainder( StdBitsetConstRef<BitSize> input_a, StdBitsetConstRef<BitSize> input_b ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );

		// input_b == 0 の時 (input_b においてtrueのビットが1つもない時) は 0除算となり、
		// 計算結果は無いため std::nullopt を返す
		if ( input_b.none( ) ) return std::nullopt;


		CompareResult input_compared = Compare( input_a, input_b );

		if ( input_compared == CompareResult::Equal ) {
			// input_a == input_b の時、商：1、あまり：0
			return std::pair( StdBitset<BitSize>( 1 ), StdBitset<BitSize>( 0 ) );
		}

		if ( input_compared == CompareResult::RightGreater ) {
			// input_a < input_b の時、商：0、あまり：input_a
			return std::pair( StdBitset<BitSize>( 0 ), input_a );
		}

		const size_t digit_ia = GetSignificantBitLength( input_a );
		const size_t digit_ib = GetSignificantBitLength( input_b );
		const size_t digit_ox = digit_ia - digit_ib + 1;


		StdBitset<BitSize>  auxiliary( 0 );
		for ( size_t i = 0; i < digit_ib; i++ ) {
			auxiliary[digit_ib - 1 - i] = input_a[digit_ia - 1 - i];
		}

		StdBitset<BitSize>  ox( 0 );

		for ( size_t i = 0; i < digit_ox; i++ ) {

			ox[digit_ox - 1 - i] = ( Compare( auxiliary, input_b ) != CompareResult::RightGreater );

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

	/**
		@brief  ビットセットの値を10で除算した商を算出する
		@tparam BitSize - ビットセットのサイズ
		@param  input   - 入力値
		@pre BitSize > 0 (満たさない場合はstatic_assertによりコンパイルエラーとなります)
		@return         - input / 10 を算出した結果のビットセット
		@remark この関数は、 #Division10WithRemainder を内部で呼び出しており、商のみを返却する実装になっております。<br>
		そのため、商と余りの両方を取得したい場合は、 #Division10WithRemainder を直接呼び出すことを推奨します。
		@remark 10で除算した商を算出する場合、 #Division を使用するよりも、 #Division10 を使用する方が高速に計算できます。
	**/
	template<size_t BitSize>  static StdBitset<BitSize> Division10( StdBitsetConstRef<BitSize> input ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		return Division10WithRemainder( input ).first;
	}

	/**
		@brief  ビットセットの値を10で除算した余りを算出する
		@tparam BitSize - ビットセットのサイズ
		@param  input   - 入力値
		@pre BitSize > 0 (満たさない場合はstatic_assertによりコンパイルエラーとなります)
		@return         - input % 10 を算出した結果のビットセット
		@remark  この関数は、 #Division10WithRemainder を内部で呼び出しており、余りのみを返却する実装になっております。<br>
		そのため、商と余りの両方を取得したい場合は、 #Division10WithRemainder を直接呼び出すことを推奨します。
		@remark 10で除算した余りを算出する場合、 #Remainder を使用するよりも、 #Remainder10 を使用する方が高速に計算できます。
	**/
	template<size_t BitSize>  static StdBitset<BitSize> Remainder10( StdBitsetConstRef<BitSize> input ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		return Division10WithRemainder( input ).second;
	}

	/**
		@brief  ビットセットの値を10で除算した商と余りを算出する
		@tparam BitSize - ビットセットのサイズ
		@param  input   - 入力値
		@pre BitSize > 0 (満たさない場合はstatic_assertによりコンパイルエラーとなります)
		@return  input / 10 と input % 10 を算出した結果のビットセットのペア (first が商、second が余り)
		@note この関数では、10で割ることを前提しているため、ゼロ除算は発生しないため、 戻り値の型は #OptionalStdBitsetPair<BitSize> ではなく、 #StdBitsetPair<BitSize> となります。 (std::nullopt は返却されません)

		@remark 10で除算した商と余りを算出する場合、 #DivisionWithRemainder を使用するよりも、 #Division10WithRemainder を使用する方が高速に計算できます。<br>
		以下は、 最上位ビットは1固定でランダム生成した値を10で除算する計算を #DivisionWithRemainder と #Division10WithRemainder の双方で複数回計算するのにかかった時間の比較結果です。(※環境により異なる場合があります)<br>
		| 入力値のビット数  | 繰り返し呼び出し回数 | 所要時間 ( Division10WithRemainder ) | 所要時間 ( DivisionWithRemainder ) |
		|---------|-----------|-----------------------------------|---------------------------------|
		| 1024 | 512 | 00:00:00.002226 | 00:00:00.847324 |
		| 2048 | 512 | 00:00:00.004158 | 00:00:03.409136 |
		| 4096 | 512 | 00:00:00.008398 | 00:00:13.465817 |
		| 8192 | 512 | 00:00:00.016390 | 00:00:55.060684 |
		| 16384 | 512 | 00:00:00.034865 | 00:03:34.096148 |
		| 32768 | 512 | 00:00:00.068404 | 00:14:07.965573 |
		<br>
		計測環境情報<br>
		|項目名|値|備考|
		|---|---|---|
		|CPU|Intel(R) Core(TM) Ultra 7 265F @ 2.40GHz| |
		|OS|Windows 11 Pro 25H2 (x64)| |
		|搭載メモリ|64.0 GB| |
		|C++バージョン|C++20|Visual Studio 2026のデフォルト設定|
		|コンパイラ|LLVM (clang-cl)|Visual Studio 2026から使用|
		|ビルド種別|Release|  |
		|最適化オプション|/O2|最大最適化 (速度を優先)|
		<br>
		計測時は、以下の Div10SpeedTestSample 関数と同様な処理を行うコードを作成して計測しています。<br>
		(本ヘッダーのインクルードなどは省略しています)
		@code{.cpp}
		template <size_t BitSize> void Div10SpeedTestSample(void ) {

			CStdBitsetUnsignedOperation::StdBitset<BitSize> number1, ten( 10 );

			const size_t loopCount = 512;

			number1 = CStdBitsetUnsignedOperation::Random<BitSize>( );
			number1[BitSize - 1]  = true;

			uint64_t processPermille = 0, prevProcessPermille = 0;

			printf("# Division10WithRemainder Speed Test Sample (%zu ビット , %zu 回)\n\n", BitSize, loopCount );

			CStdChronoBasedStopWatchA  sw;

			for ( size_t i = 0; i < loopCount; i++ ) {

				sw.start( );
				CStdBitsetUnsignedOperation::Division10WithRemainder( number1 );
				sw.stop( );

				processPermille = ( i + 1 ) * 1000 / loopCount;
				if ( ( i == 0 || processPermille != prevProcessPermille ) ) {
					printf( "\rProgress: %llu.%llu%% (%zu / %zu)",
						processPermille / 10,
						processPermille % 10, i + 1,
						loopCount
					);
					printf( "  Current Elapsed Time : %s (%s us)",
						sw.getMicrosecondsString( StdChronoBasedStopWatchStringFormat::HHMMSSWithFractionalSeconds ).c_str( ),
						sw.getMicrosecondsString( StdChronoBasedStopWatchStringFormat::None ).c_str( ) );
				}
				prevProcessPermille = processPermille;
			}

			printf( "\nFinal Elapsed Time : %s  (%s us)\n\n",
				sw.getMicrosecondsString( StdChronoBasedStopWatchStringFormat::HHMMSSWithFractionalSeconds ).c_str( ),
				sw.getMicrosecondsString( StdChronoBasedStopWatchStringFormat::None ).c_str( ) );

			printf( "# DivisionWithRemainder Speed Test Sample (%zu ビット , %zu 回)\n\n", BitSize, loopCount );

			processPermille = 0; 
			prevProcessPermille = 0;
			sw.reset( );

			for ( size_t i = 0; i < loopCount; i++ ) {

				sw.start( );
				CStdBitsetUnsignedOperation::DivisionWithRemainder( number1, ten );
				sw.stop( );

				processPermille = ( i + 1 ) * 1000 / loopCount;
				if ( ( i == 0 || processPermille != prevProcessPermille ) ) {
					printf( "\rProgress: %llu.%llu%% (%zu / %zu)",
						processPermille / 10,
						processPermille % 10, i + 1,
						loopCount
					);
					printf( "  Current Elapsed Time : %s (%s us)",
						sw.getMicrosecondsString( StdChronoBasedStopWatchStringFormat::HHMMSSWithFractionalSeconds ).c_str( ),
						sw.getMicrosecondsString( StdChronoBasedStopWatchStringFormat::None ).c_str( ) );
				}
				prevProcessPermille = processPermille;
			}

			printf( "\nFinal Elapsed Time : %s  (%s us)\n\n",
				sw.getMicrosecondsString( StdChronoBasedStopWatchStringFormat::HHMMSSWithFractionalSeconds ).c_str( ),
				sw.getMicrosecondsString( StdChronoBasedStopWatchStringFormat::None ).c_str( ) );

		}
		@endcode
	**/
	template<size_t BitSize>  static StdBitsetPair<BitSize> Division10WithRemainder( StdBitsetConstRef<BitSize> input ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );

		const size_t digit_input = GetSignificantBitLength( input );

		const size_t digit_of_ten = 4; // 10は2進数で1010であり、4ビットで表現できるため

		if ( digit_input < digit_of_ten ) {
			//  inputが4ビット未満の時 (input < 8 の時)、商：0、あまり：input
			return std::pair( StdBitset<BitSize>( 0 ), input );
		}

		const size_t digit_quotient = digit_input - digit_of_ten + 1;

		uint8_t auxiliary = 0;

		StdBitset<BitSize> quotient( 0 );

		for ( size_t i = 0; i < digit_of_ten; i++ ) {
			auxiliary <<= 1;
			if ( input[digit_input - 1 - i] ) auxiliary |= 1;
		}


		for ( size_t i = 0; i < digit_quotient; i++ ) {

			if ( auxiliary >= 10 ) {
				quotient[digit_quotient - 1 - i] = true;
				auxiliary -= 10;
			}

			if ( ( i + 1 ) < digit_quotient ) {
				auxiliary <<= 1;
				if ( input[digit_input - digit_of_ten - i - 1] ) auxiliary |= 1;
			}

		}

		return std::pair( quotient, StdBitset<BitSize>( auxiliary ) );
	}

	/**
		@brief  ビットセットの値の有効ビット長を取得する
		@tparam BitSize - ビットセットのサイズ
		@param  input   - 入力値
		@pre BitSize > 0 (満たさない場合はstatic_assertによりコンパイルエラーとなります)
		@return         - 有効ビット長
		@remark 全ビットの値が0(false)の場合は、0という一桁の数値があるものとみなすため、1を返却します。
	**/
	template<size_t BitSize>  static size_t GetSignificantBitLength( StdBitsetConstRef<BitSize> input ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		for ( size_t i = BitSize - 1; i > 0; i-- ) {
			if ( input[i] ) return i + 1;
		}

		// ループが最後まで回ってもreturnされず、ここに来るケースは
		// 最下位ビットの値が0(false)もしくは1(true)で、かつそれ以外のビットは全て0(false)の場合のみである
		// 0 (false) は 1桁もないのではなく 0 という1桁の数値が有るものと見なす。
		return 1;
	}

	/**
		@brief  ビットセットの値を比較する
		@tparam BitSize     - ビットセットのサイズ
		@param  input_left  - 左側の入力値
		@param  input_right - 右側の入力値
		@pre BitSize > 0 (満たさない場合はstatic_assertによりコンパイルエラーとなります)
		@return 下表の通りの比較結果
		|戻り値|意味|
		|---|---|
		| CBitNumberSupport::CompareResult::LeftGreater |input_left > input_right|
		| CBitNumberSupport::CompareResult::Equal |input_left == input_right|
		| CBitNumberSupport::CompareResult::RightGreater |input_left < input_right|
   **/
	template<size_t BitSize>  static  CompareResult Compare( StdBitsetConstRef<BitSize> input_left, StdBitsetConstRef<BitSize> input_right ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		bool isStillEqual = true;
		bool isLeftAbove = false;
		size_t index;

		for ( size_t i = 0; ( i < BitSize ) && isStillEqual; i++ ) {
			index = BitSize - 1 - i;
			isLeftAbove = input_left[index] && ( !input_right[index] );
			if ( isLeftAbove ) return CompareResult::LeftGreater;

			isStillEqual = !( input_left[index] ^ input_right[index] );
		}

		return  ( isStillEqual ) ? CompareResult::Equal : CompareResult::RightGreater;
	}

	/**
		@brief  ビットセットの値を比較する（ビットサイズが異なる場合も対応）
		@tparam BitSizeL    - 左側のビットセットのサイズ
		@tparam BitSizeR    - 右側のビットセットのサイズ
		@param  input_left  - 左側の入力値
		@param  input_right - 右側の入力値
		@pre BitSizeL > 0 および BitSizeR > 0 (満たさない場合はstatic_assertによりコンパイルエラーとなります)
		@return  下表の通りの比較結果
		|戻り値|意味|
		|---|---|
		| CBitNumberSupport::CompareResult::LeftGreater |input_left > input_right|
		| CBitNumberSupport::CompareResult::Equal |input_left == input_right|
		| CBitNumberSupport::CompareResult::RightGreater |input_left < input_right|
		
		**/
	template<size_t BitSizeL, size_t BitSizeR>  static  CompareResult CompareExtend( StdBitsetConstRef<BitSizeL> input_left, StdBitsetConstRef<BitSizeR> input_right ) {
		static_assert( BitSizeL > 0, "BitSizeLは無効な値です。" );
		static_assert( BitSizeR > 0, "BitSizeRは無効な値です。" );

		if  constexpr ( BitSizeL == BitSizeR ) {
			return Compare<BitSizeL>( input_left, input_right );
		} else if  constexpr ( BitSizeL > BitSizeR ) {
			return Compare<BitSizeL>( input_left, CastSize<BitSizeL>( input_right ) );
		} else {
			return Compare<BitSizeR>( CastSize<BitSizeR>( input_left ), input_right );
		}

	}



	/**
		@brief 2つのビットセットのうちの値の大きい方を取得する
		@tparam BitSize     - ビットセットのサイズ
		@param  input_left  - 左側の入力値
		@param  input_right - 右側の入力値
		@pre BitSize > 0 (満たさない場合はstatic_assertによりコンパイルエラーとなります)
		@return  値が大きい方のビットセット
	**/
	template<size_t BitSize>  static  StdBitset<BitSize> Max( StdBitsetConstRef<BitSize> input_left, StdBitsetConstRef<BitSize> input_right ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		return ( Compare( input_left, input_right ) == CompareResult::LeftGreater ) ? input_left : input_right;
	}

	/**
		@brief 2つのビットセットのうちの値の小さい方を取得する
		@tparam BitSize     - ビットセットのサイズ
		@param  input_left  - 左側の入力値
		@param  input_right - 右側の入力値
		@pre BitSize > 0 (満たさない場合はstatic_assertによりコンパイルエラーとなります)
		@return  値が小さい方のビットセット
	**/
	template<size_t BitSize>  static  StdBitset<BitSize> Min( StdBitsetConstRef<BitSize> input_left, StdBitsetConstRef<BitSize> input_right ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );

		return ( Compare( input_left, input_right ) != CompareResult::LeftGreater ) ? input_left : input_right;
	}

	/**
		@brief  指定されたビットサイズまでのランダムなビットセットを生成する
		@tparam BitSize       - ビットセットのサイズ
		@param  fill_bit_size - 生成するビット数
		@pre BitSize > 0 (満たさない場合はstatic_assertによりコンパイルエラーとなります)
		@return               - ランダムに生成されたビットセット
	**/
	template<size_t BitSize>  static  StdBitset<BitSize> Random( size_t fill_bit_size = BitSize ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );

		static thread_local  std::mt19937_64   mtRandom( std::random_device {}( ) );

		StdBitset<BitSize>result;

		size_t genBitSize = std::min( BitSize, fill_bit_size );
		size_t numberOfBlocks = genBitSize / 64;
		size_t numberOfRestBits = genBitSize % 64;

		if ( numberOfRestBits > 0 ) numberOfBlocks++;

		size_t current_gen_bits;
		uint64_t current_random_value;

		for ( size_t i = 0; i < numberOfBlocks; i++ ) {

			current_gen_bits = 64;

			if ( ( ( i + 1 ) == numberOfBlocks ) && ( numberOfRestBits > 0 ) ) {
				current_gen_bits = numberOfRestBits;
			}

			result <<= current_gen_bits;

			current_random_value = mtRandom( );

			for ( size_t bit = 0; bit < current_gen_bits; bit++ ) {
				result[bit] = current_random_value & 1;
				current_random_value >>= 1;
			}

		}

		return result;
	}

	/**
		@brief  ビットセットを左にローテートさせる
		@tparam BitSize         - ビットセットのサイズ
		@param  input           - 入力ビットセット
		@param  rotate_bit_size - 回転させるビット数
		@pre BitSize > 0 (満たさない場合はstatic_assertによりコンパイルエラーとなります)
		@return                 - ローテート後のビットセット
	**/
	template<size_t BitSize>  static  StdBitset<BitSize> RotateLeft( StdBitsetConstRef<BitSize> input, size_t rotate_bit_size ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		if ( rotate_bit_size == 0 ) return input;
		rotate_bit_size %= BitSize;
		return ( input << rotate_bit_size ) | ( input >> ( BitSize - rotate_bit_size ) );
	}

	/**
		@brief  ビットセットを右にローテートさせる
		@tparam BitSize         - ビットセットのサイズ
		@param  input           - 入力ビットセット
		@param  rotate_bit_size - 回転させるビット数
		@pre BitSize > 0 (満たさない場合はstatic_assertによりコンパイルエラーとなります)
		@return                 - ローテート後のビットセット
	**/
	template<size_t BitSize>  static  StdBitset<BitSize> RotateRight( StdBitsetConstRef<BitSize> input, size_t rotate_bit_size ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		if ( rotate_bit_size == 0 ) return input;
		rotate_bit_size %= BitSize;
		return ( input >> rotate_bit_size ) | ( input << ( BitSize - rotate_bit_size ) );
	}

};
