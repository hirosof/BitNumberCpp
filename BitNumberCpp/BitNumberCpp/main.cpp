/*
	このファイルは動作チェック用のコードです。
*/

#define NOMINMAX
#include <windows.h>
#include <locale>
#include <thread>
#include "../common/CStdBitsetUnsignedNumber.hpp"
#include "../common/CStdChronoBasedStopWatch.hpp"


struct SpeedTestResult {
	size_t bitSize;
	size_t loopCount;
	CStdChronoBasedStopWatchA::StdDuration elapsedTimeDivision10WithRemainder;
	CStdChronoBasedStopWatchA::StdDuration elapsedTimeDivisionWithRemainder;
};

template <size_t BitSize> SpeedTestResult runSpeedTest( size_t loopCount = 1024 ) {
	CStdBitsetUnsignedNumberA<BitSize> number1, ten( 10 );
	typename CStdBitsetUnsignedNumberA<BitSize>::StdBitset n1set, tenset( 10 );

	CStdChronoBasedStopWatchA  sw;

	uint64_t processPermille = 0, prevProcessPermille = 0;

	number1.selfUpdateRandom( );
	number1.set( 0, true, CBitNumberSupport::BitOffsetBasis::Most );

	n1set = number1.getStdBitsetValue( );

	SpeedTestResult res;

	res.bitSize = BitSize;
	res.loopCount = loopCount;

	StdChronoBasedStopWatchStringFormat format_type = StdChronoBasedStopWatchStringFormat::HHMMSSWithFractionalSeconds;
	StdChronoBasedStopWatchStringFormat no_format = StdChronoBasedStopWatchStringFormat::None;


	wprintf( L"=== Division10WithRemainder の速度計測 (%zu ビット , %zu 回)===\n", BitSize, loopCount );

	for ( size_t i = 0; i < loopCount; i++ ) {

		number1.selfUpdateRandom( );
		number1.set( 0, true, CBitNumberSupport::BitOffsetBasis::Most );

		n1set = number1.getStdBitsetValue( );

		sw.start( );
		CStdBitsetUnsignedOperation::Division10WithRemainder( n1set );
//		number1.division10WithRemainder( );
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

	res.elapsedTimeDivision10WithRemainder = sw.get( );

	printf( "\nFinal Elapsed Time : %s  (%s us)\n\n", sw.getMicrosecondsString( format_type ).c_str( ), sw.getMicrosecondsString( no_format ).c_str( ) );

	wprintf( L"=== DivisionWithRemainder の速度計測 (%zu ビット , %zu 回)===\n", BitSize, loopCount );

	sw.reset( );
	prevProcessPermille = 0;

	for ( size_t i = 0; i < loopCount; i++ ) {
		number1.selfUpdateRandom( );
		number1.set( 0, true, CBitNumberSupport::BitOffsetBasis::Most );
		n1set = number1.getStdBitsetValue( );

		sw.start( );
		CStdBitsetUnsignedOperation::DivisionWithRemainder( n1set, tenset );
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

	res.elapsedTimeDivisionWithRemainder = sw.get( );

	printf( "\nFinal Elapsed Time : %s  (%s us)\n\n", sw.getMicrosecondsString( format_type ).c_str( ), sw.getMicrosecondsString( no_format ).c_str( ) );

	return res;
}


template <size_t BitSize> void Div10SpeedTestSample( void ) {

	CStdBitsetUnsignedOperation::StdBitset<BitSize> number1, ten( 10 );

	const size_t loopCount = 512;

	number1 = CStdBitsetUnsignedOperation::Random<BitSize>( );
	number1[BitSize - 1] = true;

	uint64_t processPermille = 0, prevProcessPermille = 0;

	printf( "# Division10WithRemainder Speed Test Sample (%zu ビット , %zu 回)\n\n", BitSize, loopCount );

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

template <size_t BitSize> void Div10SpeedTestSample_01(void ) {

	CStdBitsetUnsignedOperation::StdBitset<BitSize> number1, ten( 10 );

	const size_t loopCount = 512;

	uint64_t processPermille = 0, prevProcessPermille = 0;

	printf("# Division10WithRemainder Speed Test Sample (%zu ビット , %zu 回)\n\n", BitSize, loopCount );

	CStdChronoBasedStopWatchA  sw;

	for ( size_t i = 0; i < loopCount; i++ ) {
		
		number1 = CStdBitsetUnsignedOperation::Random<BitSize>( );
		number1[BitSize - 1] = true;

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
	sw.reset( );

	for ( size_t i = 0; i < loopCount; i++ ) {

		number1 = CStdBitsetUnsignedOperation::Random<BitSize>( );
		number1[BitSize - 1] = true;

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

void SpeedTestEntry( ) {


	std::vector<SpeedTestResult> results;

	size_t loopCount = 512;

	results.push_back( runSpeedTest<1024>( loopCount ) );

	results.push_back( runSpeedTest<2048>( loopCount ) );
	results.push_back( runSpeedTest<4096>( loopCount ) );
	results.push_back( runSpeedTest<8192>( loopCount ) );
	results.push_back( runSpeedTest<16384>( loopCount ) );
	results.push_back( runSpeedTest<32768>( loopCount ) );

	printf( "\n\n" );

	printf( "Speed Test Results:\n" ); 

	printf( "| BitSize | LoopCount | ElapsedTimeDivision10WithRemainder | ElapsedTimeDivisionWithRemainder |\n" );
	printf( "|---------|-----------|-----------------------------------|---------------------------------|\n" ); ;
	for ( auto& res : results ) {
		printf( "| %zu | %zu | %s | %s |\n", res.bitSize, res.loopCount,
			CStdChronoBasedStopWatchA::CreateMicrosecondsString( res.elapsedTimeDivision10WithRemainder, StdChronoBasedStopWatchStringFormat::HHMMSSWithFractionalSeconds ).c_str( ),
			CStdChronoBasedStopWatchA::CreateMicrosecondsString( res.elapsedTimeDivisionWithRemainder, StdChronoBasedStopWatchStringFormat::HHMMSSWithFractionalSeconds ).c_str( ) );
	}
	printf( "\n\n" ); 
}

int main( ) {

	// UTF-8コードページに変更
	SetConsoleOutputCP( CP_UTF8 );

	// 日本語ロケールに設定
	std::setlocale( LC_ALL, "ja-JP.UTF-8" );



	//SpeedTestEntry( );

	/*
	CStdBitsetUnsignedNumberA<32> num32( 1234567890 );

	num32.toCharCast<wchar_t>( ).toHexadecimalString( );

	CStdBitsetUnsignedStringConversion<wchar_t>::ToHexadecimalString( num32.getStdBitsetRefConst( ) );


	  Div10SpeedTestSample<1024>( );
	  */


	auto ret = CStdBitsetUnsignedStringConversion<char>::FromHexadecimalStringStrict<10>( "323",
		CBitsetStringConvSupport::OperationForInvalidCharDetected::PartialReturn );

	uint32_t value = ret.value.to_ulong( );
	printf("%u %x\n", value, value );

	/*
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
	*/
	return 0;
}
