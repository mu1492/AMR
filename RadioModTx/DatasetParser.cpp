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
DatasetParser.cpp

This file contains the sources for dataset parser.
*/

#include "DatasetParser.h"

#include <algorithm>
#include <unordered_set>


//!************************************************************************
//! Constructor
//!************************************************************************
DatasetParser::DatasetParser()
    : mStatus( true )
    , mSingleModulation( Modulation::NAME_UNKNOWN )
{
}


//!************************************************************************
//! Get the dataset map
//!
//! @returns The map containing signal data for all modulation-SNR combinations
//!************************************************************************
Dataset::ModulationSnrSignalDataMap DatasetParser::getMap
    (
    bool& aStatus           //!< status
    ) const
{
    aStatus = mStatus;
    return mMap;
}


//!************************************************************************
//! Get the vector with unique modulations in the dataset
//!
//! @returns The vector with modulations
//!************************************************************************
std::vector<Modulation::ModulationName> DatasetParser::getUniqueModVec() const
{
    return mUniqueModVec;
}


//!************************************************************************
//! Get the vector with unique SNRs in the dataset
//!
//! @returns The vector with SNRs
//!************************************************************************
std::vector<int> DatasetParser::getUniqueSnrVec() const
{
    return mUniqueSnrVec;
}


//!************************************************************************
//! Remove the duplicates and sorts a vector
//!
//! @returns The vector without duplicates
//!************************************************************************
void DatasetParser::removeDuplicates
    (
    std::vector<int>& aVector   //!< vector
    )
{
    std::unordered_set<int> uSet;

    for( int i: aVector )
    {
        uSet.insert( i );
    }

    aVector.assign( uSet.begin(), uSet.end() );
    std::sort( aVector.begin(), aVector.end() );
}


//!************************************************************************
//! Remove the duplicates and sorts a vector
//!
//! @returns The vector without duplicates
//!************************************************************************
void DatasetParser::removeDuplicates
    (
    std::vector<Modulation::ModulationName>& aVector   //!< vector
    )
{
    std::unordered_set<Modulation::ModulationName> uSet;

    for( Modulation::ModulationName i: aVector )
    {
        uSet.insert( i );
    }

    aVector.assign( uSet.begin(), uSet.end() );
    std::sort( aVector.begin(), aVector.end() );
}


//!************************************************************************
//! Set the filename
//!
//! @returns nothing
//!************************************************************************
void DatasetParser::setFile
    (
    std::string aFileName   //!< input filename
    )
{
    mFileName = aFileName;
}


//!************************************************************************
//! Set the single modulation
//!
//! @returns nothing
//!************************************************************************
void DatasetParser::setSingleModulation
    (
    Modulation::ModulationName aModulation  //!< modulation
    )
{
    mSingleModulation = aModulation;
}
