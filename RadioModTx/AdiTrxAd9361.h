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
AdiTrxAd9361.h

This file contains the definitions for AD9361/AD9363/AD9364 transceivers.
*/

#ifndef AdiTrxAd9361_h
#define AdiTrxAd9361_h

#include "AdiTrx.h"


//************************************************************************
// Class for handling AD9361/AD9363/AD9364 transceivers
//************************************************************************
class AdiTrxAd9361 : public AdiTrx
{
    //************************************************************************
    // constants and types
    //************************************************************************
    private:
        const std::string AD9361_PHY_DEV_STR = "ad9361-phy";            //!< AD9361 phy device string
        const std::string AD9361_TX_DEV_STR = "cf-ad9361-dds-core-lpc"; //!< AD9361 Tx device string


    //************************************************************************
    // functions
    //************************************************************************
    public:
        AdiTrxAd9361();

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

#endif // AdiTrxAd9361_h
