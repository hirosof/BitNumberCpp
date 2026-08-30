/*
	このファイルは動作チェック用のコードです。
*/

#define NOMINMAX
#include <windows.h>
#include <locale>
#include <thread>
#include "../common/CStdBitsetUnsignedNumber.hpp"
#include "../common/CStdChronoBasedStopWatch.hpp"

template <size_t BitSize> void runSpeedTest( size_t loopCount = 1024 ) {
	CStdBitsetUnsignedNumberA<BitSize> number1, ten( 10 );

	CStdChronoBasedStopWatchA  sw;

	uint64_t processPermille = 0, prevProcessPermille = 0;

	number1.selfUpdateRandom( );
	number1.set( 0, true, CBitNumberSupport::BitOffsetBasis::Most );


	StdChronoBasedStopWatchStringFormat format_type = StdChronoBasedStopWatchStringFormat::HHMMSSWithFractionalSeconds;
	StdChronoBasedStopWatchStringFormat no_format = StdChronoBasedStopWatchStringFormat::None;


	wprintf( L"=== division10WithRemainder の速度計測 (%zu ビット , %zu 回)===\n", BitSize, loopCount );

	for ( size_t i = 0; i < loopCount; i++ ) {

		sw.start( );
		number1.division10WithRemainder( );
		sw.stop( );

		processPermille = ( i + 1 ) * 1000 / loopCount;

		if ( ( i == 0 || processPermille != prevProcessPermille ) ) {
			printf( "\rProgress: %llu.%llu%% (%zu / %zu)",
				processPermille / 10,
				processPermille % 10, i + 1,
				loopCount
			);


			printf( "  Current Elapsed Time : %s (%s us)", sw.getMicrosecondsString( format_type ).c_str(), sw.getMicrosecondsString( no_format ).c_str() );
		}

		prevProcessPermille = processPermille;
	}

	printf( "\nFinal Elapsed Time : %s  (%s us)\n\n", sw.getMicrosecondsString( format_type ).c_str( ), sw.getMicrosecondsString( no_format ).c_str( ) );

	wprintf( L"=== divisionWithRemainder の速度計測 (%zu ビット , %zu 回)===\n", BitSize, loopCount );

	sw.reset( );
	prevProcessPermille = 0;

	for ( size_t i = 0; i < loopCount; i++ ) {

		sw.start( );
		number1.divisionWithRemainder( ten );
		sw.stop( );

		processPermille = ( i + 1 ) * 1000 / loopCount;

		if ( ( i == 0 || processPermille != prevProcessPermille ) ) {
			printf( "\rProgress: %llu.%llu%% (%zu / %zu)",
				processPermille / 10,
				processPermille % 10, i + 1,
				loopCount
			);

			printf( "  Current Elapsed Time : %s (%s us)", sw.getMicrosecondsString( format_type ).c_str( ), sw.getMicrosecondsString( no_format ).c_str( ) );

		}

		prevProcessPermille = processPermille;
	}

	printf( "\nFinal Elapsed Time : %s  (%s us)\n\n", sw.getMicrosecondsString( format_type ).c_str( ), sw.getMicrosecondsString( no_format ).c_str( ) );

}


void SpeedTestEntry( ) {

	size_t loopCount = 1024;
	runSpeedTest<8192>( loopCount );

	/*
	runSpeedTest<2048>( loopCount );
	runSpeedTest<4096>( loopCount );
	runSpeedTest<8192>( loopCount );
	runSpeedTest<16384>( loopCount );
	runSpeedTest<32768>( loopCount );
	*/
}

int main( ) {

	// 日本語ロケールに設定
	std::setlocale( LC_ALL, "ja-JP.UTF-8" );

	// UTF-8コードページに変更
	SetConsoleOutputCP( CP_UTF8 );


	CStdChronoBasedStopWatchA sw;

	printf("CStdChronoBasedStopWatchAで計測中です・・・\n" );
	sw.start( );

	std::this_thread::sleep_for( std::chrono::milliseconds( 5123 ) );

	sw.stop( );



	printf( "Elapsed Time : %s (%s ms)\n", sw.getMillisecondsString( StdChronoBasedStopWatchStringFormat::HHMMSSWithFractionalSeconds ).c_str( ), sw.getMillisecondsString( StdChronoBasedStopWatchStringFormat::None ).c_str( ) );


	CStdChronoBasedStopWatchThreadSafeA sw_thread_safe;

	printf( "CStdChronoBasedStopWatchThreadSafeAで計測中です・・・\n" );
	sw_thread_safe.start( );

	std::this_thread::sleep_for( std::chrono::milliseconds( 5123 ) );

	sw_thread_safe.stop( );

	printf( "Elapsed Time (Thread Safe) : %s (%s ms)\n", sw_thread_safe.getMillisecondsString( StdChronoBasedStopWatchStringFormat::HHMMSSWithFractionalSeconds ).c_str( ), sw_thread_safe.getMillisecondsString( StdChronoBasedStopWatchStringFormat::None ).c_str( ) );

	return 0;
}
