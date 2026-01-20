#pragma once
#include <vector>
#include <map>
#include <cstdint>


namespace Gecko::Compression
{
	/*
	 *		Huffman encoding prefix lookup.
	 *		Code words obtained from: https://www.itu.int/rec/T-REC-T.6/en
	 */
	class CodeWords
	{
	public:
		enum class IntegralPrefixType : uint8_t
		{
			MakeUp,
			Terminating
		};

		enum class ModePrefixType : int8_t
		{
			Vertical0 = 0,
			VerticalR1 = 1, VerticalR2 = 2, VerticalR3 = 3,
			VerticalL1 = -1, VerticalL2 = -2, VerticalL3 = -3,
			Pass = 4, Horizontal = 5
		};

		struct IntegralPrefix { uint16_t prefixBits; uint16_t prefixLengthBits; uint16_t integral; IntegralPrefixType type; };
		struct ModePrefix { uint16_t prefixBits; uint16_t prefixLengthBits; ModePrefixType type; };

		/*
		 *		Look up CCITT G4 black/white codewords (prefixes) based on the low 13 bits of a uint16_t.
		 *		All bits except the low 13 are ignored.
		 *
		 *		Example:
		 *			lookup        : 0b1011101001
		 *			prefix result : 0b10111xxxxx (prefix length: 5)
		 *
		 *		Consumers of the API can then consume then next <prefix length> bits from the stream
		 *		without losing any data.
		 */
		static IntegralPrefix& LookupBlackIntegralPrefixFromLow13(uint16_t low13);
		static IntegralPrefix& LookupWhiteIntegralPrefixFromLow13(uint16_t low13);

		/*
		 *		Large integrals (> 64) may not be encoded within a single prefix.
		 *		In these cases, a makeup prefix is used (basically says "keep going").
		 *		
		 *		See the ITU paper linked above.
		 */
		static IntegralPrefix& GetNextBlackIntegralPrefix(uint16_t integralValue);
		static IntegralPrefix& GetNextWhiteIntegralPrefix(uint16_t integralValue);

		/*
		 *		The longest mode prefix is only 7 bits long so we only use the low
		 *		7 bits here.
		 */
		static ModePrefix& LookupModePrefixFromLow7(uint16_t low7);
		static ModePrefix& LookupModePrefixFromType(ModePrefixType type);

		static inline int IntegralDifferenceFromVerticalModePrefixType(ModePrefixType type) { return static_cast<int>(type); }

	private:
		static IntegralPrefix& GetNextIntegralPrefix(std::map<int, IntegralPrefix&>& prefixes, uint16_t integralValue);
		static void StoreIntegralEncoding(std::vector<IntegralPrefix>& lookup, std::map<int, IntegralPrefix&>& prefixMap, uint16_t prefix, uint16_t prefixLengthBits, uint16_t integral, IntegralPrefixType type);
		static void StoreModeEncoding(std::vector<ModePrefixType>& modes, std::map<ModePrefixType, ModePrefix>& prefixMap, uint16_t prefix, uint16_t prefixLengthBits, ModePrefixType type);

		static std::vector<IntegralPrefix> BuildIntegralsBlackLookup();
		static std::vector<IntegralPrefix> BuildIntegralsWhiteLookup();
		static std::vector<IntegralPrefix> integralsBlack;
		static std::vector<IntegralPrefix> integralsWhite;

		static std::map<int, IntegralPrefix&> integralPrefixesBlack;
		static std::map<int, IntegralPrefix&> integralPrefixesWhite;

		static std::vector<ModePrefixType> BuildModesLookup();
		static std::vector<ModePrefixType> modes;
		static std::map<ModePrefixType, ModePrefix> modePrefixes;
	};
}