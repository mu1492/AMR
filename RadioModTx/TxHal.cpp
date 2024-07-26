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
TxHal.cpp

This file contains the sources for transmitter HAL.
*/

#include "TxHal.h"

#include <algorithm>
#include <cstring>
#include <iostream>


TxHal* TxHal::sInstance = nullptr;

const std::map<TxHal::TxDevice, std::vector<std::string>> TxHal::TX_DEVICE_NAME_IDS =
{
    { TxHal::TX_DEVICE_UNKNOWN,     { "" } },

    { TxHal::TX_DEVICE_AD9361,      { "AD936", "PLUTO", "Pluto" } },
    { TxHal::TX_DEVICE_AD9081,      { "AD9081", "AD9082" } },
    { TxHal::TX_DEVICE_ADRV9009,    { "ADRV9009" } }
};

const std::string TxHal::DEFAULT_IP_URI = "ip:10.0.0.2";

//!************************************************************************
//! Constructor
//!************************************************************************
TxHal::TxHal()
    : mTxDevice( TX_DEVICE_UNKNOWN )
    , mIsInitialized( false )
{
    updateIioScanContexts();
}


//!************************************************************************
//! Singleton
//!
//! @returns the instance of the object
//!************************************************************************
TxHal* TxHal::getInstance()
{
    if( !sInstance )
    {
        sInstance = new TxHal;
    }

    return sInstance;
}


//!************************************************************************
//! Get the signal data for a modulation-SNR combination
//!
//! @returns nothing
//!************************************************************************
void TxHal::getData
    (
    Dataset::SignalData& aSignalData    //!< signal data for a modulation-SNR combination
    )
{
    switch( mTxDevice )
    {
        case TX_DEVICE_AD9361:
            mTrxAd9361.getSignalData( aSignalData );
            break;

        case TX_DEVICE_AD9081:
            mTrxAd9081.getSignalData( aSignalData );
            break;

        case TX_DEVICE_ADRV9009:
            mTrxAdrv9009.getSignalData( aSignalData );
            break;

        default:
            break;
    }
}


//!************************************************************************
//! Get the name of the file where to dump data
//!
//! @returns nothing
//!************************************************************************
void TxHal::getDumpFilename
    (
    std::string& aFilename      //!< filename
    )
{
    switch( mTxDevice )
    {
        case TX_DEVICE_AD9361:
            mTrxAd9361.getDumpFilename( aFilename );
            break;

        case TX_DEVICE_AD9081:
            mTrxAd9081.getDumpFilename( aFilename );
            break;

        case TX_DEVICE_ADRV9009:
            mTrxAdrv9009.getDumpFilename( aFilename );
            break;

        default:
            break;
    }
}


//!************************************************************************
//! Get the IIO scan contexts
//!
//! @returns The vector with IIO scan contexts
//!************************************************************************
std::vector<TxHal::IioScanContext> TxHal::getIioScanContexts() const
{
    return mIioScanContextsVec;
}


//!************************************************************************
//! Get the Tx bandwidth [Hz]
//!
//! @returns true if the parameter can be read
//!************************************************************************
bool TxHal::getTxBandwidth
    (
    int64_t& aBandwidth         //!< bandwidth [Hz]
    )
{
    bool status = false;

    switch( mTxDevice )
    {
        case TX_DEVICE_AD9361:
            status = mTrxAd9361.getTxBandwidth( aBandwidth );
            break;

        case TX_DEVICE_AD9081:
            status = mTrxAd9081.getTxBandwidth( aBandwidth );
            break;

        case TX_DEVICE_ADRV9009:
            status = mTrxAdrv9009.getTxBandwidth( aBandwidth );
            break;

        default:
            break;
    }

    return status;
}


//!************************************************************************
//! Get the Tx device
//!
//! @returns The Tx device
//!************************************************************************
TxHal::TxDevice TxHal::getTxDevice() const
{
    return mTxDevice;
}


//!************************************************************************
//! Get the Tx hardware gain [dB]
//!
//! @returns true if the parameter can be read
//!************************************************************************
bool TxHal::getTxHwGain
    (
    double& aHwGainDb           //!< hardware gain [dB]
    )
{
    bool status = false;

    switch( mTxDevice )
    {
        case TX_DEVICE_AD9361:
            status = mTrxAd9361.getTxHwGain( aHwGainDb );
            break;

        case TX_DEVICE_AD9081:
            status = mTrxAd9081.getTxHwGain( aHwGainDb );
            break;

        case TX_DEVICE_ADRV9009:
            status = mTrxAdrv9009.getTxHwGain( aHwGainDb );
            break;

        default:
            break;
    }

    return status;
}


//!************************************************************************
//! Get the Tx LO frequency [Hz]
//!
//! @returns true if the parameter can be read
//!************************************************************************
bool TxHal::getTxLoFrequency
    (
    int64_t& aFrequency         //!< frequency [Hz]
    )
{
    bool status = false;

    switch( mTxDevice )
    {
        case TX_DEVICE_AD9361:
            status = mTrxAd9361.getTxLoFrequency( aFrequency );
            break;

        case TX_DEVICE_AD9081:
            status = mTrxAd9081.getTxLoFrequency( aFrequency );
            break;

        case TX_DEVICE_ADRV9009:
            status = mTrxAdrv9009.getTxLoFrequency( aFrequency );
            break;

        default:
            break;
    }

    return status;
}


//!************************************************************************
//! Get the Tx LO frequency range
//!
//! @returns The range for Tx LO frequency
//!************************************************************************
AdiTrx::IntegerRange TxHal::getTxLoFrequencyRange() const
{
    AdiTrx::IntegerRange freqRange;
    memset( &freqRange, 0, sizeof( freqRange ) );

    switch( mTxDevice )
    {
        case TX_DEVICE_AD9361:
            freqRange = mTrxAd9361.getTxLoFrequencyRange();
            break;

        case TX_DEVICE_AD9081:
            freqRange = mTrxAd9081.getTxLoFrequencyRange();
            break;

        case TX_DEVICE_ADRV9009:
            freqRange = mTrxAdrv9009.getTxLoFrequencyRange();
            break;

        default:
            break;
    }

    return freqRange;
}


//!************************************************************************
//! Get the Tx NCO gain scale
//!
//! @returns true if the parameter can be read
//!************************************************************************
bool TxHal::getTxNcoGainScale
    (
    double& aGainScale          //!< gain scale [0..1]
    )
{
    bool status = false;

    switch( mTxDevice )
    {
        case TX_DEVICE_AD9361:
            status = mTrxAd9361.getTxNcoGainScale( aGainScale );
            break;

        case TX_DEVICE_AD9081:
            status = mTrxAd9081.getTxNcoGainScale( aGainScale );
            break;

        case TX_DEVICE_ADRV9009:
            status = mTrxAdrv9009.getTxNcoGainScale( aGainScale );
            break;

        default:
            break;
    }

    return status;
}


//!************************************************************************
//! Get the Tx sampling frequency [Hz]
//!
//! @returns true if the parameter can be read
//!************************************************************************
bool TxHal::getTxSamplingFrequency
    (
    int64_t& aFrequency         //!< sampling frequency [Hz]
    )
{
    bool status = false;

    switch( mTxDevice )
    {
        case TX_DEVICE_AD9361:
            status = mTrxAd9361.getTxSamplingFrequency( aFrequency );
            break;

        case TX_DEVICE_AD9081:
            status = mTrxAd9081.getTxSamplingFrequency( aFrequency );
            break;

        case TX_DEVICE_ADRV9009:
            status = mTrxAdrv9009.getTxSamplingFrequency( aFrequency );
            break;

        default:
            break;
    }

    return status;
}


//!************************************************************************
//! Initialize the Tx device
//!
//! @returns true if the Tx device can be initialized
//!************************************************************************
bool TxHal::initializeTxDevice
    (
    const int aIndex        //!< index
    )
{
    if( mIsInitialized )
    {
        switch( mTxDevice )
        {
            case TX_DEVICE_AD9361:
                mTrxAd9361.freeResources();
                break;

            case TX_DEVICE_AD9081:
                mTrxAd9081.freeResources();
                break;

            case TX_DEVICE_ADRV9009:
                mTrxAdrv9009.freeResources();
                break;

            default:
                break;
        }
    }

    bool status = false;
    mTxDevice = TX_DEVICE_UNKNOWN;
    mIsInitialized = false;

    if( aIndex >= 0 && aIndex < mIioScanContextsCount )
    {
        std::string descr = mIioScanContextsVec.at( aIndex ).description;
        bool found = false;

        for( auto mapIt = TX_DEVICE_NAME_IDS.begin(); mapIt != TX_DEVICE_NAME_IDS.end(); mapIt++ )
        {
            if( TX_DEVICE_UNKNOWN == mapIt->first )
            {
                continue;
            }

            std::vector<std::string> strVec = mapIt->second;

            for( size_t strIt = 0; strIt < strVec.size(); strIt++ )
            {
               if( std::string::npos != descr.find( strVec.at( strIt ) ) )
               {
                   mTxDevice = mapIt->first;
                   found = true;
                   break;
               }
            }

            if( found )
            {
                break;
            }
        }

        status = found;
    }

    if( status )
    {
        switch( mTxDevice )
        {
            case TX_DEVICE_AD9361:
                status = mTrxAd9361.initialize( mIioScanContextsVec.at( aIndex ).uri );
                break;

            case TX_DEVICE_AD9081:
                status = mTrxAd9081.initialize( mIioScanContextsVec.at( aIndex ).uri );
                break;

            case TX_DEVICE_ADRV9009:
                status = mTrxAdrv9009.initialize( mIioScanContextsVec.at( aIndex ).uri );
                break;

            default:
                break;
        }
    }

    mIsInitialized = status;
    return status;
}


//!************************************************************************
//! Check if the URI designates an allowed context
//!
//! @returns true for an allowed context
//!************************************************************************
bool TxHal::isAllowedContext
    (
    const std::string aUri  //!< URI
    ) const
{
    bool status = false;

    if( aUri.size() )
    {
        const std::string ALLOWED_PREFIX_URI = "usb";
        size_t len = ALLOWED_PREFIX_URI.size();

        if( 0 == ALLOWED_PREFIX_URI.compare( aUri.substr( 0, len ) ) )
        {
            status = true;
        }

    }

    return status;
}


//!************************************************************************
//! Get the initialized status of the Tx device
//!
//! @returns true if initialized
//!************************************************************************
bool TxHal::isInitialized() const
{
    return mIsInitialized;
}


//!************************************************************************
//! Set the Tx LO frequency [Hz]
//!
//! @returns true if the setting can be applied
//!************************************************************************
bool TxHal::setTxLoFrequency
    (
    const int64_t aFrequency    //!< frequency [Hz]
    )
{
    bool status = false;

    switch( mTxDevice )
    {
        case TX_DEVICE_AD9361:
            status = mTrxAd9361.setTxLoFrequency( aFrequency );
            break;

        case TX_DEVICE_AD9081:
            status = mTrxAd9081.setTxLoFrequency( aFrequency );
            break;

        case TX_DEVICE_ADRV9009:
            status = mTrxAdrv9009.setTxLoFrequency( aFrequency );
            break;

        default:
            break;
    }

    return status;
}


//!************************************************************************
//! Set the Tx NCO gain scale
//!
//! @returns true if the setting can be applied
//!************************************************************************
bool TxHal::setTxNcoGainScale
    (
    const double aGainScale     //!< gain scale [0..1]
    )
{
    bool status = false;

    switch( mTxDevice )
    {
        case TX_DEVICE_AD9361:
            status = mTrxAd9361.setTxNcoGainScale( aGainScale );
            break;

        case TX_DEVICE_AD9081:
            status = mTrxAd9081.setTxNcoGainScale( aGainScale );
            break;

        case TX_DEVICE_ADRV9009:
            status = mTrxAdrv9009.setTxNcoGainScale( aGainScale );
            break;

        default:
            break;
    }

    return status;
}


//!************************************************************************
//! Set the Tx sampling frequency [Hz]
//!
//! @returns true if the setting can be applied
//!************************************************************************
bool TxHal::setTxSamplingFrequency
    (
    const int64_t aFrequency    //!< frequency [Hz]
    )
{
    bool status = false;

    switch( mTxDevice )
    {
        case TX_DEVICE_AD9361:
            status = mTrxAd9361.setTxSamplingFrequency( aFrequency );
            break;

        case TX_DEVICE_AD9081:
            // intentionally do nothing
            break;

        case TX_DEVICE_ADRV9009:
            // intentionally do nothing
            break;

        default:
            break;
    }

    return status;
}


//!************************************************************************
//! Start the streaming
//!
//! @returns nothing
//!************************************************************************
void TxHal::startStreaming()
{
    switch( mTxDevice )
    {
        case TX_DEVICE_AD9361:
            mTrxAd9361.startTxStreaming();
            break;

        case TX_DEVICE_AD9081:
            mTrxAd9081.startTxStreaming();
            break;

        case TX_DEVICE_ADRV9009:
            mTrxAdrv9009.startTxStreaming();
            break;

        default:
            break;
    }
}


//!************************************************************************
//! Stop the streaming
//!
//! @returns nothing
//!************************************************************************
void TxHal::stopStreaming()
{
    switch( mTxDevice )
    {
        case TX_DEVICE_AD9361:
            mTrxAd9361.stopTxStreaming();
            break;

        case TX_DEVICE_AD9081:
            mTrxAd9081.stopTxStreaming();
            break;

        case TX_DEVICE_ADRV9009:
            mTrxAdrv9009.stopTxStreaming();
            break;

        default:
            break;
    }
}


//!************************************************************************
//! Update the IIO scan contexts
//!
//! @returns nothing
//!************************************************************************
void TxHal::updateIioScanContexts()
{
    mIioScanContextsVec.clear();
    struct iio_scan_context* iioScanContext = iio_create_scan_context( nullptr, 0 );

    if( iioScanContext )
    {
        struct iio_context_info** iioContextInfo;
        ssize_t contextsCount = iio_scan_context_get_info_list( iioScanContext, &iioContextInfo );

        IioScanContext isc;

        if( contextsCount > 0 )
        {
            for( size_t i = 0; i < contextsCount; i++ )
            {
                isc.uri = iio_context_info_get_uri( iioContextInfo[i] );

                if( isAllowedContext( isc.uri ) )
                {
                    isc.description = iio_context_info_get_description( iioContextInfo[i] );
                    mIioScanContextsVec.push_back( isc );
                }
            }
        }

        isc.uri = DEFAULT_IP_URI;
        isc.description = "";

        struct iio_context* iioCtx = iio_create_context_from_uri( isc.uri.c_str() );

        if( iioCtx )
        {
            if( iio_context_find_device( iioCtx, "axi-adrv9009-tx-hpc" ) )
            {
                isc.description = TX_DEVICE_NAME_IDS.at( TX_DEVICE_ADRV9009 ).at( 0 );
            }
            else if( iio_context_find_device( iioCtx, "axi-ad9081-tx-hpc" )
                  || iio_context_find_device( iioCtx, "axi-ad9082-tx-hpc" ) )
            {
                isc.description = TX_DEVICE_NAME_IDS.at( TX_DEVICE_AD9081 ).at( 0 );
            }

            iio_context_destroy( iioCtx );

            if( isc.description.size() )
            {
                mIioScanContextsVec.push_back( isc );
            }
        }
    }

    mIioScanContextsCount = mIioScanContextsVec.size();
}


//!************************************************************************
//! Update the sampling frequency
//!
//! @returns nothing
//!************************************************************************
void TxHal::updateSamplingFrequency
    (
    const Dataset::DatasetSource aDatasetSource     //!< dataset
    )
{
    std::vector<uint16_t> frameLengthVec;

    for( auto it = Dataset::FRAME_LENGTH.begin(); it != Dataset::FRAME_LENGTH.end(); it++ )
    {
        frameLengthVec.push_back( it->second );
    }

    uint16_t minFrameLength = *std::min_element( frameLengthVec.begin(), frameLengthVec.end() );

    if( minFrameLength )
    {
        uint16_t frameLength = Dataset::FRAME_LENGTH.at( aDatasetSource );
        double toMinRatio = frameLength / minFrameLength;

        switch( mTxDevice )
        {
            case TX_DEVICE_AD9361:
                {
                    // 2.5 MHz (must be > 2.083 MHz)
                    const int64_t MIN_FREQ_HZ = 2500000;
                    mTrxAd9361.setTxSamplingFrequency( MIN_FREQ_HZ * toMinRatio );
                }
                break;

            case TX_DEVICE_AD9081:
                // intentionally do nothing
                break;

            case TX_DEVICE_ADRV9009:
                // intentionally do nothing
                break;

            default:
                break;
        }
    }
}
