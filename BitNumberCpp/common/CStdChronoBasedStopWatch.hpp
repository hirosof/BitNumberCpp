/**

    @file      CStdChronoBasedStopWatch.hpp
	@brief     std::chrono をベースとしたストップウォッチクラスの定義
    @author    hirosof
    @copyright (C) 2026 hirosof.
	@note このファイルのビルドには C++20 以上のコンパイラが必要です。
**/
#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <algorithm>
#include <mutex>

/**
    @enum  StdChronoBasedStopWatchStringFormat
	@brief   CStdChronoBasedStopWatchクラスの文字列フォーマット指定用列挙型
**/
enum struct StdChronoBasedStopWatchStringFormat {

	/** フォーマットなし( 単純に数値を文字列化する ) */
	None,  

	/** 自動フォーマット<br>
	 *  None 以外の形式を、以下の規則で自動的に選択する<br>
	 *  * 時が 1 以上なら HH:MM:SS 系、そうでなければ MM:SS 系<br>
	 *  * fractional_width が 1 以上なら小数部付きの形式
	 */
	Auto,  

	/** MM:SS 形式
	* @remarks 1時間以上の場合、HHの値は分に変換され、MMの値に加算される <br>
			例: 1時間30分の場合、MM=90, SS=00 となり、 文字列化すると "90:00" となる
	*/
	MMSS,  

	/** MM:SS.fff 形式<br>
	 *  fff (小数点以下) の桁数は、std::chrono::hh_mm_ss<DurationType>::fractional_width に依存する<br>
	 *  なお、fff (小数点以下) の桁数が 0 の場合は、MM:SS 形式と同じになる
	 * 
	 * @remarks 1時間以上の場合、HHの値は分に変換され、MMの値に加算される <br>
	 *		例: 1時間30分 (かつ、fractional_width が 3)の場合、MM=90, SS=00 となり、 文字列化すると "90:00.000" となる
	 */
	MMSSWithFractionalSeconds, 

	/** HH:MM:SS 形式 */
	HHMMSS,  

	/** HH:MM:SS.fff 形式<br>
	 *  fff (小数点以下) の桁数は、std::chrono::hh_mm_ss<DurationType>::fractional_width に依存する<br>
	 *  なお、fff (小数点以下) の桁数が 0 の場合は、HH:MM:SS 形式と同じになる
	 */
	HHMMSSWithFractionalSeconds  
};



/**

    @class   CStdChronoBasedStopWatch
	@brief   ストップウォッチクラス ( std::chrono ベース )
	@tparam  CharType - char または wchar_t などの文字型
	@details
		このクラスは、std::chrono をベースとしたストップウォッチ機能を提供します。<br>
		測定開始、停止、リセット、および経過時間の取得が可能です。<br>
		また、経過時間を指定されたフォーマットで文字列化する機能も提供します。
	@remarks
		スレッドセーフではありませんので、マルチスレッド環境で使用する場合は、別途、排他処理を行うか、<br>
		CStdChronoBasedStopWatchThreadSafe クラスを使用してください。
**/
template <typename CharType> class CStdChronoBasedStopWatch {

public:

	/// CharType に依存した文字列型
	using StdString = std::basic_string<CharType>;

	/// std::chrono に依存した型定義
	using StdClock = std::chrono::steady_clock;

	/// std::chrono に依存した型定義
	using StdDuration = StdClock::duration;

	/// std::chrono に依存した型定義
	template<typename DurationType> using StdHHMMSS = std::chrono::hh_mm_ss<DurationType>;

private:

	/// 測定中かどうかを示すフラグ
	bool m_isMeasuring;

	/// 測定開始時刻
	StdClock::time_point m_tp_begin;

	/// 過去の累積測定時間
	StdDuration m_pastAccumulatedDuration;

	/**
		@brief  uint64_t 型の値を文字列に変換する
		@param  value     - 変換する値
		@param  min_digit - 最小桁数 (最小桁数に満たない場合は0で埋める)
		@return  変換後の文字列
	**/
	static StdString UInt64ToString( uint64_t value, size_t min_digit ) {

		StdString result;
		uint64_t auxiliary = value;

		uint8_t current_digit_value;

		do {
			current_digit_value = static_cast<uint8_t>( auxiliary % 10 );
			auxiliary /= 10;
			result.push_back( static_cast<CharType>( '0' + current_digit_value ) );
		} while ( auxiliary > 0 );

		size_t padding_size = ( result.length( ) < min_digit ) ? min_digit - result.length( ) : 0;

		if ( padding_size > 0 ) {
			result.append( padding_size, static_cast<CharType>( '0' ) );
		}

		std::reverse( result.begin( ), result.end( ) );

		return result;
	}

	/**
		@brief  StdDuration 型の時間を uint64_t 型に変換する
		@tparam T  - 変換先の時間単位
		@param  d  - StdDuration 型の時間
		@return  変換後の uint64_t 型の時間
	**/
	template<typename T> static uint64_t StdDurationToUInt64( StdDuration d ) {
		return static_cast<uint64_t>( std::chrono::duration_cast<T>( d ).count( ) );
	}

	/**
		@brief  uint64_t 型の値を StdDuration 型に変換する
		@tparam T     - 変換先の時間単位
		@param  value - 変換する値
		@return  変換後の StdDuration 型の時間
	**/
	template<typename T> static StdDuration UInt64ToStdDuration( uint64_t value ) {
		return std::chrono::duration_cast<StdDuration>( T( value ) );
	}

	/**
		@brief  StdDuration 型の時間を StdHHMMSS<DurationType> (std::chrono::hh_mm_ss<DurationType> 型) に変換する
		@tparam DurationType - 変換先の時間単位
		@param  d            - StdDuration 型の時間
		@return  変換後の StdHHMMSS<DurationType> (std::chrono::hh_mm_ss<DurationType> 型) の時間
	**/
	template<typename DurationType> static StdHHMMSS<DurationType> StdDurationToHHMMSS( StdDuration d ) {
		return StdHHMMSS<DurationType>( std::chrono::duration_cast<DurationType>( d ) );
	}

	/**
		@brief  uint64_t 型の値を StdHHMMSS<DurationType> (std::chrono::hh_mm_ss<DurationType> 型) に変換する
		@tparam DurationType - 変換先の時間単位
		@param  value        - 変換する値
		@return  変換後の StdHHMMSS<DurationType> (std::chrono::hh_mm_ss<DurationType> 型) の時間
	**/
	template<typename DurationType> static StdHHMMSS<DurationType> UInt64ToHHMMSS( uint64_t value ) {
		return StdHHMMSS<DurationType>( DurationType( value ) );
	}

public:

	/**
		@brief  StdDuration を StdHHMMSS<std::chrono::nanoseconds> (std::chrono::hh_mm_ss<std::chrono::nanoseconds> 型) に変換する
		@param  d - StdDuration 型の時間
		@return  StdHHMMSS<std::chrono::nanoseconds> (std::chrono::hh_mm_ss<std::chrono::nanoseconds> 型) の時間
	**/
	static StdHHMMSS<std::chrono::nanoseconds> StdDurationToNanosecondsHHMMSS( StdDuration d ) {
		return StdDurationToHHMMSS<std::chrono::nanoseconds>( d );
	}

	/**
		@brief  StdDuration を StdHHMMSS<std::chrono::microseconds> (std::chrono::hh_mm_ss<std::chrono::microseconds> 型) に変換する
		@param  d - StdDuration 型の時間
		@return  StdHHMMSS<std::chrono::microseconds> (std::chrono::hh_mm_ss<std::chrono::microseconds> 型) の時間
	**/
	static StdHHMMSS<std::chrono::microseconds> StdDurationToMicrosecondsHHMMSS( StdDuration d ) {
		return StdDurationToHHMMSS<std::chrono::microseconds>( d );
	}

	/**
		@brief  StdDuration を StdHHMMSS<std::chrono::milliseconds> (std::chrono::hh_mm_ss<std::chrono::milliseconds> 型) に変換する
		@param  d - StdDuration 型の時間
		@return  StdHHMMSS<std::chrono::milliseconds> (std::chrono::hh_mm_ss<std::chrono::milliseconds> 型) の時間
	**/
	static StdHHMMSS<std::chrono::milliseconds> StdDurationToMillisecondsHHMMSS( StdDuration d ) {
		return StdDurationToHHMMSS<std::chrono::milliseconds>( d );
	}

	/**
		@brief  StdDuration を StdHHMMSS<std::chrono::seconds> (std::chrono::hh_mm_ss<std::chrono::seconds> 型) に変換する
		@param  d - StdDuration 型の時間
		@return  StdHHMMSS<std::chrono::seconds> (std::chrono::hh_mm_ss<std::chrono::seconds> 型) の時間
	**/
	static StdHHMMSS<std::chrono::seconds> StdDurationToSecondsHHMMSS( StdDuration d ) {
		return StdDurationToHHMMSS<std::chrono::seconds>( d );
	}

	/**
		@brief  StdHHMMSS<DurationType> (std::chrono::hh_mm_ss<DurationType> 型) をフォーマット指定に従って文字列化する
		@tparam DurationType - StdHHMMSS<DurationType> (std::chrono::hh_mm_ss<DurationType> 型) の時間単位
		@param  hhmmss - StdHHMMSS<DurationType> (std::chrono::hh_mm_ss<DurationType> 型) の時間
		@param  format - フォーマット指定
		@return  フォーマットに従って文字列化された時間
	**/
	template<typename DurationType> static StdString HHMMSSToFormattedStdString( StdHHMMSS<DurationType> hhmmss, StdChronoBasedStopWatchStringFormat format = StdChronoBasedStopWatchStringFormat::Auto ) {
		StdString result;

		StdChronoBasedStopWatchStringFormat real_format = format;

		if ( real_format == StdChronoBasedStopWatchStringFormat::None ) {

			//念のため符号チェックを行っておく
			if ( hhmmss.is_negative( ) ) {
				result.push_back( static_cast<CharType>( '-' ) );
				uint64_t ui64_seconds = static_cast<uint64_t>( hhmmss.hours( ).count( ) ) * 3600;
				ui64_seconds += static_cast<uint64_t>( hhmmss.minutes( ).count( ) ) * 60;
				ui64_seconds += static_cast<uint64_t>( hhmmss.seconds( ).count( ) );

				if ( ui64_seconds > 0 ) {
					result += UInt64ToString( ui64_seconds, 0 );
				}

				if ( StdHHMMSS<DurationType>::fractional_width > 0 ) {

					result += UInt64ToString( static_cast<uint64_t>( hhmmss.subseconds( ).count( ) ),
						( ui64_seconds == 0 ) ? 0 : StdHHMMSS<DurationType>::fractional_width );
				}
				return result;
			}

			return UInt64ToString( static_cast<uint64_t>( hhmmss.to_duration( ).count( ) ), 0 );
		}


		//念のため符号チェックを行っておく
		if ( hhmmss.is_negative( ) ) {
			result.push_back( static_cast<CharType>( '-' ) );
		}

		// Autoフォーマット時におけるモード設定
		switch ( real_format ) {
			case StdChronoBasedStopWatchStringFormat::Auto:
				if ( hhmmss.hours( ).count( ) > 0 ) {
					if ( StdHHMMSS<DurationType>::fractional_width > 0 ) {
						real_format = StdChronoBasedStopWatchStringFormat::HHMMSSWithFractionalSeconds;
					} else {
						real_format = StdChronoBasedStopWatchStringFormat::HHMMSS;
					}
				} else {
					if ( StdHHMMSS<DurationType>::fractional_width > 0 ) {
						real_format = StdChronoBasedStopWatchStringFormat::MMSSWithFractionalSeconds;
					} else {
						real_format = StdChronoBasedStopWatchStringFormat::MMSS;
					}
				}
				break;
			default:
				break;
		}

		// フォーマットに応じた文字列生成
		uint64_t adjustment_mm_value = 0;

		// HHの処理
		switch ( real_format ) {
			case StdChronoBasedStopWatchStringFormat::HHMMSS:
			case StdChronoBasedStopWatchStringFormat::HHMMSSWithFractionalSeconds:
				result += UInt64ToString( static_cast<uint64_t>( hhmmss.hours( ).count( ) ), 2 );
				result.push_back( static_cast<CharType>( ':' ) );
				break;
			case StdChronoBasedStopWatchStringFormat::MMSS:
			case StdChronoBasedStopWatchStringFormat::MMSSWithFractionalSeconds:
				adjustment_mm_value = static_cast<uint64_t>( hhmmss.hours( ).count( ) ) * 60;
				break;
			default:
				break;
		}

		//MMの処理
		result += UInt64ToString( static_cast<uint64_t>( hhmmss.minutes( ).count( ) ) + adjustment_mm_value, 2 );
		result.push_back( static_cast<CharType>( ':' ) );

		//SSの処理
		result += UInt64ToString( static_cast<uint64_t>( hhmmss.seconds( ).count( ) ), 2 );


		//小数点以下の処理 ( fractional_width > 0 の場合のみ)
		if ( StdHHMMSS<DurationType>::fractional_width > 0 ) {
			switch ( real_format ) {
				case StdChronoBasedStopWatchStringFormat::MMSSWithFractionalSeconds:
				case StdChronoBasedStopWatchStringFormat::HHMMSSWithFractionalSeconds:
					result.push_back( static_cast<CharType>( '.' ) );
					result += UInt64ToString( static_cast<uint64_t>( hhmmss.subseconds( ).count( ) ), StdHHMMSS<DurationType>::fractional_width );
					break;
				default:
					break;
			}
		}

		return result;
	}

	/**
		@brief  ナノ秒単位の時間を文字列化する
		@param  total  - ナノ秒単位の時間 (ナノ秒単位であると仮定されます)
		@param  format - 文字列化のフォーマット
		@return  フォーマットに従った文字列
	**/
	static StdString CreateNanosecondsString( uint64_t total, StdChronoBasedStopWatchStringFormat format = StdChronoBasedStopWatchStringFormat::Auto ) {
		return HHMMSSToFormattedStdString( UInt64ToHHMMSS<std::chrono::nanoseconds>( total ), format );
	}

	/**
		@brief  ナノ秒単位の時間を文字列化する
		@param  d      -  StdClock::duration 型の時間
		@param  format - 文字列化のフォーマット
		@return  フォーマットに従った文字列
	**/
	static StdString CreateNanosecondsString( StdClock::duration d, StdChronoBasedStopWatchStringFormat format = StdChronoBasedStopWatchStringFormat::Auto ) {
		return HHMMSSToFormattedStdString( StdDurationToNanosecondsHHMMSS( d ), format );
	}


	/**
		@brief  マイクロ秒単位の時間を文字列化する
		@param  total  - マイクロ秒単位の時間 (マイクロ秒単位であると仮定されます)
		@param  format - 文字列化のフォーマット
		@return  フォーマットに従った文字列
	**/
	static StdString CreateMicrosecondsString( uint64_t total, StdChronoBasedStopWatchStringFormat format = StdChronoBasedStopWatchStringFormat::Auto ) {
		return HHMMSSToFormattedStdString( UInt64ToHHMMSS<std::chrono::microseconds>( total ), format );
	}

	/**
		@brief  マイクロ秒単位の時間を文字列化する
		@param  d      - StdClock::duration 型の時間
		@param  format - 文字列化のフォーマット
		@return  フォーマットに従った文字列
	**/
	static StdString CreateMicrosecondsString( StdClock::duration d, StdChronoBasedStopWatchStringFormat format = StdChronoBasedStopWatchStringFormat::Auto ) {
		return HHMMSSToFormattedStdString( StdDurationToMicrosecondsHHMMSS( d ), format );
	}

	/**
		@brief  ミリ秒単位の時間を文字列化する
		@param  total  - ミリ秒単位の時間 (ミリ秒単位であると仮定されます)
		@param  format - 文字列化のフォーマット
		@return  フォーマットに従った文字列
	**/
	static StdString CreateMillisecondsString( uint64_t total, StdChronoBasedStopWatchStringFormat format = StdChronoBasedStopWatchStringFormat::Auto ) {
		return HHMMSSToFormattedStdString( UInt64ToHHMMSS<std::chrono::milliseconds>( total ), format );
	}

	/**
		@brief  ミリ秒単位の時間を文字列化する
		@param  d      - StdClock::duration 型の時間
		@param  format - 文字列化のフォーマット
		@return  フォーマットに従った文字列
	**/
	static StdString CreateMillisecondsString( StdClock::duration d, StdChronoBasedStopWatchStringFormat format = StdChronoBasedStopWatchStringFormat::Auto ) {
		return HHMMSSToFormattedStdString( StdDurationToMillisecondsHHMMSS( d ), format );
	}

	/**
	 @brief  秒単位の時間を文字列化する
	 @param  total  - 秒単位の時間 (秒単位であると仮定されます)
	 @param  format - 文字列化のフォーマット
	 @return  フォーマットに従った文字列
	 **/
	static StdString CreateSecondsString( uint64_t total, StdChronoBasedStopWatchStringFormat format = StdChronoBasedStopWatchStringFormat::Auto ) {
		return HHMMSSToFormattedStdString( UInt64ToHHMMSS<std::chrono::seconds>( total ), format );
	}

	/**
		@brief  秒単位の時間を文字列化する
		@param  d      - StdClock::duration 型の時間
		@param  format - 文字列化のフォーマット
		@return  フォーマットに従った文字列
	**/
	static StdString CreateSecondsString( StdClock::duration d, StdChronoBasedStopWatchStringFormat format = StdChronoBasedStopWatchStringFormat::Auto ) {
		return HHMMSSToFormattedStdString( StdDurationToSecondsHHMMSS( d ), format );
	}

	CStdChronoBasedStopWatch( ) : m_isMeasuring( false ), m_tp_begin( ), m_pastAccumulatedDuration( StdDuration::zero( ) ) {

	}

	CStdChronoBasedStopWatch( const CStdChronoBasedStopWatch & ) = delete;	
	CStdChronoBasedStopWatch& operator=( const CStdChronoBasedStopWatch& ) = delete;
	CStdChronoBasedStopWatch( CStdChronoBasedStopWatch&& ) = delete;
	CStdChronoBasedStopWatch& operator=( CStdChronoBasedStopWatch&& ) = delete;

	virtual ~CStdChronoBasedStopWatch( ) {}


	/**
		@brief  測定中かどうかを取得する
		@return  測定中であれば true、それ以外は false
	**/
	virtual bool isMeasuring( void ) const {
		return m_isMeasuring;
	}

	/**
		@brief  測定を開始する (測定中であれば何もしない)

		過去の測定結果は保持されるため、stop() で停止した後に start() を呼び出すと、以前の測定結果に加算される。
	**/
	virtual void start( void ) {
		if ( !m_isMeasuring ) {
			m_tp_begin = StdClock::now( );
			m_isMeasuring = true;
		}
	}

	/**
		@brief  測定を停止する (測定していない場合は何もしない)
		@return  停止後の累積測定時間
	**/
	virtual StdDuration stop( void ) {
		if ( m_isMeasuring ) {
			m_pastAccumulatedDuration = CStdChronoBasedStopWatch::get( );
			m_isMeasuring = false;
		}
		return m_pastAccumulatedDuration;
	}

	/**
		@brief  測定をリセットする (測定中であれば停止する)

		過去の測定結果は破棄されるため、リセット後に start() を呼び出すと、0 からの測定が開始される。
	**/
	virtual void reset( void ) {
		m_pastAccumulatedDuration = StdDuration::zero( );
		m_isMeasuring = false;
	}

	/**
		@brief  測定をリセットして開始する
	**/
	virtual void resetAndStart( void ) {
		CStdChronoBasedStopWatch::reset( );
		CStdChronoBasedStopWatch::start( );
	}


	/**
		@brief  現在の累積測定時間を取得する
		@return  測定中であれば現在の累積測定時間、それ以外は過去の累積測定時間
	**/
	virtual StdDuration get( void ) const {
		if ( m_isMeasuring ) {
			StdClock::time_point tp_end = StdClock::now( );
			return m_pastAccumulatedDuration + ( tp_end - m_tp_begin );
		} else {
			return m_pastAccumulatedDuration;
		}
	}


	/**
		@brief  ナノ秒単位の累積測定時間を取得する
		@return  測定中であれば現在の累積測定時間、それ以外は過去の累積測定時間
	**/
	uint64_t getNanosecondsUInt64( ) const {
		return StdDurationToUInt64<std::chrono::nanoseconds>( get( ) );
	}

	/**
		@brief  マイクロ秒単位の累積測定時間を取得する
		@return  測定中であれば現在の累積測定時間、それ以外は過去の累積測定時間
	**/
	uint64_t getMicrosecondsUInt64( ) const {
		return StdDurationToUInt64<std::chrono::microseconds>( get( ) );
	}

	/**
		@brief  ミリ秒単位の累積測定時間を取得する
		@return  測定中であれば現在の累積測定時間、それ以外は過去の累積測定時間
	**/
	uint64_t getMillisecondsUInt64( ) const {
		return StdDurationToUInt64<std::chrono::milliseconds>( get( ) );
	}

	/**
		@brief  秒単位の累積測定時間を取得する
		@return  測定中であれば現在の累積測定時間、それ以外は過去の累積測定時間
	**/
	uint64_t getSecondsUInt64( ) const {
		return StdDurationToUInt64<std::chrono::seconds>( get( ) );
	}

	/**
		@brief  ナノ秒単位の累積測定時間を取得する
		@return  測定中であれば現在の累積測定時間、それ以外は過去の累積測定時間
	**/
	StdHHMMSS<std::chrono::nanoseconds> getNanosecondsHHMMSS( ) const {
		return StdDurationToHHMMSS<std::chrono::nanoseconds>( get( ) );
	}

	/**
		@brief  マイクロ秒単位の累積測定時間を取得する
		@return  測定中であれば現在の累積測定時間、それ以外は過去の累積測定時間
	**/
	StdHHMMSS<std::chrono::microseconds> getMicrosecondsHHMMSS( ) const {
		return StdDurationToHHMMSS<std::chrono::microseconds>( get( ) );
	}

	/**
		@brief  ミリ秒単位の累積測定時間を取得する
		@return  測定中であれば現在の累積測定時間、それ以外は過去の累積測定時間
	**/
	StdHHMMSS<std::chrono::milliseconds> getMillisecondsHHMMSS( ) const {
		return StdDurationToHHMMSS<std::chrono::milliseconds>( get( ) );
	}

	/**
	 @brief  秒単位の累積測定時間を取得する
	 @return  測定中であれば現在の累積測定時間、それ以外は過去の累積測定時間
	 **/
	StdHHMMSS<std::chrono::seconds> getSecondsHHMMSS( ) const {
		return StdDurationToHHMMSS<std::chrono::seconds>( get( ) );
	}

	/**
		@brief  ナノ秒単位の累積測定時間を文字列として取得する
		@param  format - 文字列のフォーマット
		@return  フォーマットされた累積測定時間の文字列
	**/
	StdString getNanosecondsString( StdChronoBasedStopWatchStringFormat format = StdChronoBasedStopWatchStringFormat::Auto ) const {
		return CreateNanosecondsString( get( ), format );
	}

	/**
		@brief  マイクロ秒単位の累積測定時間を文字列として取得する
		@param  format - 文字列のフォーマット
		@return  フォーマットされた累積測定時間の文字列
	**/
	StdString getMicrosecondsString( StdChronoBasedStopWatchStringFormat format = StdChronoBasedStopWatchStringFormat::Auto ) const {
		return CreateMicrosecondsString( get( ), format );
	}

	/**
		@brief  ミリ秒単位の累積測定時間を文字列として取得する
		@param  format - 文字列のフォーマット
		@return  フォーマットされた累積測定時間の文字列
	**/
	StdString getMillisecondsString( StdChronoBasedStopWatchStringFormat format = StdChronoBasedStopWatchStringFormat::Auto ) const {
		return CreateMillisecondsString( get( ), format );
	}

	/**
		@brief  秒単位の累積測定時間を文字列として取得する
		@param  format - 文字列のフォーマット
		@return   フォーマットされた累積測定時間の文字列
	**/
	StdString getSecondsString( StdChronoBasedStopWatchStringFormat format = StdChronoBasedStopWatchStringFormat::Auto ) const {
		return CreateSecondsString( get( ), format );
	}

};


/// @brief  char 型を使用する CStdChronoBasedStopWatch のエイリアス
using CStdChronoBasedStopWatchA = CStdChronoBasedStopWatch<char>;

/// @brief  wchar_t 型を使用する CStdChronoBasedStopWatch のエイリアス
using CStdChronoBasedStopWatchW = CStdChronoBasedStopWatch<wchar_t>;


/**

	@class   CStdChronoBasedStopWatchThreadSafe
	@brief   スレッドセーフなストップウォッチクラス ( std::chrono ベース )
	@tparam  CharType - char または wchar_t などの文字型
	@details
		このクラスは、 CStdChronoBasedStopWatchのスレッドセーフ版であり、<br>
		std::chrono をベースとしたストップウォッチ機能を提供します。<br>
		測定開始、停止、リセット、および経過時間の取得が可能です。<br>
		また、経過時間を指定されたフォーマットで文字列化する機能も提供します。
	@remarks
		このクラスは、CStdChronoBasedStopWatch クラスを継承しており、操作・取得系の関数の内部で再帰的ミューテックスを使用して排他制御を行っており、<br>
		マルチスレッド環境でも安全に使用できます。ただし、複数の関数の呼び出しは、排他制御の対象外であるため、必要に応じて外部で排他処理を行ってください。<br>
		<br>
		例えば、以下で定義されるfunc1とfunc2が別のスレッドからほぼ同時に呼び出された場合、func1におけるisMeasuringの値が、trueになることを保証することができません。<br>
		これは、 func1 の start() と isMeasuring() の間に、別のスレッドから呼び出された func2 において、 stop() が呼び出される可能性があるためです。<br>
		@code{.cpp}
		CStdChronoBasedStopWatchThreadSafe<char> sw;

		void func1(void) {
			bool isMeasuring;

			sw.start( );
			isMeasuring = sw.isMeasuring( );
		}

		void func2(void) {
			sw.stop( );
		}
		@endcode

**/
template <typename CharType> class CStdChronoBasedStopWatchThreadSafe  final : public CStdChronoBasedStopWatch<CharType> {

private:
	/// 排他制御用の再帰的ミューテックス
	mutable std::recursive_mutex m_mutex;
public:

	/// 基底の CStdChronoBasedStopWatch<CharType> 型のエイリアス
	using BaseType = CStdChronoBasedStopWatch<CharType>;


	CStdChronoBasedStopWatchThreadSafe(){}
	CStdChronoBasedStopWatchThreadSafe( const CStdChronoBasedStopWatchThreadSafe& ) = delete;
	CStdChronoBasedStopWatchThreadSafe& operator=( const CStdChronoBasedStopWatchThreadSafe& ) = delete;
	CStdChronoBasedStopWatchThreadSafe( CStdChronoBasedStopWatchThreadSafe&& ) = delete;
	CStdChronoBasedStopWatchThreadSafe& operator=( CStdChronoBasedStopWatchThreadSafe&& ) = delete;

	/**
		@brief  CStdChronoBasedStopWatch<CharType>::isMeasuring()に対し、排他制御を行ったスレッドセーフ版です。<br>
		戻り値を含む詳細は CStdChronoBasedStopWatch<CharType>::isMeasuring() を参照してください。
	**/
	bool isMeasuring( void ) const override {
		std::lock_guard<std::recursive_mutex> lock( m_mutex );
		return BaseType::isMeasuring( );
	}

	/**
		@brief CStdChronoBasedStopWatch<CharType>::start()に対し、排他制御を行ったスレッドセーフ版です。<br>
		詳細は CStdChronoBasedStopWatch<CharType>::start() を参照してください。
	**/
	void start( void ) override {
		std::lock_guard<std::recursive_mutex> lock( m_mutex );
		BaseType::start( );
	}

	/**
		@brief  CStdChronoBasedStopWatch<CharType>::stop()に対し、排他制御を行ったスレッドセーフ版です。<br>
		戻り値を含む詳細は CStdChronoBasedStopWatch<CharType>::stop() を参照してください。
	**/
	BaseType::StdDuration stop( void ) override {
		std::lock_guard<std::recursive_mutex> lock( m_mutex );
		return BaseType::stop( );
	}

	/**
		@brief  CStdChronoBasedStopWatch<CharType>::reset()に対し、排他制御を行ったスレッドセーフ版です。<br>
		詳細は CStdChronoBasedStopWatch<CharType>::reset() を参照してください。
	**/
	void reset( void ) override {
		std::lock_guard<std::recursive_mutex> lock( m_mutex );
		BaseType::reset( );
	}

	/**
		@brief  CStdChronoBasedStopWatch<CharType>::resetAndStart()に対し、排他制御を行ったスレッドセーフ版です。<br>
		詳細は CStdChronoBasedStopWatch<CharType>::resetAndStart() を参照してください。
	**/
	void resetAndStart( void ) override {
		std::lock_guard<std::recursive_mutex> lock( m_mutex );
		BaseType::resetAndStart( );
	}


	/**
		@brief  CStdChronoBasedStopWatch<CharType>::get()に対し、排他制御を行ったスレッドセーフ版です。<br>
		戻り値を含む詳細は CStdChronoBasedStopWatch<CharType>::get() を参照してください。
	**/
	BaseType::StdDuration get( void ) const override {
		std::lock_guard<std::recursive_mutex> lock( m_mutex );
		return BaseType::get( );
	}
};


/// @brief  char 型を使用する CStdChronoBasedStopWatchThreadSafe のエイリアス
using CStdChronoBasedStopWatchThreadSafeA = CStdChronoBasedStopWatchThreadSafe<char>;

/// @brief  wchar_t 型を使用する CStdChronoBasedStopWatchThreadSafe のエイリアス
using CStdChronoBasedStopWatchThreadSafeW = CStdChronoBasedStopWatchThreadSafe<wchar_t>;
