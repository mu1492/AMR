///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2024 Mihai Ursu                                                 //
//                                                                               //
// This program is free software; you can redistribute it and/or modify          //
// it under the terms of the GNU General Public License as published by          //
// the Free Software Foundation as version 3 of the License, or                  //
// (at your option) any later version.                                           //
//                                                                               //
// This program is distributed in the hope that it will be useful,               //
// but WITHOUT ANY WARRANTY; without even the implied warranty of                //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  //
// GNU General Public License V3 for more details.                               //
//                                                                               //
// You should have received a copy of the GNU General Public License             //
// along with this program. If not, see <http://www.gnu.org/licenses/>.          //
///////////////////////////////////////////////////////////////////////////////////

/*
AdiTrxAdrv9009.h

This file contains the definitions for the ADRV9009 transceiver.
*/

#ifndef AdiTrxAdrv9009_h
#define AdiTrxAdrv9009_h

#include "AdiTrx.h"


//************************************************************************
// Class for handling the ADRV9009 transceiver
//************************************************************************
class AdiTrxAdrv9009 : public AdiTrx
{
    //************************************************************************
    // constants and types
    //************************************************************************
    private:
        const std::string ADRV9009_PHY_DEV_STR = "adrv9009-phy";        //!< ADRV9009 phy device string
        const std::string ADRV9009_TX_DEV_STR = "axi-adrv9009-tx-hpc";  //!< ADRV9009 Tx device string


    //************************************************************************
    // functions
    //************************************************************************
    public:
        AdiTrxAdrv9009();

        bool getTxBandwidth
            (
            int64_t& aBandwidth         //!< bandwidth [Hz]
            );

        bool getTxBandwidthParams();

        IntegerRange getTxBandwidthRange() const;

        bool getTxHwGain
            (
            double& aHwGainDb           //!< hardware gain [dB]
            );

        bool getTxHwGainParams();

        bool getTxLoFrequency
            (
            int64_t& aFrequency         //!< frequency [Hz]
            );

        bool getTxLoFrequencyParams();

        IntegerRange getTxLoFrequencyRange() const;

        bool getTxLoPower
            (
            bool& aEnable               //!< true if power is enabled
            ) const;

        bool getTxNcoGainScale
            (
            double& aGainScale          //!< gain scale [0..1]
            );

        bool getTxSamplingFrequency
            (
            int64_t& aFrequency         //!< sampling frequency [Hz]
            );

        bool getTxSamplingFrequencyParams();

        IntegerRange getTxSamplingFrequencyRange() const;

        bool initialize
            (
            const std::string aUri      //!< URI
            );

        bool setTxBandwidth
            (
            const int64_t aBandwidth    //!< bandwidth [Hz]
            );

        bool setTxHwGain
            (
            const double& aHwGainDb     //!< hardware gain [dB]
            );

        bool setTxLoFrequency
            (
            const int64_t aFrequency    //!< frequency [Hz]
            );

        bool setTxLoPower
            (
            const bool aEnable          //!< true for enabling power
            );

        bool setTxNcoGainScale
            (
            const double aGainScale     //!< gain scale [0..1]
            );

        bool setTxSamplingFrequency
            (
            const int64_t aFrequency    //!< frequency [Hz]
            );

        void startTxStreaming();

        void stopTxStreaming();
};

#endif // AdiTrxAdrv9009_h
