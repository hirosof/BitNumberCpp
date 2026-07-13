#pragma once

#include <map>
#include <vector>

class CBitsetStringConvSupport {
public:

	// From系の関数で無効文字検出時の動作モード
	enum struct OperationForInvalidCharDetected {

		// 部分パース(成功している部分までの値)の結果を返す
		// 例：10進数文字列として "1234X5678"が指定された場合 → 1234 を表す値が返る
		PartialReturn = 0,

		// 0を表す値を返す
		ZeroValueReturn,

		// '0' が指定したものと解釈し、処理を続行する
		// 例：10進数文字列として "1234X5678"が指定された場合 → 123405678 を表す値が返る
		AssumeZeroContinue,

		// その値を無視して処理を続行する
		// 例：10進数文字列として "1234X5678"が指定された場合 → 12345678 を表す値が返る
		SkipContinue
	};

	class ProcessStringLengthInfo {
	public:
		size_t specified;		//指定された文字列の総文字数
		size_t processed;		// 処理した文字数

		ProcessStringLengthInfo( ) : specified( 0 ), processed( 0 ) {}
	};


	template <typename CharT>  using InvalidCharMapType = std::map<CharT, std::vector<size_t>>;

	template <typename CharT>  class ParseProcessedInfo {
	public:
		size_t countOfInvalidChars;

		InvalidCharMapType<CharT> invalidCharMap;

		ProcessStringLengthInfo processLength;

		ParseProcessedInfo( ) : countOfInvalidChars( 0 ), invalidCharMap( ), processLength( ) {}
	};


};
