#include "gecko/CodeWords.h"
#include <stdexcept>
#include <iostream>
#include <optional>

namespace Gecko::Compression
{
	std::map<int, CodeWords::IntegralPrefix&> CodeWords::integralPrefixesWhite = std::map<int, CodeWords::IntegralPrefix&>{};
	std::map<int, CodeWords::IntegralPrefix&> CodeWords::integralPrefixesBlack = std::map<int, CodeWords::IntegralPrefix&>{};

	/*
	 *		I really don't like how this works at the moment. Basically, this is a hack to build the lookup
	 *		tables pre-main(). `CodeWords::BuildIntegralsBlack/WhiteLookup()` has the side effect of also
	 *		modifying `CodeWords::integralPrefixesWhite/Black`, which is not ideal.
	 *
	 *		However, this class is self-contained and not particularly complicated so it's low priority
	 * 		for now.
	 */
	std::vector<CodeWords::IntegralPrefix> CodeWords::integralsBlack = CodeWords::BuildIntegralsBlackLookup();
	std::vector<CodeWords::IntegralPrefix> CodeWords::integralsWhite = CodeWords::BuildIntegralsWhiteLookup();

	std::map<CodeWords::ModePrefixType, CodeWords::ModePrefix> CodeWords::modePrefixes = std::map<CodeWords::ModePrefixType, ModePrefix>{};
	std::vector<CodeWords::ModePrefixType> CodeWords::modes = CodeWords::BuildModesLookup();


	CodeWords::IntegralPrefix& CodeWords::LookupBlackIntegralPrefixFromLow13(uint16_t low13)
	{
		return integralsBlack[low13 & ((1 << 13) - 1)];
	}


	CodeWords::IntegralPrefix& CodeWords::LookupWhiteIntegralPrefixFromLow13(uint16_t low13)
	{
		return integralsWhite[low13 & ((1 << 13) - 1)];
	}


	CodeWords::IntegralPrefix& CodeWords::GetNextBlackIntegralPrefix(uint16_t integralValue)
	{
		return GetNextIntegralPrefix(integralPrefixesBlack, integralValue);
	}


	CodeWords::IntegralPrefix& CodeWords::GetNextWhiteIntegralPrefix(uint16_t integralValue)
	{
		return GetNextIntegralPrefix(integralPrefixesWhite, integralValue);
	}


	CodeWords::IntegralPrefix& CodeWords::GetNextIntegralPrefix(std::map<int, IntegralPrefix&>& prefixes, uint16_t integralValue)
	{
		auto it = prefixes.upper_bound(integralValue);
		--it;
		return it->second;
	}


	CodeWords::ModePrefix& CodeWords::LookupModePrefixFromLow7(uint16_t low7)
	{
		return modePrefixes.at(modes[low7]);
	}


	CodeWords::ModePrefix& CodeWords::LookupModePrefixFromType(ModePrefixType type)
	{
		return modePrefixes.at(type);
	}


	std::vector<CodeWords::IntegralPrefix> CodeWords::BuildIntegralsBlackLookup()
	{
		std::vector<IntegralPrefix> lookup(1 << 13);

		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000110111,   10, 0,  IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b010,          3,  1,  IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b11,           2,  2,  IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b10,           2,  3,  IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b011,          3,  4,  IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0011,         4,  5,  IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0010,         4,  6,  IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b00011,        5,  7,  IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000101,       6,  8,  IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000100,       6,  9,  IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000100,      7,  10, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000101,      7,  11, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000111,      7,  12, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b00000100,     8,  13, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b00000111,     8,  14, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000011000,    9,  15, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000010111,   10, 16, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000011000,   10, 17, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000001000,   10, 18, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b00001100111,  11, 19, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b00001101000,  11, 20, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b00001101100,  11, 21, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b00000110111,  11, 22, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b00000101000,  11, 23, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b00000010111,  11, 24, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b00000011000,  11, 25, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000011001010, 12, 26, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000011001011, 12, 27, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000011001100, 12, 28, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000011001101, 12, 29, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000001101000, 12, 30, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000001101001, 12, 31, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000001101010, 12, 32, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000001101011, 12, 33, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000011010010, 12, 34, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000011010011, 12, 35, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000011010100, 12, 36, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000011010101, 12, 37, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000011010110, 12, 38, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000011010111, 12, 39, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000001101100, 12, 40, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000001101101, 12, 41, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000011011010, 12, 42, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000011011011, 12, 43, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000001010100, 12, 44, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000001010101, 12, 45, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000001010110, 12, 46, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000001010111, 12, 47, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000001100100, 12, 48, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000001100101, 12, 49, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000001010010, 12, 50, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000001010011, 12, 51, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000000100100, 12, 52, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000000110111, 12, 53, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000000111000, 12, 54, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000000100111, 12, 55, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000000101000, 12, 56, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000001011000, 12, 57, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000001011001, 12, 58, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000000101011, 12, 59, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000000101100, 12, 60, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000001011010, 12, 61, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000001100110, 12, 62, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000001100111, 12, 63, IntegralPrefixType::Terminating);

		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000001111,    10, 64,   IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000011001000,  12, 128,  IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000011001001,  12, 192,  IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000001011011,  12, 256,  IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000000110011,  12, 320,  IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000000110100,  12, 384,  IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000000110101,  12, 448,  IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000001101100, 13, 512,  IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000001101101, 13, 576,  IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000001001010, 13, 640,  IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000001001011, 13, 704,  IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000001001100, 13, 768,  IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000001001101, 13, 832,  IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000001110010, 13, 896,  IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000001110011, 13, 960,  IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000001110100, 13, 1024, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000001110101, 13, 1088, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000001110110, 13, 1152, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000001110111, 13, 1216, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000001010010, 13, 1280, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000001010011, 13, 1344, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000001010100, 13, 1408, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000001010101, 13, 1472, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000001011010, 13, 1536, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000001011011, 13, 1600, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000001100100, 13, 1664, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b0000001100101, 13, 1728, IntegralPrefixType::MakeUp);

		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b00000001000,  11, 1792, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b00000001100,  11, 1856, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b00000001101,  11, 1920, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000000010010, 12, 1984, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000000010011, 12, 2048, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000000010100, 12, 2112, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000000010101, 12, 2176, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000000010110, 12, 2240, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000000010111, 12, 2304, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000000011100, 12, 2368, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000000011101, 12, 2432, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000000011110, 12, 2496, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesBlack, 0b000000011111, 12, 2560, IntegralPrefixType::MakeUp);

		return lookup;
	}


	std::vector<CodeWords::IntegralPrefix> CodeWords::BuildIntegralsWhiteLookup()
	{
		std::vector<IntegralPrefix> lookup(1 << 13);

		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00110101,  8, 0,  IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b000111  ,  6, 1,  IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b0111    ,  4, 2,  IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b1000    ,  4, 3,  IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b1011    ,  4, 4,  IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b1100    ,  4, 5,  IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b1110    ,  4, 6,  IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b1111    ,  4, 7,  IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b10011   ,  5, 8,  IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b10100   ,  5, 9,  IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00111   ,  5, 10, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b01000   ,  5, 11, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b001000  ,  6, 12, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b000011  ,  6, 13, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b110100  ,  6, 14, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b110101  ,  6, 15, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b101010  ,  6, 16, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b101011  ,  6, 17, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b0100111 ,  7, 18, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b0001100 ,  7, 19, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b0001000 ,  7, 20, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b0010111 ,  7, 21, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b0000011 ,  7, 22, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b0000100 ,  7, 23, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b0101000 ,  7, 24, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b0101011 ,  7, 25, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b0010011 ,  7, 26, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b0100100 ,  7, 27, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b0011000 ,  7, 28, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00000010,  8, 29, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00000011,  8, 30, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00011010,  8, 31, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00011011,  8, 32, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00010010,  8, 33, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00010011,  8, 34, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00010100,  8, 35, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00010101,  8, 36, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00010110,  8, 37, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00010111,  8, 38, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00101000,  8, 39, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00101001,  8, 40, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00101010,  8, 41, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00101011,  8, 42, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00101100,  8, 43, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00101101,  8, 44, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00000100,  8, 45, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00000101,  8, 46, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00001010,  8, 47, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00001011,  8, 48, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b01010010,  8, 49, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b01010011,  8, 50, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b01010100,  8, 51, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b01010101,  8, 52, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00100100,  8, 53, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00100101,  8, 54, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b01011000,  8, 55, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b01011001,  8, 56, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b01011010,  8, 57, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b01011011,  8, 58, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b01001010,  8, 59, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b01001011,  8, 60, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00110010,  8, 61, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00110011,  8, 62, IntegralPrefixType::Terminating);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00110100,  8, 63, IntegralPrefixType::Terminating);

		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b11011    , 5,   64, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b10010    , 5,  128, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b010111   , 6,  192, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b0110111  , 7,  256, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00110110 , 8,  320, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00110111 , 8,  384, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b01100100 , 8,  448, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b01100101 , 8,  512, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b01101000 , 8,  576, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b01100111 , 8,  640, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b011001100, 9,  704, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b011001101, 9,  768, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b011010010, 9,  832, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b011010011, 9,  896, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b011010100, 9,  960, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b011010101, 9, 1024, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b011010110, 9, 1088, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b011010111, 9, 1152, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b011011000, 9, 1216, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b011011001, 9, 1280, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b011011010, 9, 1344, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b011011011, 9, 1408, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b010011000, 9, 1472, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b010011001, 9, 1536, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b010011010, 9, 1600, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b011000   , 6, 1664, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b010011011, 9, 1728, IntegralPrefixType::MakeUp);

		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00000001000 , 11, 1792, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00000001100 , 11, 1856, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b00000001101 , 11, 1920, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b000000010010, 12, 1984, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b000000010011, 12, 2048, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b000000010100, 12, 2112, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b000000010101, 12, 2176, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b000000010110, 12, 2240, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b000000010111, 12, 2304, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b000000011100, 12, 2368, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b000000011101, 12, 2432, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b000000011110, 12, 2496, IntegralPrefixType::MakeUp);
		StoreIntegralEncoding(lookup, integralPrefixesWhite, 0b000000011111, 12, 2560, IntegralPrefixType::MakeUp);

		return lookup;
	}


	std::vector<CodeWords::ModePrefixType> CodeWords::BuildModesLookup()
	{
		std::vector<CodeWords::ModePrefixType> lookup(1 << 7);
		StoreModeEncoding(lookup, modePrefixes, 0b0001,	   4, ModePrefixType::Pass);
		StoreModeEncoding(lookup, modePrefixes, 0b001,	   3, ModePrefixType::Horizontal);
		StoreModeEncoding(lookup, modePrefixes, 0b1,	   1, ModePrefixType::Vertical0);
		StoreModeEncoding(lookup, modePrefixes, 0b011,	   3, ModePrefixType::VerticalR1);
		StoreModeEncoding(lookup, modePrefixes, 0b000011,  6, ModePrefixType::VerticalR2);
		StoreModeEncoding(lookup, modePrefixes, 0b0000011, 7, ModePrefixType::VerticalR3);
		StoreModeEncoding(lookup, modePrefixes, 0b010,	   3, ModePrefixType::VerticalL1);
		StoreModeEncoding(lookup, modePrefixes, 0b000010,  6, ModePrefixType::VerticalL2);
		StoreModeEncoding(lookup, modePrefixes, 0b0000010, 7, ModePrefixType::VerticalL3);
		return lookup;
	}


	void CodeWords::StoreIntegralEncoding(std::vector<CodeWords::IntegralPrefix>& lookup, std::map<int, IntegralPrefix&>& prefixMap, uint16_t prefix, uint16_t prefixLengthBits, uint16_t integral, IntegralPrefixType type)
	{
		uint16_t remainingBits = 13 - prefixLengthBits;
		uint16_t iterations    = 1 << remainingBits;
		uint16_t shiftedPrefix = prefix;
	
		shiftedPrefix = shiftedPrefix << (13 - prefixLengthBits);
		shiftedPrefix = shiftedPrefix & ~(iterations - 1);

		for (int i = 0; i < iterations; ++i)
		{
			lookup[shiftedPrefix + i].prefixBits	   = prefix & ((1u << prefixLengthBits) - 1u);
			lookup[shiftedPrefix + i].prefixLengthBits = prefixLengthBits;
			lookup[shiftedPrefix + i].integral		   = integral;
			lookup[shiftedPrefix + i].type			   = type;

			prefixMap.insert_or_assign(integral, lookup[shiftedPrefix + i]);
		}
	}


	void CodeWords::StoreModeEncoding(std::vector<ModePrefixType>& lookup, std::map<ModePrefixType, ModePrefix>& prefixMap, uint16_t prefix, uint16_t prefixLengthBits, CodeWords::ModePrefixType type)
	{
		uint16_t remainingBits = 7 - prefixLengthBits;
		uint16_t iterations = 1 << remainingBits;
		uint16_t shiftedPrefix = prefix;

		shiftedPrefix = shiftedPrefix << (7 - prefixLengthBits);
		shiftedPrefix = shiftedPrefix & ~(iterations - 1);

		for (int i = 0; i < iterations; ++i)
		{
			lookup[shiftedPrefix + i] = type;

			auto [iter, succ] = prefixMap.try_emplace(type);

			iter->second.prefixBits = prefix & ((1u << prefixLengthBits) - 1u);
			iter->second.prefixLengthBits = prefixLengthBits;
			iter->second.type = type;
		}
	}
}