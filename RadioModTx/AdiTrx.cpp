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

This file contains the sources for ADI transceivers.
*/

#include "AdiTrx.h"

#include <cstdlib>
#include <cstring>
#include <iostream>


//!************************************************************************
//! Constructor
//!************************************************************************
AdiTrx::AdiTrx()
    // context
    : mIioContext( nullptr )
    // devices
    , mPhyDev( nullptr )
    , mTxDev( nullptr )
    , mRxDev( nullptr )
    // channels
    , mTxPhyChan( nullptr )
    , mTxLoChan( nullptr )
    , mTxNcoChan( nullptr )
    , mTx0_I( nullptr )
    , mTx0_Q( nullptr )
    // Tx Buffer
    , mTxBuf( nullptr )
    , mTxBufIqPairsCount( 0 )
    // settings
    , mTxBandwidth( 0 )
    , mTxSamplingFrequency( 0 )
    , mTxLoFrequency( 0 )
    , mTxHwGainDb( 0 )
    , mTxNcoGainScale( 0 )
    // init status
    , mInitialized( false )
    // signal data
    , mFrameLength( 0 )
    , mFramesNr( 0 )
{
    memset( &mTxBandwidthParams, 0, sizeof( mTxBandwidthParams ) );
    memset( &mTxSamplingFrequencyParams, 0, sizeof( mTxSamplingFrequencyParams ) );
    memset( &mTxLoFrequencyParams, 0, sizeof( mTxLoFrequencyParams ) );
    memset( &mTxHwGainDbParams, 0, sizeof( mTxHwGainDbParams ) );
}


//!************************************************************************
//! Destructor
//!************************************************************************
AdiTrx::~AdiTrx()
{
    freeResources();
}


//!************************************************************************
//! Extract a double value from a string, based on a substring index
//!
//! @returns true if a double value can be extracted
//!************************************************************************
bool AdiTrx::extractDouble
    (
    const std::string   aString,    //!< string
    const size_t        aIndex,     //!< substring index
    double&             aValue      //!< value
    )
{
    aValue = 0;
    const size_t STR_SIZE = aString.size();
    bool status = STR_SIZE > 0;

    if( status )
    {
        std::vector<std::string> vec;
        int start = 0;
        int end = 0;

        while( ( start = aString.find_first_not_of( ' ', end ) ) != std::string::npos )
        {
            end = aString.find( ' ', start );
            vec.push_back( aString.substr( start, end - start ) );
        }

        status = ( vec.size() > aIndex );

        if( status )
        {
            aValue = std::atof( vec.at( aIndex ).c_str() );
        }
    }

    return status;
}


//!************************************************************************
//! Extract a double limit from a string
//!
//! @returns true if a limit can be extracted
//!************************************************************************
bool AdiTrx::extractDoubleLimit
    (
    std::string                 aString,    //!< string
    double&                     aValue      //!< value
    )
{
    aValue = 0;
    const size_t STR_SIZE = aString.size();
    bool status = STR_SIZE > 2;

    if( status )
    {
        if( '[' == aString.at( 0 )
         && ']' == aString.at( STR_SIZE - 1 ) )
        {
            aString = aString.substr( 1, aString.size() - 2 );
        }

        std::vector<std::string> vec;
        int start = 0;
        int end = 0;

        while( ( start = aString.find_first_not_of( ' ', end ) ) != std::string::npos )
        {
            end = aString.find( ' ', start );
            vec.push_back( aString.substr( start, end - start ) );
        }

        status = ( 1 == vec.size() );

        if( status )
        {
            aValue = std::atof( vec.at( 0 ).c_str() );
        }
    }

    return status;
}


//!************************************************************************
//! Extract an integer from a string, based on a substring index
//!
//! @returns true if an integer can be extracted
//!************************************************************************
bool AdiTrx::extractInteger
    (
    const std::string   aString,    //!< string
    const size_t        aIndex,     //!< substring index
    int64_t&            aValue      //!< value
    )
{
    aValue = 0;
    const size_t STR_SIZE = aString.size();
    bool status = STR_SIZE > 0;

    if( status )
    {
        std::vector<std::string> vec;
        int start = 0;
        int end = 0;

        while( ( start = aString.find_first_not_of( ' ', end ) ) != std::string::npos )
        {
            end = aString.find( ' ', start );
            vec.push_back( aString.substr( start, end - start ) );
        }

        status = ( vec.size() > aIndex );

        if( status )
        {
            aValue = static_cast<int64_t>( std::atof( vec.at( aIndex ).c_str() ) );
        }
    }

    return status;
}


//!************************************************************************
//! Extract an integer range from a string
//!
//! @returns true if a range can be extracted
//!************************************************************************
bool AdiTrx::extractIntegerRange
    (
    std::string                 aString,    //!< string
    AdiTrx::IntegerRange&       aRange      //!< range
    )
{
    memset( &aRange, 0, sizeof( aRange ) );
    const size_t STR_SIZE = aString.size();
    bool status = STR_SIZE > 2;

    if( status )
    {
        if( '[' == aString.at( 0 )
         && ']' == aString.at( STR_SIZE - 1 ) )
        {
            aString = aString.substr( 1, aString.size() - 2 );
        }

        std::vector<std::string> vec;
        int start = 0;
        int end = 0;

        while( ( start = aString.find_first_not_of( ' ', end ) ) != std::string::npos )
        {
            end = aString.find( ' ', start );
            vec.push_back( aString.substr( start, end - start ) );
        }

        status = ( 3 == vec.size() );

        if( status )
        {
            aRange.min  = std::stoll( vec.at( 0 ).c_str() );
            aRange.step = std::stoll( vec.at( 1 ).c_str() );
            aRange.max  = std::stoll( vec.at( 2 ).c_str() );

            status = ( aRange.min < aRange.max
                    && aRange.step < ( aRange.max - aRange.min ) );
        }
    }

    return status;
}


//!************************************************************************
//! Free the allocated resources
//!
//! @returns nothing
//!************************************************************************
void AdiTrx::freeResources()
{
    if( mTxBuf )
    {
        iio_buffer_destroy( mTxBuf );
        mTxBuf = nullptr;
    }

    if( mTx0_I )
    {
        iio_channel_disable( mTx0_I );
        mTx0_I = nullptr;
    }
    if( mTx0_Q )
    {
        iio_channel_disable( mTx0_Q );
        mTx0_Q = nullptr;
    }

    if( mTxNcoChan )
    {
        iio_channel_disable( mTxNcoChan );
        mTxNcoChan = nullptr;
    }
    if( mTxLoChan )
    {
        iio_channel_disable( mTxLoChan );
        mTxLoChan = nullptr;
    }
    if( mTxPhyChan )
    {
        iio_channel_disable( mTxPhyChan );
        mTxPhyChan = nullptr;
    }

    if( mIioContext )
    {
        iio_context_destroy( mIioContext );
        mIioContext = nullptr;
    }
}


//!************************************************************************
//! Get the name of the file where to dump data
//!
//! @returns nothing
//!************************************************************************
void AdiTrx::getDumpFilename
    (
    std::string&                aFilename   //!< filename
    )
{
    mDumpFilename = aFilename;
}


//!************************************************************************
//! Get the signal data for a modulation-SNR combination
//!
//! @returns nothing
//!************************************************************************
void AdiTrx::getSignalData
    (
    Dataset::SignalData&        aSignalData //!< signal data for a modulation-SNR combination
    )
{
    mSignalData = aSignalData;
    mFramesNr = aSignalData.frameDataVec.size();
    mFrameLength = aSignalData.frameDataVec.at( 0 ).size();
}


//!************************************************************************
//! Check if the transceiver is initialized
//!
//! @returns true if initialized
//!************************************************************************
bool AdiTrx::isInitialized() const
{
    return mInitialized;
}


//!************************************************************************
//! Read a byte from a register
//!
//! @returns true if the value can be read
//!************************************************************************
bool AdiTrx::readRegister
    (
    const uint16_t aAddress,    //!< register address
    uint8_t&       aValue       //!< read value
    )
{
    uint32_t readValue = 0;
    bool status = ( 0 == iio_device_reg_read( mPhyDev, aAddress, &readValue ) );
    aValue = status ? ( readValue & 0xff ) : 0;
    return status;
}


//!************************************************************************
//! Reset the Tx buffer
//!
//! @returns true if the buffer can be reset
//!************************************************************************
bool AdiTrx::resetTxBuffer
    (
    const size_t aLength,       //!< new buffer length
    const bool   aIsCyclic      //!< true if cyclic
    )
{
    bool status = true;
    mTxBufIqPairsCount = aLength;

    if( mTxBuf )
    {
        iio_buffer_destroy( mTxBuf );
        mTxBuf = nullptr;
    }

    if( mTxBufIqPairsCount )
    {
        mTxBuf = iio_device_create_buffer( mTxDev, mTxBufIqPairsCount, aIsCyclic );
        status = nullptr != mTxBuf;
    }

    return status;
}


//!************************************************************************
//! Write a byte to a register
//!
//! @returns true if the value can be written
//!************************************************************************
bool AdiTrx::writeRegister
    (
    const uint16_t aAddress,    //!< register address
    const uint8_t  aValue       //!< value to write
    )
{
    return ( 0 == iio_device_reg_write( mPhyDev, aAddress, aValue ) );
}
