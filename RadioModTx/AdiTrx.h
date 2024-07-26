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
AdiTrx.h

This file contains the definitions for ADI transceivers.
*/

#ifndef AdiTrx_h
#define AdiTrx_h

#include "Dataset.h"

#include <iio.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>


//************************************************************************
// Class for handling Analog Devices transceivers
//************************************************************************
class AdiTrx
{
    //************************************************************************
    // constants and types
    //************************************************************************
    public:
        typedef struct
        {
            double min;                //!< lower limit
            double step;               //!< step
            double max;                //!< upper limit
        }DoubleRange;

        typedef struct
        {
            int64_t min;                //!< lower limit
            int64_t step;               //!< step
            int64_t max;                //!< upper limit
        }IntegerRange;


    //************************************************************************
    // functions
    //************************************************************************
    public:
        AdiTrx();

        ~AdiTrx();

        void freeResources();

        void getDumpFilename
            (
            std::string&                aFilename   //!< filename
            );

        void getSignalData
            (
            Dataset::SignalData&        aSignalData //!< signal data for a modulation-SNR combination
            );

    protected:
        bool extractDouble
            (
            const std::string           aString,    //!< string
            const size_t                aIndex,     //!< substring index
            double&                     aValue      //!< value
            );

        bool extractDoubleLimit
            (
            std::string                 aString,    //!< string
            double&                     aValue      //!< value
            );

        bool extractInteger
            (
            const std::string           aString,    //!< string
            const size_t                aIndex,     //!< substring index
            int64_t&                    aValue      //!< value
            );

        bool extractIntegerRange
            (
            std::string                 aString,    //!< string
            AdiTrx::IntegerRange&       aRange      //!< range
            );

        virtual bool getTxBandwidth
            (
            int64_t& aBandwidth         //!< bandwidth [Hz]
            ) = 0;

        virtual bool getTxBandwidthParams() = 0;

        virtual IntegerRange getTxBandwidthRange() const = 0;

        virtual bool getTxHwGain
            (
            double& aHwGainDb           //!< hardware gain [dB]
            ) = 0;

        virtual bool getTxHwGainParams() = 0;

        virtual bool getTxLoFrequency
            (
            int64_t& aFrequency         //!< frequency [Hz]
            ) = 0;

        virtual bool getTxLoFrequencyParams() = 0;

        virtual IntegerRange getTxLoFrequencyRange() const = 0;

        virtual bool getTxLoPower
            (
            bool& aEnable               //!< true if power is enabled
            ) const = 0;

        virtual bool getTxNcoGainScale
            (
            double& aGainScale          //!< gain scale [0..1]
            ) = 0;

        virtual bool getTxSamplingFrequency
            (
            int64_t& aFrequency         //!< sampling frequency [Hz]
            ) = 0;

        virtual bool getTxSamplingFrequencyParams() = 0;

        virtual IntegerRange getTxSamplingFrequencyRange() const = 0;

        virtual bool initialize
            (
            const std::string aUri      //!< URI
            ) = 0;


        bool isInitialized() const;

        bool readRegister
            (
            const uint16_t aAddress,    //!< register address
            uint8_t&       aValue       //!< read value
            );

        bool resetTxBuffer
            (
            const size_t aLength,       //!< new buffer length
            const bool   aIsCyclic      //!< true if cyclic
            );


        virtual bool setTxBandwidth
            (
            const int64_t aBandwidth    //!< bandwidth [Hz]
            ) = 0;

        virtual bool setTxHwGain
            (
            const double& aHwGainDb     //!< hardware gain [dB]
            ) = 0;

        virtual bool setTxLoFrequency
            (
            const int64_t aFrequency    //!< frequency [Hz]
            ) = 0;

        virtual bool setTxLoPower
            (
            const bool aEnable          //!< true for enabling power
            ) = 0;

        virtual bool setTxNcoGainScale
            (
            const double aGainScale     //!< gain scale [0..1]
            ) = 0;

        virtual bool setTxSamplingFrequency
            (
            const int64_t aFrequency    //!< frequency [Hz]
            ) = 0;

        virtual void startTxStreaming() = 0;

        virtual void stopTxStreaming() = 0;

        bool writeRegister
            (
            const uint16_t aAddress,    //!< register address
            const uint8_t  aValue       //!< value to write
            );


    //************************************************************************
    // variables
    //************************************************************************
    protected:
        struct iio_context*     mIioContext;                //!< IIO context

        struct iio_device*      mPhyDev;                    //!< phy device
        struct iio_device*      mTxDev;                     //!< Tx streaming device
        struct iio_device*      mRxDev;                     //!< Rx streaming device

        struct iio_channel*     mTxPhyChan;                 //!< Tx phy channel
        struct iio_channel*     mTxLoChan;                  //!< Tx LO channel
        struct iio_channel*     mTxNcoChan;                 //!< Tx NCO channel

        struct iio_channel*     mTx0_I;                     //!< Tx I channel
        struct iio_channel*     mTx0_Q;                     //!< Tx Q channel

        struct iio_buffer*      mTxBuf;                     //!< Tx data buffer
        size_t                  mTxBufIqPairsCount;         //!< number of (I,Q) pairs in Tx buffer

        std::vector<std::string> mTxPortSelectVec;          //!< vector with Tx selected ports

        IntegerRange            mTxBandwidthParams;         //!< Tx bandwidth parameters
        int64_t                 mTxBandwidth;               //!< Tx bandwidth [Hz]

        IntegerRange            mTxSamplingFrequencyParams; //!< Tx sampling frequency parameters
        int64_t                 mTxSamplingFrequency;       //!< Tx sampling frequency [Hz]

        IntegerRange            mTxLoFrequencyParams;       //!< Tx LO frequency parameters
        int64_t                 mTxLoFrequency;             //!< Tx LO frequency [Hz]

        DoubleRange             mTxHwGainDbParams;          //!< Tx hardware gain parameters
        double                  mTxHwGainDb;                //!< Tx hardware gain [dB]

        double                  mTxNcoGainScale;            //!< Tx NCO gain scale [0..1]

        bool                    mInitialized;               //!< initialization status

        std::vector<int16_t>    mTxDataVec;                 //!< vector with Tx data

        Dataset::SignalData     mSignalData;                //!< signal data for a modulation-SNR combination
        uint16_t                mFrameLength;               //!< frame length in (I,Q) pairs
        uint16_t                mFramesNr;                  //!< frames count per modulation-SNR combination

        std::string             mDumpFilename;              //!< name of file where to dump data
};

#endif // AdiTrx_h
