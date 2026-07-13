
/*
以下にまとめた内容をベースに操作関連のクラス実装化 + 追加独自実装あり
https://gist.github.com/hirosof/2dad279fc120d476a7079506cfab2572

開発環境：Visual Studio 2026
*/

#pragma once
#include <bitset>
#include <optional>
#include <algorithm>
#include <utility>
#include <random>

class CStdBitsetUnsignedOperation {
public:
	template<size_t BitSize> using StdBitset = std::bitset<BitSize>;
	template<size_t BitSize> using StdBitsetPointer = std::bitset<BitSize>*;
	template<size_t BitSize> using StdBitsetConst = const StdBitset<BitSize>;
	template<size_t BitSize> using StdBitsetConstRef = const StdBitset<BitSize>&;

	template <size_t BitSize> using StdBitsetPair = std::pair<std::bitset<BitSize>, std::bitset<BitSize>>;
	template <size_t BitSize> using OptionalStdBitset = std::optional<StdBitset<BitSize>>;
	template <size_t BitSize> using OptionalStdBitsetPair = std::optional<StdBitsetPair<BitSize>>;

	enum struct CompareResult {
		LeftGreater = 0,
		Equal,
		RightGreater
	};

	template< size_t ToBitSize, size_t FromBitSize>  static StdBitset<ToBitSize> CastSize( StdBitsetConstRef<FromBitSize> input ) {
		static_assert( FromBitSize > 0, "FromBitSizeは無効な値です。" );
		static_assert( ToBitSize > 0, "ToBitSizeは無効な値です。" );

		StdBitset<ToBitSize> target;

		size_t copySize = std::min( FromBitSize, ToBitSize );
		for ( size_t i = 0; i < copySize; i++ ) {
			target[i] = input[i];
		}

		return target;
	}

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

	template<size_t BitSize>  static StdBitset<BitSize> Decrement( StdBitsetConstRef<BitSize> input ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );


		// このクラスは、1減算するのではなく、全ビットに1を加算する方法でデクリメントを実装している
		// 全ビットに1を加算するということは、1の2の補数(-1) を加算することと同義となり、1減算を実装できる

		StdBitset<BitSize>  ox( input );

		bool A, X, Cn, Cb = false;

		for ( size_t i = 0; i < BitSize; i++ ) {

			A = ox[i];

			X = !( A ^ Cb );
			Cn = A || Cb;

			ox[i] = X;

			Cb = Cn;
		}

		return ox;
	}

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

	template<size_t BitSize>  static StdBitset<BitSize> Multiplication10( StdBitsetConstRef<BitSize> input ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		
		// input == 0  の時、 input * 10 = 0
		if ( input.none( ) ) {
			//inputはすでに0のため、inputを返せば0を返すことになる
			return input;
		}

		// input * 10 =  input * 8 + input * 2 = (input << 3 ) + (input << 1)
		return Addition( input << 3, input << 1 );
	}

	template<size_t BitSize>  static OptionalStdBitset<BitSize> Division( StdBitsetConstRef<BitSize> input_a, StdBitsetConstRef<BitSize> input_b ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		OptionalStdBitsetPair<BitSize> dwr = DivisionWithRemainder( input_a, input_b );

		if ( dwr.has_value( ) ) {
			return dwr->first;
		}

		return std::nullopt;
	}

	template<size_t BitSize>  static OptionalStdBitset<BitSize> Remainder( StdBitsetConstRef<BitSize> input_a, StdBitsetConstRef<BitSize> input_b ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		OptionalStdBitsetPair<BitSize> dwr = DivisionWithRemainder( input_a, input_b );

		if ( dwr.has_value( ) ) {
			return dwr->second;
		}

		return std::nullopt;
	}

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

		const size_t digit_ia = GetNumberOfDigitsForDisplay( input_a );
		const size_t digit_ib = GetNumberOfDigitsForDisplay( input_b );
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


	template<size_t BitSize>  static size_t GetNumberOfDigitsForDisplay( StdBitsetConstRef<BitSize> input ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		for ( size_t i = BitSize - 1; i > 0; i-- ) {
			if ( input[i] ) return i + 1;
		}

		// ループが最後まで回ってもreturnされず、ここに来るケースは
		// 最下位ビットの値が0(false)もしくは1(true)で、かつそれ以外のビットは全て0(false)の場合のみである
		// 0 (false) は 1桁もないのではなく 0 という1桁の数値が有るものと見なす。
		return 1;
	}


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




	template<size_t BitSize>  static  StdBitset<BitSize> Max( StdBitsetConstRef<BitSize> input_left, StdBitsetConstRef<BitSize> input_right ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );
		return ( Compare( input_left, input_right ) == CompareResult::LeftGreater ) ? input_left : input_right;
	}

	template<size_t BitSize>  static  StdBitset<BitSize> Min( StdBitsetConstRef<BitSize> input_left, StdBitsetConstRef<BitSize> input_right ) {
		static_assert( BitSize > 0, "BitSizeは無効な値です。" );

		return ( Compare( input_left, input_right ) != CompareResult::LeftGreater ) ? input_left : input_right;
	}


	template<size_t BitSize>  static  StdBitset<BitSize> Random( size_t fill_bit_size = BitSize ) {

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


};

