#pragma once
#include "gecko/BitStream.h"

namespace Gecko::Compression
{
    struct PackedData
    {
      public:
        PackedImage(BitStream bs) 
            : m_bs(bs) {}

        inline std::vector<uint8_t> Release()
        {
            return m_bs.Release();
        }

      private:
        BitStream m_bs;
    }
}