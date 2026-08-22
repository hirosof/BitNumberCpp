#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <algorithm>
#include <mutex>

enum struct StdChronoBasedStopWatchStringFormat {
	None,
	Auto,
	MMSS,
	MMSSWithFractionalSeconds,
	HHMMSS,
	HHMMSSWithFractionalSeconds
};

template <typename CharType> class CStdChronoBasedStopWatch {

public:
	using StdString = std::basic_string<CharType>;
	using StdClock = std::chrono::steady_clock;
	using StdDuration = StdClock::duration;
	template<typename DurationType> using StdHHMMSS = std::chrono::hh_mm_ss<DurationType>;

private:

	bool m_isMeasuring;

	StdClock::time_point m_tp_begin;
	StdDuration m_pastAccumulatedDuration;

	mutable std::recursive_mutex m_mutex;

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

	template<typename T> static uint64_t StdDurationToUInt64( StdDuration d ) {
		return static_cast<uint64_t>( std::chrono::duration_cast<T>( d ).count( ) );
	}

	template<typename T> static StdDuration UInt64ToStdDuration( uint64_t value ) {
		return std::chrono::duration_cast<StdDuration>( T( value ) );
	}

	template<typename DurationType> static StdHHMMSS<DurationType> StdDurationToHHMMSS( StdDuration d ) {
		return StdHHMMSS<DurationType>( std::chrono::duration_cast<DurationType>( d ) );
	}

	template<typename DurationType> static StdHHMMSS<DurationType> UInt64ToHHMMSS( uint64_t value ) {
		return StdHHMMSS<DurationType>( DurationType( value ) );
	}

public:


	static StdHHMMSS<std::chrono::nanoseconds> StdDurationToNanosecondsHHMMSS( StdDuration d ) {
		return StdDurationToHHMMSS<std::chrono::nanoseconds>( d );
	}

	static StdHHMMSS<std::chrono::microseconds> StdDurationToMicrosecondsHHMMSS( StdDuration d ) {
		return StdDurationToHHMMSS<std::chrono::microseconds>( d );
	}

	static StdHHMMSS<std::chrono::milliseconds> StdDurationToMillisecondsHHMMSS( StdDuration d ) {
		return StdDurationToHHMMSS<std::chrono::milliseconds>( d );
	}

	static StdHHMMSS<std::chrono::seconds> StdDurationToSecondsHHMMSS( StdDuration d ) {
		return StdDurationToHHMMSS<std::chrono::seconds>( d );
	}

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

				if(ui64_seconds > 0 ) {
					result += UInt64ToString( ui64_seconds, 0 );
				}

				if ( StdHHMMSS<DurationType>::fractional_width > 0 ) {

					result += UInt64ToString( static_cast<uint64_t>( hhmmss.subseconds( ).count( ) ), 
						( ui64_seconds == 0 )  ? 0 : StdHHMMSS<DurationType>::fractional_width );
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

	static StdString CreateNanosecondsString( uint64_t total, StdChronoBasedStopWatchStringFormat format = StdChronoBasedStopWatchStringFormat::Auto ) {
		return HHMMSSToFormattedStdString( UInt64ToHHMMSS<std::chrono::nanoseconds>( total ), format );
	}

	static StdString CreateNanosecondsString( StdClock::duration d, StdChronoBasedStopWatchStringFormat format = StdChronoBasedStopWatchStringFormat::Auto ) {
		return HHMMSSToFormattedStdString( StdDurationToNanosecondsHHMMSS( d ), format );
	}


	static StdString CreateMicrosecondsString( uint64_t total, StdChronoBasedStopWatchStringFormat format = StdChronoBasedStopWatchStringFormat::Auto ) {
		return HHMMSSToFormattedStdString( UInt64ToHHMMSS<std::chrono::microseconds>( total ), format );
	}

	static StdString CreateMicrosecondsString( StdClock::duration d, StdChronoBasedStopWatchStringFormat format = StdChronoBasedStopWatchStringFormat::Auto ) {
		return HHMMSSToFormattedStdString( StdDurationToMicrosecondsHHMMSS( d ), format );
	}

	static StdString CreateMillisecondsString( uint64_t total, StdChronoBasedStopWatchStringFormat format = StdChronoBasedStopWatchStringFormat::Auto ) {
		return HHMMSSToFormattedStdString( UInt64ToHHMMSS<std::chrono::milliseconds>( total ), format );
	}

	static StdString CreateMillisecondsString( StdClock::duration d, StdChronoBasedStopWatchStringFormat format = StdChronoBasedStopWatchStringFormat::Auto ) {
		return HHMMSSToFormattedStdString( StdDurationToMillisecondsHHMMSS( d ), format );
	}

	static StdString CreateSecondsString( uint64_t total, StdChronoBasedStopWatchStringFormat format = StdChronoBasedStopWatchStringFormat::Auto ) {
		return HHMMSSToFormattedStdString( UInt64ToHHMMSS<std::chrono::seconds>( total ), format );
	}

	static StdString CreateSecondsString( StdClock::duration d, StdChronoBasedStopWatchStringFormat format = StdChronoBasedStopWatchStringFormat::Auto ) {
		return HHMMSSToFormattedStdString( StdDurationToSecondsHHMMSS( d ), format );
	}

	CStdChronoBasedStopWatch( ) : m_isMeasuring( false ), m_tp_begin( ), m_pastAccumulatedDuration( StdDuration::zero( ) ) , m_mutex( ) {

	}

	bool isMeasuring( void ) const {
		std::lock_guard<std::recursive_mutex> lock( m_mutex );
		return m_isMeasuring;
	}

	void start( void ) {
		std::lock_guard<std::recursive_mutex> lock( m_mutex );
		if ( !m_isMeasuring ) {
			m_tp_begin = StdClock::now( );
			m_isMeasuring = true;
		}
	}

	StdDuration stop( void ) {
		std::lock_guard<std::recursive_mutex> lock( m_mutex );
		if ( m_isMeasuring ) {
			m_pastAccumulatedDuration = get( );
			m_isMeasuring = false;
		}
		return m_pastAccumulatedDuration;
	}

	void reset( void ) {
		std::lock_guard<std::recursive_mutex> lock( m_mutex );
		m_pastAccumulatedDuration = StdDuration::zero( );
		m_isMeasuring = false;
	}

	void resetAndStart( void ) {
		std::lock_guard<std::recursive_mutex> lock( m_mutex );
		reset( );
		start( );
	}

	StdDuration get( void ) const {
		std::lock_guard<std::recursive_mutex> lock( m_mutex );
		if ( m_isMeasuring ) {
			StdClock::time_point tp_end = StdClock::now( );
			return m_pastAccumulatedDuration + ( tp_end - m_tp_begin );
		} else {
			return m_pastAccumulatedDuration;
		}
	}

	uint64_t getNanosecondsUInt64( ) const {
		return StdDurationToUInt64<std::chrono::nanoseconds>( get( ) );
	}

	uint64_t getMicrosecondsUInt64( ) const {
		return StdDurationToUInt64<std::chrono::microseconds>( get( ) );
	}

	uint64_t getMillisecondsUInt64( ) const {
		return StdDurationToUInt64<std::chrono::milliseconds>( get( ) );
	}

	uint64_t getSecondsUInt64( ) const {
		return StdDurationToUInt64<std::chrono::seconds>( get( ) );
	}

	StdHHMMSS<std::chrono::nanoseconds> getNanosecondsHHMMSS( ) const {
		return StdDurationToHHMMSS<std::chrono::nanoseconds>( get( ) );
	}

	StdHHMMSS<std::chrono::microseconds> getMicrosecondsHHMMSS( ) const {
		return StdDurationToHHMMSS<std::chrono::microseconds>( get( ) );
	}

	StdHHMMSS<std::chrono::milliseconds> getMillisecondsHHMMSS( ) const {
		return StdDurationToHHMMSS<std::chrono::milliseconds>( get( ) );
	}

	StdHHMMSS<std::chrono::seconds> getSecondsHHMMSS( ) const {
		return StdDurationToHHMMSS<std::chrono::seconds>( get( ) );
	}


	StdString getNanosecondsString( StdChronoBasedStopWatchStringFormat format = StdChronoBasedStopWatchStringFormat::Auto ) const {
		return CreateNanosecondsString( get( ), format );
	}

	StdString getMicrosecondsString( StdChronoBasedStopWatchStringFormat format = StdChronoBasedStopWatchStringFormat::Auto ) const {
		return CreateMicrosecondsString( get( ), format );
	}

	StdString getMillisecondsString( StdChronoBasedStopWatchStringFormat format = StdChronoBasedStopWatchStringFormat::Auto ) const {
		return CreateMillisecondsString( get( ), format );
	}

	StdString getSecondsString( StdChronoBasedStopWatchStringFormat format = StdChronoBasedStopWatchStringFormat::Auto ) const {
		return CreateSecondsString( get( ), format );
	}

};


using CStdChronoBasedStopWatchA = CStdChronoBasedStopWatch<char>;
using CStdChronoBasedStopWatchW = CStdChronoBasedStopWatch<wchar_t>;
