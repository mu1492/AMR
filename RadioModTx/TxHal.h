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
TxHal.h

This file contains the definitions for transmitter HAL.
*/

#ifndef TxHal_h
#define TxHal_h

#include "AdiTrxAd9361.h"
#include "AdiTrxAdrv9009.h"
#include "AdiTrxAd9081.h"

#include <iio.h>

#include <cstdint>
#include <map>
#include <string>
#include <vector>


//************************************************************************
// Class for handling the transmitter Hardware Abstraction Layer (HAL)
//************************************************************************
class TxHal
{
    //************************************************************************
    // constants and types
    //************************************************************************
    public:
        typedef enum : uint8_t
        {
            TX_DEVICE_UNKNOWN,

            TX_DEVICE_AD9361,
            TX_DEVICE_AD9081,
            TX_DEVICE_ADRV9009
        }TxDevice;

        static const std::map<TxDevice, std::vector<std::string>> TX_DEVICE_NAME_IDS;

        typedef struct
        {
            std::string uri;
            std::string description;
        }IioScanContext;

    private:
        static const std::string DEFAULT_IP_URI;


    //************************************************************************
    // functions
    //************************************************************************
    public:
        TxHal();

        static TxHal* getInstance();

        void getData
            (
            Dataset::SignalData& aSignalData    //!< signal data for a modulation-SNR combination
            );

        void getDumpFilename
            (
            std::string& aFilename      //!< filename
            );

        std::vector<IioScanContext> getIioScanContexts() const;

        bool getTxBandwidth
            (
            int64_t& aBandwidth         //!< bandwidth [Hz]
            );

        TxDevice getTxDevice() const;

        bool getTxHwGain
            (
            double& aHwGainDb           //!< hardware gain [dB]
            );

        bool getTxLoFrequency
            (
            int64_t& aFrequency         //!< frequency [Hz]
            );

        bool getTxNcoGainScale
            (
            double& aGainScale          //!< gain scale [0..1]
            );

        AdiTrx::IntegerRange getTxLoFrequencyRange() const;

        bool getTxSamplingFrequency
            (
            int64_t& aFrequency         //!< sampling frequency [Hz]
            );

        bool initializeTxDevice
            (
            const int aIndex        //!< index
            );

        bool isInitialized() const;

        bool setTxLoFrequency
            (
            const int64_t aFrequency    //!< frequency [Hz]
            );

        bool setTxNcoGainScale
            (
            const double aGainScale     //!< gain scale [0..1]
            );

        bool setTxSamplingFrequency
            (
            const int64_t aFrequency    //!< frequency [Hz]
            );

        void startStreaming();

        void stopStreaming();

        void updateIioScanContexts();

        void updateSamplingFrequency
            (
            const Dataset::DatasetSource aDatasetSource     //!< dataset
            );

    private:
        bool isAllowedContext
            (
            const std::string aUri  //!< URI
            ) const;


    //************************************************************************
    // variables
    //************************************************************************
    private:
        static TxHal*               sInstance;              //!< singleton

        std::vector<IioScanContext> mIioScanContextsVec;    //!< vector with IIO scan contexts
        size_t                      mIioScanContextsCount;  //!< number of scan contextst

        TxDevice                    mTxDevice;              //!< selected device for transmit
        bool                        mIsInitialized;         //!< true if Tx devices initialized

        AdiTrxAd9361                mTrxAd9361;             //!< AD9361 transceiver
        AdiTrxAdrv9009              mTrxAdrv9009;           //!< ADRV9009 transceiver
        AdiTrxAd9081                mTrxAd9081;             //!< AD9081 transceiver
};

#endif // TxHal_h
