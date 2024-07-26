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
AdiTrxAd9081.h

This file contains the definitions for AD9081/AD9082 transceivers.
*/

#ifndef AdiTrxAd9081_h
#define AdiTrxAd9081_h

#include "AdiTrx.h"


//************************************************************************
// Class for handling AD9081/AD9082 transceivers
//************************************************************************
class AdiTrxAd9081 : public AdiTrx
{
    //************************************************************************
    // constants and types
    //************************************************************************
    private:
        const std::string AD9081_TX_DEV_STR = "axi-ad9081-tx-hpc";  //!< AD9081 Tx device string
        const std::string AD9081_RX_DEV_STR = "axi-ad9081-rx-hpc";  //!< AD9081 Rx device string


    //************************************************************************
    // functions
    //************************************************************************
    public:
        AdiTrxAd9081();

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

#endif // AdiTrxAd9081_h
