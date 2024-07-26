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
AdiTrxAd9081.cpp

This file contains the sources for AD9081/AD9082 transceivers.
*/

#include "AdiTrxAd9081.h"

#include <cstring>

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
AdiTrxAd9081::AdiTrxAd9081()
{
}


//!************************************************************************
//! Get the Tx bandwidth [Hz]
//!
//! @returns true if the parameter can be read
//!************************************************************************
bool AdiTrxAd9081::getTxBandwidth
    (
    int64_t& aBandwidth         //!< bandwidth [Hz]
    )
{
    aBandwidth = 0;
    return true;
}


//!************************************************************************
//! Get the Tx bandwidth parameters
//!
//! @returns true if the parameters can be read
//!************************************************************************
bool AdiTrxAd9081::getTxBandwidthParams()
{
    return false;
}


//!************************************************************************
//! Get the Tx bandwidth range
//!
//! @returns The range for Rx bandwidth
//!************************************************************************
AdiTrx::IntegerRange AdiTrxAd9081::getTxBandwidthRange() const
{
    return mTxBandwidthParams;
}


//!************************************************************************
//! Get the Tx hardware gain [dB]
//!
//! @returns true if the parameter can be read
//!************************************************************************
bool AdiTrxAd9081::getTxHwGain
    (
    double& aHwGainDb       //!< hardware gain [dB]
    )
{
    aHwGainDb = 0;
    return true;
}


//!************************************************************************
//! Get the Tx hardware gain parameters
//!
//! @returns true if the parameters can be read
//!************************************************************************
bool AdiTrxAd9081::getTxHwGainParams()
{
    return false;
}


//!************************************************************************
//! Get the Tx LO frequency [Hz]
//!
//! @returns true if the parameter can be read
//!************************************************************************
bool AdiTrxAd9081::getTxLoFrequency
    (
    int64_t& aFrequency         //!< frequency [Hz]
    )
{
    mTxLoFrequency = 0;
    char freqString[256];
    bool status = ( iio_channel_attr_read( mTxNcoChan, "main_nco_frequency", freqString, sizeof( freqString ) ) > 0 );

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
bool AdiTrxAd9081::getTxLoFrequencyParams()
{
    char freqAvailableString[256];
    bool status = ( iio_channel_attr_read( mTxNcoChan, "main_nco_frequency_available", freqAvailableString, sizeof( freqAvailableString ) ) > 2 );

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
AdiTrx::IntegerRange AdiTrxAd9081::getTxLoFrequencyRange() const
{
    return mTxLoFrequencyParams;
}


//!************************************************************************
//! Get the status of Tx LO power
//!
//! @returns true if the parameter can be read
//!************************************************************************
bool AdiTrxAd9081::getTxLoPower
    (
    bool& aEnable               //!< true if power is enabled
    ) const
{
    aEnable = true;
    long long isEnabled = 0;
    bool status = ( 0 == iio_channel_attr_read_longlong( mTxNcoChan, "en", &isEnabled ) );

    if( status )
    {
        aEnable = isEnabled;
    }

    return status;
}


//!************************************************************************
//! Get the Tx NCO gain scale
//!
//! @returns true if the parameter can be read
//!************************************************************************
bool AdiTrxAd9081::getTxNcoGainScale
    (
    double& aGainScale          //!< gain scale [0..1]
    )
{
    mTxNcoGainScale = 0;
    double gainScale = 0;
    bool status = ( 0 == iio_channel_attr_read_double( mTxNcoChan, "channel_nco_gain_scale", &gainScale ) );

    if( status )
    {
        mTxNcoGainScale = gainScale;
    }

    aGainScale = mTxNcoGainScale;
    return status;
}


//!************************************************************************
//! Get the Tx sampling frequency [Hz]
//!
//! @returns true if the parameter can be read
//!************************************************************************
bool AdiTrxAd9081::getTxSamplingFrequency
    (
    int64_t& aFrequency         //!< sampling frequency [Hz]
    )
{
    mTxSamplingFrequency = 0;
    char samplingFreqString[256];
    bool status = ( iio_channel_attr_read( mTxNcoChan, "sampling_frequency", samplingFreqString, sizeof( samplingFreqString ) ) > 0 );

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
bool AdiTrxAd9081::getTxSamplingFrequencyParams()
{
    memset( &mTxSamplingFrequencyParams, 0, sizeof( mTxSamplingFrequencyParams ) );
    int64_t samplingFrequency = 0;
    bool status = getTxSamplingFrequency( samplingFrequency );

    if( status )
    {
        mTxSamplingFrequencyParams.max = samplingFrequency;
        mTxSamplingFrequencyParams.min = mTxSamplingFrequencyParams.max;
        mTxSamplingFrequencyParams.step = 0;
    }   

    return status;
}


//!************************************************************************
//! Get the Tx sampling frequency range
//!
//! @returns The range for Tx sampling frequency
//!************************************************************************
AdiTrx::IntegerRange AdiTrxAd9081::getTxSamplingFrequencyRange() const
{
    return mTxSamplingFrequencyParams;
}


//!************************************************************************
//! Initialize the transceiver
//!
//! @returns true if the transceiver can be initialized
//!************************************************************************
bool AdiTrxAd9081::initialize
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

    // devices: Tx and Rx streaming
    if( status )
    {
        mTxDev = iio_context_find_device( mIioContext, AD9081_TX_DEV_STR.c_str() );
        status = nullptr != mTxDev;
    }

    if( status )
    {
        mRxDev = iio_context_find_device( mIioContext, AD9081_RX_DEV_STR.c_str() );
        status = nullptr != mRxDev;
    }

    // channels: Tx NCO
    if( status )
    {
        const bool IS_OUTPUT_CH = true;
        mTxNcoChan = iio_device_find_channel( mRxDev, "voltage0_i", IS_OUTPUT_CH );
        status = nullptr != mTxNcoChan;
    }

    // channels: Tx streaming
    if( status )
    {
        const bool IS_OUTPUT_CH = true;
        mTx0_I = iio_device_find_channel( mTxDev, "voltage0_i", IS_OUTPUT_CH );
        status = nullptr != mTx0_I;
    }

    if( status )
    {
        const bool IS_OUTPUT_CH = true;
        mTx0_Q = iio_device_find_channel( mTxDev, "voltage0_q", IS_OUTPUT_CH );
        status = nullptr != mTx0_Q;
    }

    // enable channels
    if( status )
    {
        iio_channel_enable( mTx0_I );
        iio_channel_enable( mTx0_Q );
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
        status = getTxLoFrequencyParams();
    }

    //*******************
    // set parameters
    //*******************
    if( status )
    {
        status = ( 0 == iio_channel_attr_write_longlong( mTxNcoChan, "channel_nco_frequency", 0 ) );
    }

    if( status )
    {
        status = ( 0 == iio_channel_attr_write_longlong( mTxNcoChan, "channel_nco_phase", 0 ) );
    }

    if( status )
    {
        status = ( 0 == iio_channel_attr_write_longlong( mTxNcoChan, "channel_nco_test_tone_en", 0 ) );
    }

    if( status )
    {
        status = ( 0 == iio_channel_attr_write_double( mTxNcoChan, "channel_nco_test_tone_scale", 0 ) );
    }

    if( status )
    {
        status = setTxLoPower( true );
    }

    if( status )
    {
        status = ( 0 == iio_channel_attr_write_longlong( mTxNcoChan, "main_nco_phase", 0 ) );
    }

    if( status )
    {
        status = ( 0 == iio_channel_attr_write_longlong( mTxNcoChan, "main_nco_test_tone_en", 0 ) );
    }

    if( status )
    {
        status = ( 0 == iio_channel_attr_write_longlong( mTxNcoChan, "main_nco_test_tone_scale", 0 ) );
    }

    mInitialized = status;
    return status;
}


//!************************************************************************
//! Set the Tx bandwidth [Hz]
//!
//! @returns true if the setting can be applied
//!************************************************************************
bool AdiTrxAd9081::setTxBandwidth
    (
    const int64_t aBandwidth    //!< bandwidth [Hz]
    )
{
    Q_UNUSED( aBandwidth );
    return false;
}


//!************************************************************************
//! Set the Tx hardware gain [dB]
//!
//! @returns true if the setting can be applied
//!************************************************************************
bool AdiTrxAd9081::setTxHwGain
    (
    const double& aHwGainDb    //!< hardware gain [dB]
    )
{
    Q_UNUSED( aHwGainDb );
    return false;
}


//!************************************************************************
//! Set the Tx LO frequency [Hz]
//!
//! @returns true if the setting can be applied
//!************************************************************************
bool AdiTrxAd9081::setTxLoFrequency
    (
    const int64_t aFrequency    //!< frequency [Hz]
    )
{
    bool status = ( aFrequency >= mTxLoFrequencyParams.min
                 && aFrequency <= mTxLoFrequencyParams.max );

    if( status )
    {
        status = ( 0 == iio_channel_attr_write_longlong( mTxNcoChan, "main_nco_frequency", aFrequency ) );
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
bool AdiTrxAd9081::setTxLoPower
    (
    const bool aEnable          //!< true for enabling power
    )
{
    bool status = ( 0 == iio_channel_attr_write_longlong( mTxNcoChan, "en", aEnable ? 1 : 0 ) );
    return status;
}


//!************************************************************************
//! Set the Tx NCO gain scale
//!
//! @returns true if the setting can be applied
//!************************************************************************
bool AdiTrxAd9081::setTxNcoGainScale
    (
    const double aGainScale     //!< gain scale [0..1]
    )
{
    bool status = ( aGainScale >= 0 && aGainScale <= 1 );

    if( status )
    {
        status = ( 0 == iio_channel_attr_write_double( mTxNcoChan, "channel_nco_gain_scale", aGainScale ) );
    }

    return status;
}


//!************************************************************************
//! Set the Tx sampling frequency [Hz]
//!
//! @returns true if the setting can be applied
//!************************************************************************
bool AdiTrxAd9081::setTxSamplingFrequency
    (
    const int64_t aFrequency    //!< frequency [Hz]
    )
{
    Q_UNUSED( aFrequency );
    return false;
}


//!************************************************************************
//! Start Tx streaming
//!
//! @returns nothing
//!************************************************************************
void AdiTrxAd9081::startTxStreaming()
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
        const double SCALE_RATIO = 32767.0 / mSignalData.maxVal;
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

            // AD9081 => 16-bit DAC
            reinterpret_cast< int16_t* >( dataBuf )[0] = ( static_cast<int16_t>( pt.i * SCALE_RATIO ) );
            reinterpret_cast< int16_t* >( dataBuf )[1] = ( static_cast<int16_t>( pt.q * SCALE_RATIO ) );
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
void AdiTrxAd9081::stopTxStreaming()
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
