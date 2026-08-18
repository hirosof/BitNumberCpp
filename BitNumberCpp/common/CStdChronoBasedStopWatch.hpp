#pragma once

#include <chrono>
#include <cstdint>
#include <string>


template <typename CharType> class CStdChronoBasedStopWatch {

public:
	using StdString = std::basic_string<CharType>;
	using StdClock = std::chrono::steady_clock;
private:

	bool isMeasuring;

	StdClock::time_point tp_begin;
	StdClock::duration preMeasured;


	static StdString ToString( uint64_t value, uint64_t min_digit ) {

		StdString result;
		uint64_t auxiliary = value;

		uint8_t current_digit_value;

		do {
			current_digit_value = auxiliary % 10;
			auxiliary /= 10;
			result.push_back( '0' + current_digit_value );
		} while ( auxiliary > 0 );

		size_t padding_size = (result.length( ) < min_digit) ? min_digit - result.length( ) : 0;

		if ( padding_size > 0 ) {
			result.append( padding_size, '0' );
		}

		std::reverse( result.begin( ), result.end( ) );

		return result;
	}

	static StdString ToFormattedSecondsString( uint64_t total_seconds ) {
		uint64_t hours = total_seconds / 3600;
		uint64_t minutes = ( total_seconds % 3600 ) / 60;
		uint64_t seconds = total_seconds % 60;
		StdString result;	
		result += ToString( hours, 2 );
		result.push_back( ':' );
		result += ToString( minutes, 2 );
		result.push_back( ':' );
		result += ToString( seconds, 2 );
		return result;
	}


public:

	static StdString CreateNanoSecondsString( uint64_t total, bool formatted ) {
		if ( formatted ) {
			StdString str = ToFormattedSecondsString( total / 1000000000 );
			str.push_back( '.' );
			str.append( ToString( total % 1000000000, 9 ) );
			return str;
		}
		return ToString( total, 0 );
	}


	static StdString CreateMicroSecondsString( uint64_t total, bool formatted ) {
		if ( formatted ) {
			StdString str = ToFormattedSecondsString( total / 1000000 );
			str.push_back( '.' );
			str.append( ToString( total % 1000000, 6 ) );
			return str;
		}
		return ToString( total, 0 );
	}

	static StdString CreateMilliSecondsString( uint64_t total, bool formatted ) {
		if ( formatted ) {
			StdString str = ToFormattedSecondsString( total / 1000 );
			str.push_back( '.' );
			str.append( ToString( total % 1000, 3 ) );
			return str;
		}
		return ToString( total, 0 );
	}

	static StdString CreateSecondsString( uint64_t total, bool formatted ) {
		if ( formatted ) {
			return ToFormattedSecondsString( total );
		}
		return ToString( total, 0 );
	}

	CStdChronoBasedStopWatch( ) :  isMeasuring( false ), tp_begin( ), preMeasured( StdClock::duration::zero( ) ) {

	}

	void start( ) {
		if ( !isMeasuring ) {
			tp_begin = StdClock::now( );
			isMeasuring = true;
		}
	}

	StdClock::duration stop( ) {
		if ( isMeasuring ) {
			preMeasured = get( );
			isMeasuring = false;
		}
		return preMeasured;
	}

	void reset( ) {
		preMeasured = StdClock::duration::zero( );	
		isMeasuring = false;
	}

	void resetWithStart( void) {
		reset( );
		start( );
	}

	StdClock::duration get( ) const {
		if ( isMeasuring ) {
			StdClock::time_point tp_end = StdClock::now( );
			return preMeasured + ( tp_end - tp_begin );
		} else {
			return preMeasured;
		}
	}

	uint64_t getNanoSeconds( ) const {
		using uint64_duration = std::chrono::duration<uint64_t, std::nano>;
		uint64_duration elapsed = std::chrono::duration_cast<uint64_duration>( get( ));
		return elapsed.count( );
	}

	uint64_t getMicroSeconds( ) const {
		using uint64_duration = std::chrono::duration<uint64_t, std::micro>;
		uint64_duration elapsed = std::chrono::duration_cast<uint64_duration>( get( ) );
		return elapsed.count( );
	}

	uint64_t getMilliSeconds( ) const {
		using uint64_duration = std::chrono::duration<uint64_t, std::milli>;
		uint64_duration elapsed = std::chrono::duration_cast<uint64_duration>( get( ) );
		return elapsed.count( );
	}

	uint64_t getSeconds( ) const {
		using uint64_duration = std::chrono::duration<uint64_t, std::ratio<1>>;
		uint64_duration elapsed = std::chrono::duration_cast<uint64_duration>( get( ) );
		return elapsed.count( );
	}

	StdString getNanoSecondsString( bool formatted = false ) const {
		return CreateNanoSecondsString(getNanoSeconds( ), formatted );
	}

	StdString getMicroSecondsString( bool formatted = false ) const {
		return CreateMicroSecondsString( getMicroSeconds( ), formatted );
	}

	StdString getMilliSecondsString( bool formatted = false ) const {
		return CreateMilliSecondsString( getMilliSeconds( ), formatted );
	}

	StdString getSecondsString( bool formatted = false ) const {
		return CreateSecondsString( getSeconds( ), formatted );
	}

};


using CStdChronoBasedStopWatchA = CStdChronoBasedStopWatch<char>;
using CStdChronoBasedStopWatchW = CStdChronoBasedStopWatch<wchar_t>;
