/**

    @file      CBitsetStringConvSupport.hpp
    @brief     ビットセット文字列変換サポート
    @author    hirosof
    @copyright (C) 2026 hirosof.

**/
#pragma once

#include <map>
#include <set>


/**
    @class   CBitsetStringConvSupport
	@brief   ビットセット文字列変換サポートクラス
**/
class CBitsetStringConvSupport {
public:


	/**
		@brief 無効文字検出時の動作モード (主に、From系の関数で使用される)
	**/
	enum struct OperationForInvalidCharDetected {



		/**
		 @brief 部分パース(成功している部分までの値)の結果を返す
		 @details 例：10進数文字列として "1234X5678"が指定された場合 → 1234 を表す値が返る
		*/
		PartialReturn = 0,

		/**
		 @brief 0を表す値を返す
		*/
		ZeroValueReturn,

		/**
		 @brief '0' が指定したものと解釈し、処理を続行する
		 @details 例：10進数文字列として "1234X5678"が指定された場合 → 123405678 を表す値が返る
		*/
		AssumeZeroContinue,

		/**
		 @brief その値を無視して処理を続行する
		 @details 例：10進数文字列として "1234X5678"が指定された場合 → 12345678 を表す値が返る
		*/
		SkipContinue
	};


	/**
		@class   ProcessStringLengthInfo
		@brief   文字列の処理長情報を保持するクラス
	**/
	class ProcessStringLengthInfo {
	public:
		/**
		 @brief 指定された文字列の総文字数
		*/
		size_t specified;

		/**
		 @brief 処理した文字数
		*/
		size_t processed;

		/**
		 * @brief コンストラクタ　(specified と processed を 0 に初期化)
		 */
		ProcessStringLengthInfo( ) : specified( 0 ), processed( 0 ) {}
	};


	/**
	 * @brief 無効文字マップの型定義
	 * @tparam CharT - 文字型
	 * @details std::mapのキーは無効文字、値はその文字が出現したインデックス(0ベース)の集合(std::set)です。
	 */
	template <typename CharT>  using InvalidCharMapType = std::map<CharT, std::set<size_t>>;


	/**
		@class   ParseProcessedInfo
		@brief   パースの処理情報と無効文字の解析結果を保持するクラス
		@tparam  CharT - 文字型
	**/
	template <typename CharT>  class ParseProcessedInfo {
	public:

		/**
		*	 @brief 無効文字の総数
		*   @warning この値は、無効文字が出現した回数の総和であり、無効文字の種類の数ではありません。
		*/
		size_t countOfInvalidChars;

		/**
		 * @brief 無効文字マップ
		 */
		InvalidCharMapType<CharT> invalidCharMap;

		/**
		 * @brief 文字列の処理長情報
		 */
		ProcessStringLengthInfo processLength;

		/**
		 * @brief コンストラクタ　(countOfInvalidChars を 0 に初期化、invalidCharMap と processLength はデフォルト値に初期化)
		 */
		ParseProcessedInfo( ) : countOfInvalidChars( 0 ), invalidCharMap( ), processLength( ) {}
	};


	/**
		@brief ゼロパディングモード
	**/
	enum struct ZeroPaddingMode {

		/**
			パディングなし
		*/
		NoPadding = 0,

		/**
			コンテナのビット数に合わせてパディング
		*/
		ContainerBitsPadding,

		/**
			8ビット単位に合わせてパディング
		*/
		EightBitsPadding,

		/**
			コンテナのビット数と8ビット単位の両方に合わせてパディング
		*/
		ContainerAndEightBitsPadding

	};

	/**

		@class   ParsedData
		@brief   パースされたデータを保持するクラス
		@tparam  ValueType - 値の型
		@tparam  CharT     - 文字型

	**/
	template<typename ValueType, typename CharT> class ParsedData {
	public:

		/**
		 * @brief パースされた値
		 */
		ValueType  value;

		/**
		 * @brief パースの処理情報と無効文字の解析結果
		 */
		ParseProcessedInfo<CharT> info;

		/**
			@brief コンストラクタ (value は ValueType のデフォルト値、info は ParseProcessedInfo<CharT> のデフォルト値に初期化されます)
		**/
		ParsedData( ) : value( ), info( ) {}
	};


};
