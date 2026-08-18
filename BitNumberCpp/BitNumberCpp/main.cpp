/*
	このファイルは動作チェック用のコードです。
*/


#define NOMINMAX
#include <windows.h>
#include <locale>
#include "../common/CUnsignedBitNumber.hpp"
#include "../common/CStdBitsetUnsignedNumber.hpp"
#include "../common/CStdChronoBasedStopWatch.hpp"

template <size_t BitSize> void runSpeedTest( size_t loopCount = 1024 ) {
	CStdBitsetUnsignedNumberA<BitSize> number1, ten( 10 );

	CStdChronoBasedStopWatchA  sw;

	uint64_t processPermille = 0, prevProcessPermille = 0;

	number1.selfUpdateRandom( );

	wprintf( L"=== division10WithRemainder の速度計測 (%zu ビット)===\n", BitSize );

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


			printf( "  Current Elapsed Time : %s", sw.getMilliSecondsString( true ).c_str( ) );
		}

		prevProcessPermille = processPermille;
	}

	printf( "\nElapsed Time : %s (%s ms)\n\n", sw.getMilliSecondsString( true ).c_str( ), sw.getMilliSecondsString( false ).c_str( ) );

	wprintf( L"=== divisionWithRemainder の速度計測 (%zu ビット)===\n", BitSize );

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

			printf( "  Current Elapsed Time : %s", sw.getMilliSecondsString( true ).c_str( ) );

		}

		prevProcessPermille = processPermille;
	}

	printf( "\nElapsed Time : %s (%s ms)\n\n", sw.getMilliSecondsString( true ).c_str( ), sw.getMilliSecondsString( false ).c_str( ) );

}


int main( ) {

	// 日本語ロケールに設定
	std::setlocale( LC_ALL, "ja-JP.UTF-8" );

	// UTF-8コードページに変更
	SetConsoleOutputCP( CP_UTF8 );

	size_t loopCount = 1024;

	runSpeedTest<4096>( loopCount );
	runSpeedTest<8192>( loopCount );
	runSpeedTest<16384>( loopCount );
	runSpeedTest<32768>( loopCount );

	return 0;
}
