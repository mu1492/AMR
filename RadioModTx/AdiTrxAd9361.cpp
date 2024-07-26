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
AdiTrxAd9361.cpp

This file contains the sources for AD9361/AD9363/AD9364 transceivers.
*/

#include "AdiTrxAd9361.h"

#include <QtGlobal>

#define DUMP_FRAMES_TO_FILE 0

#include <cmath>
#include <iostream>
#if DUMP_FRAMES_TO_FILE
    #include <cstring>
    #include <fstream>
#endif


//!************************************************************************
//! Constructor
//!************************************************************************
AdiTrxAd9361::AdiTrxAd9361()
{
}


//!************************************************************************
//! Get the Tx bandwidth [Hz]
//!
//! @returns true if the parameter can be read
//!************************************************************************
bool AdiTrxAd9361::getTxBandwidth
    (
    int64_t& aBandwidth         //!< bandwidth [Hz]
    )
{
    mTxBandwidth = 0;
    char rfBandwidthString[256];
    bool status = ( iio_channel_attr_read( mTxPhyChan, "rf_bandwidth", rfBandwidthString, sizeof( rfBandwidthString ) ) > 0 );

    if( status )
    {
        mTxBandwidth = std::stoll( rfBandwidthString );
    }

    aBandwidth = mTxBandwidth;
    return status;
}


//!************************************************************************
//! Get the Tx bandwidth parameters
//!
//! @returns true if the parameters can be read
//!************************************************************************
bool AdiTrxAd9361::getTxBandwidthParams()
{
    char rfBandwidthAvailableString[256];
    bool status = ( iio_channel_attr_read( mTxPhyChan, "rf_bandwidth_available", rfBandwidthAvailableString, sizeof( rfBandwidthAvailableString ) ) > 2 );

    if( status )
    {
        std::string rawStr = rfBandwidthAvailableString;
        status = extractIntegerRange( rawStr, mTxBandwidthParams );
    }

    return status;
}


//!************************************************************************
//! Get the Tx bandwidth range
//!
//! @returns The range for Rx bandwidth
//!************************************************************************
AdiTrx::IntegerRange AdiTrxAd9361::getTxBandwidthRange() const
{
    return mTxBandwidthParams;
}


//!************************************************************************
//! Get the Tx hardware gain [dB]
//!
//! @returns true if the parameter can be read
//!************************************************************************
bool AdiTrxAd9361::getTxHwGain
    (
    double& aHwGainDb       //!< hardware gain [dB]
    )
{
    mTxHwGainDb = 0;
    char hwGainString[256];
    bool status = ( iio_channel_attr_read( mTxPhyChan, "hardwaregain", hwGainString, sizeof( hwGainString ) ) > 0 );

    if( status )
    {
        std::string rawStr = hwGainString;
        status = extractDouble( rawStr, 0, mTxHwGainDb );
    }

    aHwGainDb = mTxHwGainDb;
    return status;
}


//!************************************************************************
//! Get the Tx hardware gain parameters
//!
//! @returns true if the parameters can be read
//!************************************************************************
bool AdiTrxAd9361::getTxHwGainParams()
{
    mTxHwGainDbParams.max = 0;
    mTxHwGainDbParams.step = 0.25;

    char hwGainAvailableString[256];
    bool status = ( iio_channel_attr_read( mTxPhyChan, "hardwaregain_available", hwGainAvailableString, sizeof( hwGainAvailableString ) ) > 0 );

    if( status )
    {
        std::string rawStr = hwGainAvailableString;

        double minGainDb = mTxHwGainDbParams.max;
        status = extractDoubleLimit( rawStr, minGainDb );

        if( status )
        {
            mTxHwGainDbParams.min = minGainDb;
        }
    }

    return status;
}


//!************************************************************************
//! Get the Tx LO frequency [Hz]
//!
//! @returns true if the parameter can be read
//!************************************************************************
bool AdiTrxAd9361::getTxLoFrequency
    (
    int64_t& aFrequency         //!< frequency [Hz]
    )
{
    mTxLoFrequency = 0;
    char freqString[256];
    bool status = ( iio_channel_attr_read( mTxLoChan, "frequency", freqString, sizeof( freqString ) ) > 0 );

    if( status )
    {
        mTxLoFrequency = std::stoll( freqString );
    }

    aFrequency = mTxLoFrequency;
    return status;
}


//!************************************************************************
//! Get the Tx LO frequency parameters
//!
//! @returns true if the parameter can be read
//!************************************************************************
bool AdiTrxAd9361::getTxLoFrequencyParams()
{
    char freqAvailableString[256];
    bool status = ( iio_channel_attr_read( mTxLoChan, "frequency_available", freqAvailableString, sizeof( freqAvailableString ) ) > 2 );

    if( status )
    {
        std::string rawStr = freqAvailableString;
        status = extractIntegerRange( rawStr, mTxLoFrequencyParams );
    }

    return status;
}


//!************************************************************************
//! Get the Tx LO frequency range
//!
//! @returns The range for Tx LO frequency
//!************************************************************************
AdiTrx::IntegerRange AdiTrxAd9361::getTxLoFrequencyRange() const
{
    return mTxLoFrequencyParams;
}


//!************************************************************************
//! Get the status of Tx LO power
//!
//! @returns true if the parameter can be read
//!************************************************************************
bool AdiTrxAd9361::getTxLoPower
    (
    bool& aEnable               //!< true if power is enabled
    ) const
{
    aEnable = true;
    long long isPowerDown = 0;
    bool status = ( 0 == iio_channel_attr_read_longlong( mTxLoChan, "powerdown", &isPowerDown ) );

    if( status )
    {
        aEnable = !isPowerDown;
    }

    return status;
}


//!************************************************************************
//! Get the Tx NCO gain scale
//! It applies to AD9081/AD9082 only.
//!
//! @returns true if the parameter can be read
//!************************************************************************
bool AdiTrxAd9361::getTxNcoGainScale
    (
    double& aGainScale          //!< gain scale [0..1]
    )
{
    aGainScale = 0;
    return false;
}


//!************************************************************************
//! Get the Tx sampling frequency [Hz]
//!
//! @returns true if the parameter can be read
//!************************************************************************
bool AdiTrxAd9361::getTxSamplingFrequency
    (
    int64_t& aFrequency         //!< sampling frequency [Hz]
    )
{
    mTxSamplingFrequency = 0;
    char samplingFreqString[256];
    bool status = ( iio_channel_attr_read( mTxPhyChan, "sampling_frequency", samplingFreqString, sizeof( samplingFreqString ) ) > 0 );

    if( status )
    {
        mTxSamplingFrequency = std::stoll( samplingFreqString );
    }

    aFrequency = mTxSamplingFrequency;
    return status;
}


//!************************************************************************
//! Get the Tx sampling frequency parameters
//!
//! @returns true if the parameters can be read
//!************************************************************************
bool AdiTrxAd9361::getTxSamplingFrequencyParams()
{
    char samplingFreqAvailableString[256];
    bool status = ( iio_channel_attr_read( mTxPhyChan, "sampling_frequency_available", samplingFreqAvailableString, sizeof( samplingFreqAvailableString ) ) > 2 );

    if( status )
    {
        std::string rawStr = samplingFreqAvailableString;
        status = extractIntegerRange( rawStr, mTxSamplingFrequencyParams );
    }

    return status;
}


//!************************************************************************
//! Get the Tx sampling frequency range
//!
//! @returns The range for Tx sampling frequency
//!************************************************************************
AdiTrx::IntegerRange AdiTrxAd9361::getTxSamplingFrequencyRange() const
{
    return mTxSamplingFrequencyParams;
}


//!************************************************************************
//! Initialize the transceiver
//!
//! @returns true if the transceiver can be initialized
//!************************************************************************
bool AdiTrxAd9361::initialize
    (
    const std::string aUri      //!< URI
    )
{
    mIioContext = iio_create_context_from_uri( aUri.c_str() );
    bool status = nullptr != mIioContext;

    if( status )
    {
        status = iio_context_get_devices_count( mIioContext ) > 0;
    }

    // devices: phy, Tx streaming
    if( status )
    {
        mPhyDev = iio_context_find_device( mIioContext, AD9361_PHY_DEV_STR.c_str() );
        status = nullptr != mPhyDev;
    }

    if( status )
    {
        mTxDev = iio_context_find_device( mIioContext, AD9361_TX_DEV_STR.c_str() );
        status = nullptr != mTxDev;
    }

    // channels: Tx phy, Tx LO
    if( status )
    {
        const bool IS_OUTPUT_CH = true;
        mTxPhyChan = iio_device_find_channel( mPhyDev, "voltage0", IS_OUTPUT_CH );
        status = nullptr != mTxPhyChan;
    }

    if( status )
    {
        const bool IS_OUTPUT_CH = true;
        mTxLoChan = iio_device_find_channel( mPhyDev, "altvoltage1", IS_OUTPUT_CH );
        status = nullptr != mTxLoChan;
    }

    // channels: Tx streaming
    if( status )
    {
        const bool IS_OUTPUT_CH = true;
        mTx0_I = iio_device_find_channel( mTxDev, "voltage0", IS_OUTPUT_CH );
        status = nullptr != mTx0_I;
    }

    if( status )
    {
        const bool IS_OUTPUT_CH = true;
        mTx0_Q = iio_device_find_channel( mTxDev, "voltage1", IS_OUTPUT_CH );
        status = nullptr != mTx0_I;
    }

    // enable channels
    if( status )
    {
        iio_channel_enable( mTx0_I );
        iio_channel_enable( mTx0_Q );
    }

    // kernel buffers count
    if( status )
    {
        const unsigned int NR_BUFFERS = 1;
        status = 0 == iio_device_set_kernel_buffers_count( mTxDev, NR_BUFFERS );
    }

    // create buffer
    if( status )
    {
        status = resetTxBuffer( 0, true );
    }

    //*******************
    // get parameters
    //*******************
    if( status )
    {
        status = getTxBandwidthParams();
    }

    if( status )
    {
        status = getTxSamplingFrequencyParams();
    }

    if( status )
    {
        status = getTxLoFrequencyParams();
    }

    if( status )
    {
        status = getTxHwGainParams();
    }

    //*******************
    // set parameters
    //*******************
    if( status )
    {        
        status = setTxHwGain( mTxHwGainDbParams.max );
    }

    if( status )
    {
        status = setTxBandwidth( mTxBandwidthParams.max );
    }

    if( status )
    {
        status = setTxSamplingFrequency( 2500000 );
    }

    mInitialized = status;
    return status;
}


//!************************************************************************
//! Set the Tx bandwidth [Hz]
//!
//! @returns true if the setting can be applied
//!************************************************************************
bool AdiTrxAd9361::setTxBandwidth
    (
    const int64_t aBandwidth    //!< bandwidth [Hz]
    )
{
    bool status = ( aBandwidth >= mTxBandwidthParams.min
                 && aBandwidth <= mTxBandwidthParams.max );

    if( status )
    {
        status = ( 0 == iio_channel_attr_write_longlong( mTxPhyChan, "rf_bandwidth", aBandwidth ) );
    }

    if( status )
    {
        mTxBandwidth = aBandwidth;
    }

    return status;
}


//!************************************************************************
//! Set the Tx hardware gain [dB]
//!
//! @returns true if the setting can be applied
//!************************************************************************
bool AdiTrxAd9361::setTxHwGain
    (
    const double& aHwGainDb    //!< hardware gain [dB]
    )
{
    bool status = ( aHwGainDb >= mTxHwGainDbParams.min
                 && aHwGainDb <= mTxHwGainDbParams.max );

    if( status )
    {
        status = ( 0 == iio_channel_attr_write_double( mTxPhyChan, "hardwaregain", aHwGainDb ) );
    }

    if( status )
    {
        mTxHwGainDb = aHwGainDb;
    }

    return status;
}


//!************************************************************************
//! Set the Tx LO frequency [Hz]
//!
//! @returns true if the setting can be applied
//!************************************************************************
bool AdiTrxAd9361::setTxLoFrequency
    (
    const int64_t aFrequency    //!< frequency [Hz]
    )
{
    bool status = ( aFrequency >= mTxLoFrequencyParams.min
                 && aFrequency <= mTxLoFrequencyParams.max );

    if( status )
    {
        status = ( 0 == iio_channel_attr_write_longlong( mTxLoChan, "frequency", aFrequency ) );
    }

    if( status )
    {
        mTxLoFrequency = aFrequency;
    }

    return status;
}


//!************************************************************************
//! Enable or disable the Tx LO power
//!
//! @returns true if the setting can be applied
//!************************************************************************
bool AdiTrxAd9361::setTxLoPower
    (
    const bool aEnable          //!< true for enabling power
    )
{
    bool status = ( 0 == iio_channel_attr_write_longlong( mTxLoChan, "powerdown", aEnable ? 0 : 1 ) );
    return status;
}


//!************************************************************************
//! Set the Tx NCO gain scale
//! It applies to AD9081/AD9082 only.
//!
//! @returns true if the setting can be applied
//!************************************************************************
bool AdiTrxAd9361::setTxNcoGainScale
    (
    const double aGainScale     //!< gain scale [0..1]
    )
{
    Q_UNUSED( aGainScale );
    return false;
}


//!************************************************************************
//! Set the Tx sampling frequency [Hz]
//!
//! @returns true if the setting can be applied
//!************************************************************************
bool AdiTrxAd9361::setTxSamplingFrequency
    (
    const int64_t aFrequency    //!< frequency [Hz]
    )
{
    bool status = ( aFrequency >= mTxSamplingFrequencyParams.min
                 && aFrequency <= mTxSamplingFrequencyParams.max );

    if( status )
    {
        status = ( 0 == iio_channel_attr_write_longlong( mTxPhyChan, "sampling_frequency", aFrequency ) );
    }

    if( status )
    {
        mTxSamplingFrequency = aFrequency;
    }

    return status;
}


//!************************************************************************
//! Start Tx streaming
//!
//! @returns nothing
//!************************************************************************
void AdiTrxAd9361::startTxStreaming()
{
    bool status = resetTxBuffer( mFrameLength * mFramesNr, true );

    if( status )
    {
        std::ptrdiff_t pBufStep = iio_buffer_step( mTxBuf );
        uint8_t* pBufEnd = static_cast< uint8_t* >( iio_buffer_end( mTxBuf ) );
        uint8_t* dataBuf;
        uint32_t i = 0;
        uint16_t crtFrame = 0;
        uint16_t crtPoint = 0;
        const double SCALE_RATIO = 2047.0 / mSignalData.maxVal;
#if DUMP_FRAMES_TO_FILE
        const uint16_t NR_OF_FRAMES_TO_DUMP = 2;
        char crtLine[80] = "";
        std::ofstream dumpFile( mDumpFilename );

        if( !dumpFile.is_open() )
        {
            std::cout << "Cannot create dump file " << mDumpFilename << std::endl;
        }
#endif

        for( dataBuf = static_cast< uint8_t* >( iio_buffer_first( mTxBuf, mTx0_I ) );
             dataBuf < pBufEnd;
             dataBuf += pBufStep
           )
        {
            crtFrame = i / mFrameLength;
            crtPoint = crtFrame ? ( i % mFrameLength ) : i;

            Dataset::IQPoint pt = mSignalData.frameDataVec.at( crtFrame ).at( crtPoint );

            // AD9361 => 12-bit DAC
            reinterpret_cast< int16_t* >( dataBuf )[0] = ( static_cast<int16_t>( pt.i * SCALE_RATIO ) ) << 4;
            reinterpret_cast< int16_t* >( dataBuf )[1] = ( static_cast<int16_t>( pt.q * SCALE_RATIO ) ) << 4;
            i++;

#if DUMP_FRAMES_TO_FILE
            if( crtFrame < NR_OF_FRAMES_TO_DUMP )
            {
                if( dumpFile.is_open() )
                {
                    sprintf( crtLine, "%u %lf %lf\n", i - 1,  pt.i / mSignalData.maxVal, pt.q / mSignalData.maxVal );
                    dumpFile.write( crtLine, strlen( crtLine ) );
                }
            }
#endif
        }

#if DUMP_FRAMES_TO_FILE
        if( dumpFile.is_open() )
        {
            dumpFile.close();
        }
#endif
        iio_buffer_push( mTxBuf );
    }
}


//!************************************************************************
//! Stop Tx streaming
//!
//! @returns nothing
//!************************************************************************
void AdiTrxAd9361::stopTxStreaming()
{
    bool status = resetTxBuffer( 1024, true );

    if( status )
    {
        std::ptrdiff_t pBufStep = iio_buffer_step( mTxBuf );
        uint8_t* pBufEnd = static_cast< uint8_t* >( iio_buffer_end( mTxBuf ) );
        uint8_t* dataBuf;

        for( dataBuf = static_cast< uint8_t* >( iio_buffer_first( mTxBuf, mTx0_I ) );
             dataBuf < pBufEnd;
             dataBuf += pBufStep
           )
        {
            reinterpret_cast< int16_t* >( dataBuf )[0] = 0;
            reinterpret_cast< int16_t* >( dataBuf )[1] = 0;
        }

        iio_buffer_push( mTxBuf );
    }
}
